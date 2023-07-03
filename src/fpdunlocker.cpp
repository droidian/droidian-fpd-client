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

        QProcess *process = new QProcess();
        QString command = "loginctl unlock-session " + sessionId;
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
