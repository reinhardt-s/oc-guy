#include "forwarding.h"

Forwarding::Forwarding(QProcess *process, QString ident, QListWidgetItem *item)
{
    this->ident = ident;
    this->process = process;
    this->item = item;
}

QString Forwarding::getIdent() const
{
    return ident;
}

QProcess *Forwarding::getProcess() const
{
    return process;
}

QListWidgetItem *Forwarding::getItem() const
{
    return item;
}
