// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2023 sailfishos-open

#include "fpdinterface.h"

#include <QDebug>
#include <QDBusReply>

FPDInterface::FPDInterface(QObject *parent) : QObject(parent),
    m_serviceWatcher(new QDBusServiceWatcher(
        QStringLiteral(SERVICE_NAME), QDBusConnection::sessionBus(),
        QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration))
{
    QObject::connect(m_serviceWatcher, &QDBusServiceWatcher::serviceRegistered,   this, &FPDInterface::connectDaemon);
    QObject::connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &FPDInterface::disconnectDaemon);

    connectDaemon();
}

int FPDInterface::enroll(const QString &finger)
{
    qDebug() << Q_FUNC_INFO;
    if (!iface->isValid()) {
        return -1;
    }
    QDBusReply<int> reply = iface->call("Enroll", finger);
    if (reply.isValid()) {
        return reply.value();
    }
    return -1;
}

int FPDInterface::identify()
{
    qDebug() << Q_FUNC_INFO;
    if (!iface->isValid()) {
        return -1;
    }
    QDBusReply<int> reply = iface->call("Identify");
    if (reply.isValid()) {
        return reply.value();
    }
    return -1;
}

void FPDInterface::clear()
{
    qDebug() << Q_FUNC_INFO;
    if (!iface->isValid()) {
        return;
    }
    iface->call("Clear");
}

int FPDInterface::remove(const QString &finger)
{
    qDebug() << Q_FUNC_INFO;
    if (!iface->isValid()) {
        return -1;
    }
    QDBusReply<int> reply = iface->call("Remove", finger);
    if (reply.isValid()) {
        return reply.value();
    }
    return -1;
}

void FPDInterface::connectDaemon()
{
    qDebug() << Q_FUNC_INFO;

    if (iface) {
        iface->deleteLater();
    }

    iface = new QDBusInterface(QStringLiteral(SERVICE_NAME), QStringLiteral("/org/sailfishos/fingerprint1"), QStringLiteral(SERVICE_NAME), QDBusConnection::systemBus());

    if (!iface->isValid()) {
        iface->deleteLater();
        return;
    }
    m_connected = true;
    connectionStateChanged();

    //FPD Signals
    connect(iface, SIGNAL(Added(const QString&)), this, SIGNAL(added(const QString&)), Qt::UniqueConnection);
    connect(iface, SIGNAL(Removed(const QString&)), this, SIGNAL(removed(const QString&)), Qt::UniqueConnection);
    connect(iface, SIGNAL(Identified(const QString&)), this, SIGNAL(identified(const QString&)), Qt::UniqueConnection);
    connect(iface, SIGNAL(Aborted()), this, SIGNAL(aborted()), Qt::UniqueConnection);
    connect(iface, SIGNAL(Failed()), this, SIGNAL(failed()), Qt::UniqueConnection);
    connect(iface, SIGNAL(Verified()), this, SIGNAL(verified()), Qt::UniqueConnection);

    connect(iface, SIGNAL(StateChanged(const QString&)), this, SIGNAL(stateChanged(const QString&)), Qt::UniqueConnection);
    connect(iface, SIGNAL(EnrollProgressChanged(int)), this, SLOT(onEnrollProgress(int)), Qt::UniqueConnection);
    connect(iface, SIGNAL(AcquisitionInfo(const QString&)), this, SIGNAL(acquisitionInfo(const QString&)), Qt::UniqueConnection);
    connect(iface, SIGNAL(ErrorInfo(const QString&)), this, SIGNAL(errorInfo(const QString&)), Qt::UniqueConnection);
    connect(iface, SIGNAL(ListChanged()), this, SIGNAL(fingerprintsChanged()), Qt::UniqueConnection);
}

void FPDInterface::disconnectDaemon()
{
    qDebug() << Q_FUNC_INFO;

    m_connected = false;
    connectionStateChanged();
}

int FPDInterface::enrollProgress() const
{
    return m_enroll_progress;
}

void FPDInterface::onEnrollProgress(int p)
{
    m_enroll_progress = p;
    emit enrollProgressChanged(p);
}

QString FPDInterface::state() const
{
    qDebug() << Q_FUNC_INFO;
    if (!iface->isValid()) {
        return QStringLiteral("Unknown: DBus interface invalid");
    }

    QDBusReply<QString> reply = iface->call("GetState");
    if (reply.isValid()) {
        return reply.value();
    }
    return QStringLiteral("Unknown: DBus reply invalid");
}

QStringList FPDInterface::fingerprints() const
{
    qDebug() << Q_FUNC_INFO;
    if (!iface->isValid()) {
        qWarning() << "DBus interface invalid";
        return QStringList();
    }

    QDBusReply<QStringList> reply = iface->call("GetAll");
    if (reply.isValid()) {
        return reply.value();
    }
    qWarning() << "DBus reply invalid";
    return QStringList();
}

