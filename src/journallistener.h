// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Droidian Project
//
// Authors:
// Bardia Moshiri <fakeshell@bardia.tech>

#ifndef JOURNALCTLLISTENER_H
#define JOURNALCTLLISTENER_H

#include <QObject>
#include <QProcess>

class JournalctlListener : public QObject
{
    Q_OBJECT
public:
    explicit JournalctlListener(QObject *parent = nullptr);
    ~JournalctlListener();

    void runCommand();

signals:
    void commandFinished();

private:
    QProcess *process;
    void killProcessGroup();
};

#endif // JOURNALCTLLISTENER_H
