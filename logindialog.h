#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0, QProcess *ocProcess = 0, QComboBox *projects = 0);
    ~LoginDialog();

private slots:
    void on_cancel_clicked();

    void on_login_clicked();

private:
    Ui::LoginDialog *ui;
    QProcess *ocProcess;
    QComboBox *projects;
    void login();
};

#endif // LOGINDIALOG_H
