#ifndef FORWARDING_H
#define FORWARDING_H

#include <QProcess>
#include <QListWidgetItem>

class Forwarding
{
public:
    Forwarding(QProcess *process, QString ident, QListWidgetItem *item);



    QString getIdent() const;

    QProcess *getProcess() const;

    QListWidgetItem *getItem() const;

private:
    QProcess *process;
    QListWidgetItem *item;
    QString ident;
};

#endif // FORWARDING_H
