// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Droidian Project
//
// Authors:
// Bardia Moshiri <fakeshell@bardia.tech>

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include "fpdinterface.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    FPDInterface fpdInterface;

    QObject::connect(&fpdInterface, &FPDInterface::identified, [](const QString &finger) {
        qDebug() << "Identified finger:" << finger;

        QProcess *process = new QProcess();
        QString command = "fbcli -E bell-terminal";
        process->start("bash", QStringList() << "-c" << command);

        QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [&](int exitCode, QProcess::ExitStatus exitStatus) {
                process->deleteLater(); // Schedule the process for deletion
                QCoreApplication::exit(exitCode);
            });
    });

    QObject::connect(&fpdInterface, &FPDInterface::errorInfo, [](const QString &info) {
        qDebug() << "Error info:" << info;

        if (info.contains("FINGER_NOT_RECOGNIZED")) {
            QProcess *process = new QProcess();
            QString vibra_bad = "for i in {0..2}; do fbcli -E button-pressed; done";
            process->start("bash", QStringList() << "-c" << vibra_bad);

            QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [process](int, QProcess::ExitStatus) {
                    process->deleteLater(); // Schedule the process for deletion
                });
        }

        QCoreApplication::exit(1);
    });

    qDebug() << "Waiting for finger identification...";
    fpdInterface.identify();

    return app.exec();
}
