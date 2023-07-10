// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include "fpdinterface.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = QCoreApplication::arguments();

    if(args.count() != 2) {
        qCritical() << "Invalid number of arguments. Usage: droidian-fpd-unlocker <session_id>";
        return 1;
    }

    QString sessionId = args[1];

    FPDInterface fpdInterface;

    QObject::connect(&fpdInterface, &FPDInterface::identified, [sessionId](const QString &finger) {
        qDebug() << "Identified finger:" << finger;

        QProcess checkScreen;
        checkScreen.start("batman-helper", QStringList() << "wlrdisplay");
        checkScreen.waitForFinished();

        QString batmanOutput(checkScreen.readAllStandardOutput());

        if (batmanOutput.trimmed() == "yes") {
            QProcess *process = new QProcess();
            QString vibra_good = "fbcli -E bell-terminal";
            QString unlock = "loginctl unlock-session " + sessionId;
            QString command = vibra_good + " && " + unlock;
            process->start("bash", QStringList() << "-c" << command);

            QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [&](int exitCode, QProcess::ExitStatus exitStatus) {
                    QCoreApplication::exit(exitCode);
                });
        }
        else {
            QCoreApplication::exit(0);
        }
    });

    QObject::connect(&fpdInterface, &FPDInterface::errorInfo, [](const QString &info) {
        qDebug() << "Error info:" << info;

        QProcess checkScreen;
        checkScreen.start("batman-helper", QStringList() << "wlrdisplay");
        checkScreen.waitForFinished();

        QString batmanOutput(checkScreen.readAllStandardOutput());

        if (info.contains("FINGER_NOT_RECOGNIZED") && batmanOutput.trimmed() == "yes") {
            QProcess *process = new QProcess();
            QString vibra_bad = "for i in {0..2}; do fbcli -E button-pressed; done";
            process->start("bash", QStringList() << "-c" << vibra_bad);

           QCoreApplication::exit(1);
        } else {
           QCoreApplication::exit(0);
        }
    });

    qDebug() << "Waiting for finger identification...";
    fpdInterface.identify();

    return app.exec();
}
