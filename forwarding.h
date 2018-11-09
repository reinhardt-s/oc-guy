#ifndef FORWARDING_H
#define FORWARDING_H

#include <QProcess>
#include <QListWidgetItem>
#include "LogMainWindow.h"

class Forwarding {
public:
    Forwarding(QProcess *process, QString ident, QListWidgetItem *item);

    QString getIdent() const;

    QProcess *getProcess() const;

    QListWidgetItem *getItem() const;

    LogMainWindow *getLogMainWindow() const;

private:
    QProcess *process;
    QListWidgetItem *item;
    QString ident;
    LogMainWindow *logWindow;

};

#endif // FORWARDING_H
