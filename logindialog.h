#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>
#include <QSettings>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0, QComboBox *projects = 0);

    ~LoginDialog();

public
    slots:
            void

    processOutput();

private
    slots:
            void

    on_cancel_clicked();

    void on_login_clicked();

private:
    Ui::LoginDialog *ui;
    QProcess *ocProcess;
    QComboBox *projects;

    void login();
};

#endif // LOGINDIALOG_H
