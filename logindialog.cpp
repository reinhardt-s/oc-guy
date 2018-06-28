#include "logindialog.h"
#include "ui_logindialog.h"




LoginDialog::LoginDialog(QWidget *parent, QProcess *ocProcess, QComboBox *projects) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    this->ocProcess = ocProcess;
    this->projects = projects;
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_cancel_clicked()
{
    this->close();
}

void LoginDialog::on_login_clicked()
{
    this->login();
}

void LoginDialog::login() {
    ui->login->setEnabled(false);
    ui->cancel->setEnabled(false);

    if ( ui->password->text().length() == 0) {
        QMessageBox::critical(this, "Error while trying to connect to " + ui->target->currentText(), "Please provide your password!");
        ui->login->setEnabled(true);
        ui->cancel->setEnabled(true);
        return;
    }

    QString program = "oc";
    QStringList arguments;
    arguments << "login" <<"-u" << ui->username->text() <<"-p" << ui->password->text() << "--insecure-skip-tls-verify=true" << ui->target->currentText();

    ocProcess->start(program, arguments);

    if (!ocProcess->waitForFinished(4000))
           return;

    ui->login->setEnabled(true);
    ui->cancel->setEnabled(true);

    QByteArray result = ocProcess->readAll();
    QString stringResult = QString(result);
    if ( stringResult.startsWith("Login successful.") || stringResult.startsWith("Logged into") ) {

        stringResult = stringResult.mid(0, stringResult.lastIndexOf("\n\n"));
        QStringList projectList = stringResult.mid((stringResult.indexOf("\'oc project <projectname>\':\n\n")+29)).replace(" ","").split('\n');

        this->projects->clear();
        QString current = "default";
        for (QString item : projectList) {
            if ( item.startsWith("*")){
                item = item.mid(1);
                current = item;
            }
            this->projects->addItem(item);
        }
        this->projects->setCurrentIndex(this->projects->findText(current));

        this->close();
    } else {
        QMessageBox::critical(this, "Error while trying to connect to " + ui->target->currentText(), stringResult);
    }
    qDebug() << stringResult;

}

