#ifndef GUY_H
#define GUY_H

#include <QDialog>
#include "logindialog.h"
#include "loadingthread.h"
#include "forwarding.h"
#include "logmainwindow.h"
#include "secrets.h"
#include <QProcess>
#include <QDebug>
#include <QSettings>

namespace Ui {
    class Guy;
}

class Guy : public QDialog {
    Q_OBJECT

public:
    explicit Guy(QWidget *parent = 0);

    ~Guy();

protected:
    QStringList splitToList(QString command);

protected
    slots:
            void

    receivePorts();

    void waitForTerminated();

    void showPIDInfo();

    void receivePIDInfo();

private
    slots:
            void
    showForwardLog(QListWidgetItem
    *item);

    void on_connectButton_clicked();

    void onLoginDone(int status);

    void on_projects_currentIndexChanged(const QString &arg1);

    void on_stop_clicked();

    void onProjectChanged(QString response);

    void onPortsLoaded(QStringList portList);

    void onPodsLoaded(QStringList podList);

    void onStatusChanged(QString status);

    void onGuyStateChanged(bool state);

    void on_taillogButton_clicked();

    void on_sshButton_clicked();

    void on_projectsRefresh_clicked();

    void on_podRefresh_clicked();

    void on_portsRefresh_clicked();

    void on_ports_currentTextChanged(const QString &arg1);

    void on_infoButton_clicked();

    void on_pods_currentTextChanged(const QString &currentText);

    void on_terminateButton_clicked();

    void on_pushButton_clicked();

    void on_projects_currentTextChanged(const QString &arg1);

    void on_pushButton_3_clicked();

    void terminateProcess();

    void on_secrets_clicked();

    void on_save_clicked();

private:
    Ui::Guy *ui;
    QProcess *ocProcess;
    QProcess *portProcess;
    QProcess *terminationProcess;
    QProcess *pidInfoProcess;
    LoginDialog *login;
    LoadingThread *loadingThread;

    QList <Forwarding> forwardings;

    void changeProject(QString currentProject, QString pod = "");

    void forward();

    void stopForwarding();

    QString sendCommand(QString program, QStringList arguments);

    void loadUser();

    void loadProjects();

    QStringList getProjects();

    QString loadCurrentProject();

    void showLogin();

    void showActivePorts();
};

#endif // GUY_H
