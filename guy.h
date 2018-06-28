#ifndef GUY_H
#define GUY_H

#include <QDialog>
#include "logindialog.h"
#include "forwarding.h"
#include <QProcess>
#include <QDebug>

namespace Ui {
class Guy;
}

class Guy : public QDialog
{
    Q_OBJECT

public:
    explicit Guy(QWidget *parent = 0);
    ~Guy();

private slots:
    void on_connectButton_clicked();

    void on_projects_currentIndexChanged(const QString &arg1);

    void on_stop_clicked();

private:
    Ui::Guy *ui;
    QProcess *ocProcess;
    LoginDialog *login;
    QList<Forwarding> forwardings;

    void changeProject();
    void loadPods();
    void loadPorts();
    void forward();
    void stopForwarding();
    QString sendCommand(QString program, QStringList arguments);

};

#endif // GUY_H
