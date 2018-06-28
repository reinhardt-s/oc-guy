#include "guy.h"
#include "ui_guy.h"






Guy::Guy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Guy)
{
    ui->setupUi(this);
     ocProcess = new QProcess(this);
     login = new LoginDialog(this, ocProcess, ui->projects);
     login->show();
}

Guy::~Guy()
{
    delete ui;
}

void Guy::on_connectButton_clicked()
{
    forward();
}

void Guy::on_projects_currentIndexChanged(const QString &arg1)
{
    changeProject();
    loadPorts();
    loadPods();
}

void Guy::changeProject()
{
    QString program = "oc";
    QStringList arguments;
    arguments << "project" << ui->projects->currentText();

    QString result = sendCommand(program, arguments);
    ui->responseArea->setPlainText(result);
}

void Guy::loadPods()
{
    QString program = "oc";
    QStringList arguments;
    arguments << "get" << "po" << "-o" << "jsonpath='{.items[*].metadata.name}'";

    QString result = sendCommand(program, arguments);
    ui->responseArea->setPlainText(result);
    result = result.mid(1, result.length()-2);

    ui->pods->clear();
    QStringList podList = result.split(" ");
    ui->pods->addItems(podList);
}

void Guy::loadPorts()
{
    QString program = "oc";
    QStringList arguments;
    arguments << "get" << "svc"<< "-o" << "jsonpath='{.items[*].spec.ports[*].targetPort}'";

    QString result = sendCommand(program, arguments);
    ui->responseArea->setPlainText(result);

    ui->ports->clear();
    QStringList portList = result.mid(1,result.length()-2).split(" ");
    portList.removeDuplicates();

    ui->ports->addItems(portList);
}

void Guy::forward()
{
    QProcess *genericOcProcess = new QProcess();
    QListWidgetItem *item = new QListWidgetItem(ui->pods->currentItem()->text()+" "+QString::number(ui->targetPort->value())+":"+ui->ports->currentText());
    forwardings.append(Forwarding(genericOcProcess, item->text(), item));

    ui->forwards->addItem(item);

    QString program = "oc";
    QStringList arguments;
    arguments << "port-forward" << ui->pods->currentItem()->text() << QString::number(ui->targetPort->value())+":"+ui->ports->currentText();
    qDebug()  << "oc port-forward "+ui->pods->currentItem()->text()+" "+QString::number(ui->targetPort->value())+":"+ui->ports->currentText();

    genericOcProcess->start(program, arguments);

    if (!genericOcProcess->waitForFinished(4000))
           return;


    QString result = sendCommand(program, arguments);
    ui->responseArea->setPlainText(result);


}

void Guy::stopForwarding()
{
    if(!ui->forwards->currentItem())
        return;
    for ( int i = 0; i < forwardings.size(); ++i) {
        if ( forwardings.at(i).getIdent() == ui->forwards->currentItem()->text()){
            forwardings.at(i).getItem()->~QListWidgetItem();
            forwardings.at(i).getProcess()->close();
            forwardings.removeAt(i);
            break;
        }
    }
}

QString Guy::sendCommand(QString program, QStringList arguments)
{
    ocProcess->start(program, arguments);

    if (!ocProcess->waitForFinished(4000))
           return "";

    QByteArray result = ocProcess->readAll();
    QString out = QString(result);
    qDebug() << out;
    return out;
}

void Guy::on_stop_clicked()
{
    stopForwarding();
}
