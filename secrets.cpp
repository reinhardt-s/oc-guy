#include "secrets.h"
#include "ui_secrets.h"

Secrets::Secrets(QString project, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Secrets) {
    this->project = project;
    this->process = new QProcess();
    ui->setupUi(this);

    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
    connect(this->process, SIGNAL(readyReadStandardError()), this, SLOT(processOutput()));

    loadSecrets();
}

Secrets::~Secrets() {
    delete ui;
}

void Secrets::loadSecrets() {
    QStringList arguments;
    arguments << "get" << "-o" << "json" << "secret";
    this->setWindowTitle("Secrets: " + project);
    process->start("oc", arguments);
}

void Secrets::processOutput() {
    QString out = process->readAllStandardOutput();
    QString err = process->readAllStandardError();

    QJsonDocument secretsDoc = QJsonDocument::fromJson(QByteArray::fromStdString(out.toStdString()));

    QJsonObject secrets = secretsDoc.object();
    QJsonArray secretList = secrets.value("items").toArray();

    for (int i = 0; i < secretList.size(); i++) {
        QMap <QString, QString> data;
        QJsonObject dataObject = secretList[i].toObject().value("data").toObject();
        QStringList keys = dataObject.keys();
        for (int k = 0; k < keys.size(); k++) {
            data.insert(keys[k], QByteArray::fromBase64(QByteArray::fromStdString(
                    secretList[i].toObject().value("data").toObject().value(keys[k]).toString().toStdString())));
        }
        this->secretList.append(Secret(secretList[i].toObject().value("metadata").toObject().value("name").toString(),
                                       QJsonDocument(secretList[i].toObject()).toJson(),
                                       data
        ));
        ui->secretList->addItem(secretList[i].toObject().value("metadata").toObject().value("name").toString());
    }

}

QString Secrets::getProject() const {
    return project;
}

void Secrets::setProject(const QString &value) {
    project = value;
}

void Secrets::on_secretList_currentRowChanged(int currentRow) {
    ui->data->setRowCount(0);
    ui->secretText->setPlainText(secretList.at(currentRow).getValue());

    QList <QString> keys = secretList.at(currentRow).getData().keys();

    for (QString key : keys) {
        ui->data->insertRow(ui->data->rowCount());
        ui->data->setItem(ui->data->rowCount() - 1, 0, new QTableWidgetItem(key));
        ui->data->setItem(ui->data->rowCount() - 1, 1,
                          new QTableWidgetItem(secretList.at(currentRow).getData().value(key)));
    }

    ui->data->resizeColumnsToContents();
    ui->data->resizeRowsToContents();
}
