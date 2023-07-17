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
