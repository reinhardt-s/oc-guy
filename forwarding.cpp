#include "forwarding.h"

Forwarding::Forwarding(QProcess *process, QString ident, QListWidgetItem *item) {
    this->ident = ident;
    this->process = process;
    this->item = item;
    this->logWindow = new LogMainWindow(item->text(), "forwardLog", nullptr, process);

}

QString Forwarding::getIdent() const {
    return ident;
}

QProcess *Forwarding::getProcess() const {
    return process;
}

QListWidgetItem *Forwarding::getItem() const {
    return item;
}

LogMainWindow *Forwarding::getLogMainWindow() const {
    return logWindow;
}
