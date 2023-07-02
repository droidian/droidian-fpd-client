// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "fpdinterfaceqmladapter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<FPDInterfaceQmlAdapter>("org.droidian.fingerprint", 1, 0, "FPDInterface");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
