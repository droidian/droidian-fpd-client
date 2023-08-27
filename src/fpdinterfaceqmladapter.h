// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Droidian Project
//
// Authors:
// Bardia Moshiri <fakeshell@bardia.tech>

#ifndef FPDINTERFACEQMLADAPTER_H
#define FPDINTERFACEQMLADAPTER_H

#include <QObject>
#include <QStringList>
#include "fpdinterface.h"

class FPDInterfaceQmlAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList fingerprints READ fingerprints NOTIFY fingerprintsChanged)
    Q_PROPERTY(int enrollmentProgress READ enrollmentProgress NOTIFY enrollmentProgressChanged)
    Q_PROPERTY(QString acquisitionInfo READ acquisitionInfo NOTIFY acquisitionInfoChanged)

public:
    explicit FPDInterfaceQmlAdapter(QObject *parent = nullptr);

    QStringList fingerprints() const;
    Q_INVOKABLE void enroll(const QString &finger);
    Q_INVOKABLE void remove(const QString &finger);

    int enrollmentProgress() const;
    QString acquisitionInfo() const;

signals:
    void fingerprintsChanged();
    void messageChanged(const QString &msg);
    void enrollmentProgressChanged(int progress);
    void acquisitionInfoChanged(const QString &info);

private:
    FPDInterface m_fpdInterface;
    QString m_acquisitionInfo;
};

#endif // FPDINTERFACEQMLADAPTER_H
