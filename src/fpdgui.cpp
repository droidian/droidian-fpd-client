// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Droidian Project
//
// Authors:
// Bardia Moshiri <fakeshell@bardia.tech>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "fpdinterfaceqmladapter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<FPDInterfaceQmlAdapter>("org.droidian.fingerprint", 1, 0, "FPDInterface");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("/usr/lib/droidian-fingerprint/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
