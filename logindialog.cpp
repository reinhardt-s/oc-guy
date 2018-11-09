#include "logindialog.h"
#include "ui_logindialog.h"


LoginDialog::LoginDialog(QWidget *parent, QComboBox *projects) :
        QDialog(parent),
        ui(new Ui::LoginDialog) {
    ui->setupUi(this);

    QSettings settings("guy.ini",
                       QSettings::IniFormat);

    ui->target->addItem(settings.value("target").toString());
    ui->username->setText(settings.value("username").toString());

    ui->progressBar->setVisible(false);
    this->ocProcess = new QProcess();
    connect(ocProcess, SIGNAL(finished(int)), this, SLOT(processOutput()));
    this->projects = projects;

}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::on_cancel_clicked() {
    this->done(100);
}

void LoginDialog::on_login_clicked() {
    this->login();
}

void LoginDialog::login() {
    ui->login->setEnabled(false);
    ui->cancel->setEnabled(false);
    ui->password->setEnabled(false);
    ui->username->setEnabled(false);
    ui->target->setEnabled(false);

    if (ui->password->text().length() == 0) {
        QMessageBox::critical(this, "Error while trying to connect to " + ui->target->currentText(),
                              "Please provide your password!");
        ui->login->setEnabled(true);
        ui->cancel->setEnabled(true);
        ui->password->setEnabled(true);
        ui->username->setEnabled(true);
        ui->target->setEnabled(true);
        return;
    }

    QString program = "oc";
    QStringList arguments;
    arguments << "login" << "-u" << ui->username->text() << "-p" << ui->password->text()
              << "--insecure-skip-tls-verify=true" << ui->target->currentText();
    ui->progressBar->setVisible(true);
    ocProcess->start(program, arguments);


}

void LoginDialog::processOutput() {
    QString out = ocProcess->readAllStandardOutput();
    QString err = ocProcess->readAllStandardError();
    if (out.length() > 1 && (out.startsWith("Login successful.") || out.startsWith("Logged into"))) {
        qDebug() << out;
        QSettings settings("guy.ini",
                           QSettings::IniFormat);
        settings.setValue("target", ui->target->currentText());
        settings.setValue("username", ui->username->text());
        this->done(200);
    } else if (out.length() > 1) {
        QMessageBox::critical(this, "Error while trying to connect to " + ui->target->currentText(), out);
    }

    if (err.length() > 2)
        QMessageBox::critical(this, "Error while trying to connect to " + ui->target->currentText(), err);

    ui->login->setEnabled(true);
    ui->cancel->setEnabled(true);
    ui->password->setEnabled(true);
    ui->username->setEnabled(true);
    ui->target->setEnabled(true);
    ui->progressBar->setVisible(false);
}


