// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include "fpdinterface.h"

void printHelp() {
    qDebug() << "Available commands:";
    qDebug() << "enroll <finger>: Start the enrollment process for the specified finger";
    qDebug() << "identify: Start the identification process";
    qDebug() << "remove <finger>: Remove the specified finger";
    qDebug() << "clear: Clear all fingerprints";
    qDebug() << "list: List all enrolled fingers";
    qDebug() << "help: Display this help message";
}

void handleInput(FPDInterface &fpdInterface, const QString &input)
{
    if (input.startsWith("enroll ")) {
        QString finger = input.section(' ', 1).trimmed();
        qDebug() << "Enrolling finger:" << finger;
        fpdInterface.enroll(finger);
    } else if (input == "identify") {
        qDebug() << "Identifying...";
        fpdInterface.identify();
    } else if (input.startsWith("remove ")) {
        QString finger = input.section(' ', 1).trimmed();
        qDebug() << "Removing finger:" << finger;
        fpdInterface.remove(finger);
    } else if (input == "clear") {
        qDebug() << "Clearing...";
        fpdInterface.clear();
    } else if (input == "help") {
        printHelp();
    } else if (input == "list") {
        QStringList fingers = fpdInterface.fingerprints();
        if (fingers.isEmpty())
            qDebug() << "No enrolled fingers.";
        else
            qDebug() << "Enrolled fingers: " << fingers.join(", ");
    } else {
        qDebug() << "Unknown command:" << input;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    FPDInterface fpdInterface;

    if (argc == 1) {
        printHelp();

        QStringList fingers = fpdInterface.fingerprints();
        if (fingers.isEmpty())
            qDebug() << "No enrolled fingers.";
        else
            qDebug() << "Enrolled fingers: " << fingers.join(", ");

        QObject::connect(&fpdInterface, &FPDInterface::connectionStateChanged, []() {
            qDebug() << "Connection state changed";
        });
        QObject::connect(&fpdInterface, &FPDInterface::fingerprintsChanged, []() {
            qDebug() << "Fingerprints changed";
        });
        QObject::connect(&fpdInterface, &FPDInterface::stateChanged, [](const QString &state) {
            qDebug() << "State changed:" << state;
        });
        QObject::connect(&fpdInterface, &FPDInterface::enrollProgressChanged, [](int progress) {
            qDebug() << "Enroll progress changed:" << progress;
        });
        QObject::connect(&fpdInterface, &FPDInterface::acquisitionInfo, [](const QString &info) {
            if(info != "FPACQUIRED_UNRECOGNIZED") // Ignore unwanted status message
                qDebug() << "Acquisition info:" << info;
        });
        QObject::connect(&fpdInterface, &FPDInterface::errorInfo, [](const QString &info) {
            qDebug() << "Error info:" << info;
        });
        QObject::connect(&fpdInterface, &FPDInterface::added, [](const QString &finger) {
            qDebug() << "Added finger:" << finger;
        });
        QObject::connect(&fpdInterface, &FPDInterface::removed, [](const QString &finger) {
            qDebug() << "Removed finger:" << finger;
        });
        QObject::connect(&fpdInterface, &FPDInterface::identified, [](const QString &finger) {
            qDebug() << "Identified finger:" << finger;
        });
        QObject::connect(&fpdInterface, &FPDInterface::aborted, []() {
            qDebug() << "Operation aborted";
        });
        QObject::connect(&fpdInterface, &FPDInterface::failed, []() {
            qDebug() << "Operation failed";
        });
        QObject::connect(&fpdInterface, &FPDInterface::verified, []() {
            qDebug() << "Verification successful";
        });

        QObject::connect(&fpdInterface, &FPDInterface::enrollProgressChanged,
                         [&fpdInterface](int progress) {
                             if (progress == 100) {
                                 qDebug() << "Enrollment complete! You can now execute another command.";
                             }
                         });

        QTextStream qin(stdin);
        while (true) {
            QString line = qin.readLine();
            if (!line.isNull()) {
                handleInput(fpdInterface, line);
                if (line.startsWith("enroll ")) {
                    while (fpdInterface.enrollProgress() != 100) {
                        app.processEvents();
                    }
                }
            } else {
                break;
            }
        }
    }
    else if (argc >= 2) {
        QString command(argv[1]);

        if (command == "list") {
            QStringList fingers = fpdInterface.fingerprints();
            if (fingers.isEmpty()) {
                return 1;
            } else {
                qDebug().noquote() << fingers.join(", ");
                return 0;
            }
        }
        else if (command == "enroll" && argc == 3) {
            QString finger(argv[2]);
            if (fpdInterface.fingerprints().contains(finger)) {
                qDebug() << "Fingerprint already enrolled:" << finger;
                return 1;
            }

            QObject::connect(&fpdInterface, &FPDInterface::enrollProgressChanged,
                [&app](int progress) {
                    if (progress == 100) {
                        app.quit();
                    } else {
                        qDebug() << progress;
                    }
                }
            );

            fpdInterface.enroll(finger);

            return app.exec();
        }
        else if (command == "remove" && argc == 3) {
            QString finger(argv[2]);
            if (fpdInterface.fingerprints().contains(finger)) {
                fpdInterface.remove(finger);
                return 0;
            } else {
                return 1;
            }
        }
        else {
            qDebug() << "Unknown command or wrong number of arguments";
            return 1;
        }
    }

    return app.exec();
}
