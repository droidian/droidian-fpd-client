#include "journallistener.h"
#include <signal.h>
#include <sys/types.h>

JournalctlListener::JournalctlListener(QObject *parent) : QObject(parent)
{
    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setProgram("bash");
}

JournalctlListener::~JournalctlListener()
{
    killProcessGroup();
    delete process;
}

void JournalctlListener::runCommand()
{
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
        if(exitCode == 0) {
            emit commandFinished();
        }
    });

    QString command = "journalctl -q -f --since \"$(date '+%Y-%m-%d %H:%M:%S')\" | (while read -r line; do echo \"$line\" | grep -q 'gkr-pam' && break; done)";
    process->setArguments(QStringList() << "-c" << command);
    process->start();
    process->waitForStarted();
}

void JournalctlListener::killProcessGroup()
{
    if(process->state() == QProcess::Running) {
        ::kill(-process->processId(), SIGTERM);
        process->waitForFinished();
    }
}
