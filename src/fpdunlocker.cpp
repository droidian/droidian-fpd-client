// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QProcess> // Include QProcess
#include "fpdinterface.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    FPDInterface fpdInterface;

    QObject::connect(&fpdInterface, &FPDInterface::identified, [](const QString &finger) {
        qDebug() << "Identified finger:" << finger;

        QProcess *process = new QProcess();
        QString command = "loginctl unlock-session $(loginctl list-sessions | awk '/tty7/{print $1}')";
        process->start("bash", QStringList() << "-c" << command);

        QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [&](int exitCode, QProcess::ExitStatus exitStatus) {
                QCoreApplication::exit(exitCode);
            });
    });

    QObject::connect(&fpdInterface, &FPDInterface::errorInfo, [](const QString &info) {
        qDebug() << "Error info:" << info;
        QCoreApplication::exit(1);
    });

    qDebug() << "Waiting for finger identification...";
    fpdInterface.identify();

    return app.exec();
}
