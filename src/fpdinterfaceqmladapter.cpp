// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

#include "fpdinterfaceqmladapter.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>

FPDInterfaceQmlAdapter::FPDInterfaceQmlAdapter(QObject *parent)
    : QObject(parent)
{
    QObject::connect(&m_fpdInterface, &FPDInterface::acquisitionInfo, this, [this](const QString &info) {
        if(info != "FPACQUIRED_UNRECOGNIZED") { // Ignore unwanted status message
            m_acquisitionInfo = info;
            emit acquisitionInfoChanged(m_acquisitionInfo);
        }
    });
}

QStringList FPDInterfaceQmlAdapter::fingerprints() const
{
    return m_fpdInterface.fingerprints();
}

void FPDInterfaceQmlAdapter::enroll(const QString &finger)
{
    QString formattedFinger = QString(finger).replace(" ", "_").toLower();

    if (formattedFinger.isEmpty()) {
        emit messageChanged("Cannot input an empty name");
        return;
    }

    if (m_fpdInterface.fingerprints().contains(formattedFinger)) {
        emit messageChanged(QString("Finger already exists: %1").arg(finger));
    } else {
        m_fpdInterface.enroll(formattedFinger);

        QObject::connect(&m_fpdInterface, &FPDInterface::enrollProgressChanged, this, [this](int progress) {
            emit enrollmentProgressChanged(progress);
        });

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, formattedFinger]() {
            if (m_fpdInterface.enrollProgress() == 100) {
                timer->stop();
                timer->deleteLater();
                emit fingerprintsChanged();

                QProcess::execute("systemctl", QStringList() << "restart" << "--user" << "droidian-fpd-unlocker");
            }
        });
        timer->start(100);
    }
}

void FPDInterfaceQmlAdapter::remove(const QString &finger)
{
    QString formattedFinger = QString(finger).replace(" ", "_").toLower();

    if (formattedFinger.isEmpty()) {
        emit messageChanged("Cannot input an empty name");
        return;
    }

    if (m_fpdInterface.fingerprints().contains(formattedFinger)) {
        m_fpdInterface.remove(formattedFinger);
        emit fingerprintsChanged();

        QProcess::execute("systemctl", QStringList() << "restart" << "--user" << "droidian-fpd-unlocker");
    }
}

int FPDInterfaceQmlAdapter::enrollmentProgress() const
{
    return m_fpdInterface.enrollProgress();
}

QString FPDInterfaceQmlAdapter::acquisitionInfo() const
{
    return m_acquisitionInfo;
}
