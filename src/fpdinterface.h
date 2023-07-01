// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2023 sailfishos-open

#ifndef FPDINTERFACE_H
#define FPDINTERFACE_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QStringList>

#define SERVICE_NAME "org.sailfishos.fingerprint1"

class FPDInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectionStateChanged)
    Q_PROPERTY(QStringList fingerprints READ fingerprints NOTIFY fingerprintsChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(int enrollProgress READ enrollProgress NOTIFY enrollProgressChanged)

public:
    explicit FPDInterface(QObject *parent = nullptr);

    bool connected() const { return m_connected; }
    QString state() const;
    int enrollProgress() const;
    QStringList fingerprints() const;

signals:
    void connectionStateChanged();
    void fingerprintsChanged();

    void stateChanged(const QString &state);
    void enrollProgressChanged(int progress);
    void acquisitionInfo(const QString &info);
    void errorInfo(const QString &info);

    void added(const QString &finger);
    void removed(const QString &finger);
    void identified(const QString &finger);
    void aborted();
    void failed();
    void verified();

public slots:
    int enroll(const QString &finger);
    int identify();
    int remove(const QString &finger);
    void clear();

private slots:
    void connectDaemon();
    void disconnectDaemon();
    void onEnrollProgress(int);

private:
    QDBusInterface *iface = nullptr;
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    bool m_connected = false;
    int m_enroll_progress = -1;
};

#endif // FPDINTERFACE_H
