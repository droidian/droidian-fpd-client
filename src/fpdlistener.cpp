// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Droidian Project
//
// Authors:
// Bardia Moshiri <fakeshell@bardia.tech>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <batman/wlrdisplay.h>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QDir>
#include <QStandardPaths>
#include <QSharedPointer>
#include <QFile>
#include "fpdinterface.h"

extern "C" int wlrdisplay_status() {
    int result = wlrdisplay(0, NULL);
    return result != 0;
}

extern "C" int delay(double seconds) {
    return usleep(seconds * 1000000);
}

QString getConfigFile() {
    QFileInfo primaryConfig("/etc/droidian-fpd-client.conf");
    QFileInfo secodaryConfig("/usr/share/droidian-fpd-client/droidian-fpd-client.conf");

    if (primaryConfig.exists()) {
        return primaryConfig.absoluteFilePath();
    } else if (secodaryConfig.exists()) {
        return secodaryConfig.absoluteFilePath();
    } else {
        return "None";
    }
}

QString getVibrationLevel(bool connect) {
    int vibration_level_selected = 1; // default is 1
    QString configFilePath = getConfigFile();
    QString vibraCommand = "";
    // qDebug() << "config file path: " << configFilePath;
    QFile configFile(configFilePath);

    if(configFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&configFile);
        QString line;

        while (in.readLineInto(&line)) {
            if (line.startsWith("vibration_level=")) {
                int vibrationLevel = (line.split("=")[1].trimmed()).toInt();
                // qDebug() << vibrationLevel;
                if (vibrationLevel == 0) {
                    vibration_level_selected = -1;
                    // return empty string for no vibration
                    // qDebug() << "vibration off";
                } else {
                    // qDebug() << vibrationLevel;
                    vibration_level_selected = vibrationLevel - 1;
                }

                break;
            }
        }
    }

    if (connect && vibration_level_selected != -1) {
        // good finger
        vibraCommand = QStringLiteral("for i in {0..%1}; do fbcli -E button-released; done").arg(vibration_level_selected);
    } else if (!connect && vibration_level_selected != -1) {
        // bad finger
        vibraCommand = QStringLiteral("for i in {0..%1}; do fbcli -E window-close; done").arg(vibration_level_selected);
    }

    // qDebug() << vibraCommand;
    return vibraCommand;
}

void fpdunlocker(const QString& sessionId, int &exitStatus) {
    FPDInterface fpdInterface;
    QEventLoop loop;
    exitStatus = 0;

    QObject::connect(&fpdInterface, &FPDInterface::identified, [&](const QString &finger) {
        qDebug() << "Identified finger: " << finger;

        if (wlrdisplay_status() == 0) {
            QSharedPointer<QProcess> process(new QProcess());

            QString vibraGood = getVibrationLevel(true);
            QString unlock = "loginctl unlock-session " + sessionId;
            QString command = unlock;

            if(!vibraGood.isEmpty()) {
                command = vibraGood + " && " + command;
            }

            process->start("bash", QStringList() << "-c" << command);

            QObject::connect(process.data(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [process, &exitStatus](int exitCode, QProcess::ExitStatus) {
                    exitStatus = exitCode;
                });

            // Wait for the process to finish
            process->waitForFinished();
        } else {
            exitStatus = 0;
        }

        loop.quit();
    });

    QObject::connect(&fpdInterface, &FPDInterface::errorInfo, [&](const QString &info) {
        qDebug() << "Error info:" << info;

        if (info.contains("FINGER_NOT_RECOGNIZED") && wlrdisplay_status() == 0) {
            QSharedPointer<QProcess> process(new QProcess());
            QString vibraBad = getVibrationLevel(false);
            if(!vibraBad.isEmpty()) {
                process->start("bash", QStringList() << "-c" << vibraBad);

                // Wait for the process to finish
                process->waitForFinished();
            }

            exitStatus = 1;
        } else if (info.contains("ERROR_CANCELED") && wlrdisplay_status() != 0) {
            exitStatus = 1;
        } else {
            exitStatus = 0;
        }

        loop.quit();
    });

    qDebug() << "Waiting for finger identification...";

    fpdInterface.identify();

    loop.exec();
}

extern "C" void fpdrunner(const char *sessionId) {
    int oldStat = -1;

    while (1) {
        int dispStat = wlrdisplay_status() == 0 ? 1 : 0;
        int exitStatus = 0;

        if (oldStat != dispStat) {
            oldStat = dispStat;

            if (dispStat == 1) {
                int unlocked = 0;
                while (unlocked == 0) {
                    fpdunlocker(QString(sessionId), exitStatus);
                    if (exitStatus == 0) {
                        unlocked = 1;
                    } else {
                        delay(0.5);
                    }
                }
            }
        } else {
            delay(0.2);
        }
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    system("/usr/bin/binder-wait android.hardware.biometrics.fingerprint@2.1::IBiometricsFingerprint/default");

    char sessionId[64] = {0};
    FILE *fp = NULL;

    while (1) {
        fp = popen("loginctl list-sessions | awk '/tty7/{print $1}'", "r");
        if (fp == NULL) {
            qWarning() << "Failed to run command using popen.";
            delay(1);
            continue;
        }

        if (fgets(sessionId, sizeof(sessionId), fp) != NULL) {
            pclose(fp);
            break;
        } else {
            qWarning() << "Failed to read output";
            pclose(fp);
            delay(1);
        }
    }

    QThread *mainLoopThread = QThread::create([=](){ fpdrunner(sessionId); });
    QObject::connect(mainLoopThread, &QThread::finished, mainLoopThread, &QThread::deleteLater);

    mainLoopThread->start();

    return app.exec();
}
