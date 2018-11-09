#include "guy.h"
#include "ui_guy.h"

#include <QDir>


Guy::Guy(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Guy) {
    ui->setupUi(this);
    ocProcess = new QProcess(this);

    loadingThread = new LoadingThread();

    connect(loadingThread, SIGNAL(valueChanged(int)),
            ui->progressBar, SLOT(setValue(int)));

    connect(loadingThread, SIGNAL(guyEnabled(bool)),
            this, SLOT(onGuyStateChanged(bool)));

    connect(loadingThread, SIGNAL(statusChanged(QString)),
            this, SLOT(onStatusChanged(QString)));

    connect(loadingThread, SIGNAL(projectChanged(QString)),
            this, SLOT(onProjectChanged(QString)));

    connect(loadingThread, SIGNAL(podsLoaded(QStringList)),
            this, SLOT(onPodsLoaded(QStringList)));

    connect(loadingThread, SIGNAL(portsLoaded(QStringList)),
            this, SLOT(onPortsLoaded(QStringList)));

    connect(ui->forwards, SIGNAL(itemDoubleClicked(QListWidgetItem * )),
            this, SLOT(showForwardLog(QListWidgetItem * )));


    QSettings settings("guy.ini",
                       QSettings::IniFormat);

    ui->logLength->setValue(settings.value("logLength").toInt());
    ui->terminateConnection->setText(settings.value("terminateConnection").toString());
    ui->getActivePorts->setText(settings.value("getActivePorts").toString());
    ui->pidInfo->setText(settings.value("getPIDinfo").toString());

    loadUser();
}

void Guy::showForwardLog(QListWidgetItem *item) {
    if (!ui->forwards->currentItem())
        return;
    for (int i = 0; i < forwardings.size(); ++i) {
        if (forwardings.at(i).getIdent() == ui->forwards->currentItem()->text()) {
            forwardings.at(i).getLogMainWindow()->show();
            break;
        }
    }
}

void Guy::onGuyStateChanged(bool state) {
    ui->groupBox->setEnabled(state);
    ui->groupBox_3->setEnabled(state);
    ui->groupBox_2->setEnabled(state);
}

void Guy::onStatusChanged(QString status) {
    ui->status->setText(status);
}

void Guy::onProjectChanged(QString response) {
}

void Guy::onPortsLoaded(QStringList portList) {
    ui->ports->clear();
    portList.removeDuplicates();
    ui->ports->addItems(portList);
}

void Guy::onPodsLoaded(QStringList podList) {
    ui->pods->clear();
    ui->pods->addItems(podList);
}

void Guy::loadUser() {
    QString program = "oc";
    QStringList arguments;
    arguments << "whoami";

    QString result = sendCommand(program, arguments);
    result = result.replace("\r", "").replace("\n", "");
    qDebug() << result;
    if (!result.isEmpty()) {

        QMessageBox msgBox;
        msgBox.setText("Already logged in");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setInformativeText("Continue as " + result + "?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                qDebug() << "Just go on loading projects";
                loadProjects();
                break;
            case QMessageBox::No:
                qDebug() << "Let's choose another user";
                showLogin();
                break;
            default:
                qDebug() << "Something is odd.";
                break;
        }
    } else {
        qDebug() << "Not logged in yet.";
        showLogin();
    }
}

void Guy::showLogin() {
    login = new LoginDialog(this, ui->projects);
    connect(login, SIGNAL(finished(int)),
            this, SLOT(onLoginDone(int)));
    login->show();
}

void Guy::onLoginDone(int status) {
    if (status == 200) {
        loadProjects();
    } else {
        this->close();
    }
}

void Guy::loadProjects() {

    ui->projects->disconnect();

    QString current = loadCurrentProject();
    QStringList projectList = getProjects();
    ui->projects->clear();
    for (QString item : projectList) {
        ui->projects->addItem(item);
    }

    ui->projects->setCurrentIndex(ui->projects->findText(current));

    QObject::connect(ui->projects, &QComboBox::currentTextChanged,
                     this, &Guy::on_projects_currentIndexChanged);

    changeProject(current);


}

QString Guy::loadCurrentProject() {

    QString program = "oc";
    QStringList arguments;
    arguments << "project" << "-q";
    QString result = sendCommand(program, arguments);
    result = result.replace("\r", "").replace("\n", "");
    qDebug() << "Current project: " << result;
    return result;
}

QStringList Guy::getProjects() {
    QString program = "oc";
    QStringList arguments;
    arguments << "get" << "projects" << "-o" << "jsonpath='{.items[*].metadata.name}'";

    QString result = sendCommand(program, arguments);
    result = result.mid(1, result.length() - 2);
    QStringList podList = result.split(" ");
    return podList;
}

Guy::~Guy() {
    delete ui;
}

void Guy::on_connectButton_clicked() {
    forward();
}

void Guy::on_projects_currentIndexChanged(const QString &currentProject) {
    changeProject(currentProject);
}

void Guy::changeProject(QString currentProject, QString pod) {
    qDebug() << "Changing project to: " + currentProject;
    loadingThread->setProject(currentProject);
    loadingThread->setPod(pod);
    loadingThread->start();
}


void Guy::forward() {
    if (ui->pods->currentRow() == -1)
        return;
    QProcess *genericOcProcess = new QProcess();
    QListWidgetItem * item = new QListWidgetItem(
            ui->pods->currentItem()->text() + ":" + ui->ports->currentText() + " >> localhost:" +
            QString::number(ui->targetPort->value()));
    forwardings.append(Forwarding(genericOcProcess, item->text(), item));

    ui->forwards->addItem(item);

    QString program = "oc";
    QStringList arguments;
    arguments << "port-forward" << ui->pods->currentItem()->text()
              << QString::number(ui->targetPort->value()) + ":" + ui->ports->currentText();
    qDebug() << "oc port-forward " + ui->pods->currentItem()->text() + " " + QString::number(ui->targetPort->value()) +
                ":" + ui->ports->currentText();

    genericOcProcess->start(program, arguments);


}

void Guy::stopForwarding() {
    if (!ui->forwards->currentItem())
        return;
    for (int i = 0; i < forwardings.size(); ++i) {
        if (forwardings.at(i).getIdent() == ui->forwards->currentItem()->text()) {
            forwardings.at(i).getItem()->~QListWidgetItem();
            forwardings.at(i).getProcess()->close();
            forwardings.removeAt(i);
            break;
        }
    }
}

QString Guy::sendCommand(QString program, QStringList arguments) {
    ocProcess->start(program, arguments);

    if (!ocProcess->waitForFinished(4000)) {
        QMessageBox::critical(this, "Error while sending command", "No response.");
        return "";
    }

    QByteArray result = ocProcess->readAll();
    QString out = QString(result);
    qDebug() << out;
    return out;
}

void Guy::on_stop_clicked() {
    stopForwarding();
}

void Guy::on_taillogButton_clicked() {
    if (ui->pods->currentRow() == -1)
        return;
    LogMainWindow *lw = new LogMainWindow(ui->pods->currentItem()->text(), "log");
    lw->show();
}

void Guy::on_sshButton_clicked() {
    if (ui->pods->currentRow() == -1)
        return;
    system("start cmd /K oc rsh " + ui->pods->currentItem()->text().toLocal8Bit());
}

void Guy::on_projectsRefresh_clicked() {
    loadProjects();
}

void Guy::on_podRefresh_clicked() {
    changeProject(ui->projects->currentText());
}

void Guy::on_portsRefresh_clicked() {
    if (ui->pods->currentRow() != -1) {
        changeProject(ui->projects->currentText(), ui->pods->currentItem()->text());
    }
}

void Guy::on_ports_currentTextChanged(const QString &arg1) {
    ui->targetPort->setValue(arg1.toInt());
}

void Guy::on_infoButton_clicked() {
    if (ui->pods->currentRow() == -1)
        return;
    LogMainWindow *lw = new LogMainWindow(ui->pods->currentItem()->text(), "info");
    lw->show();
}

void Guy::on_pods_currentTextChanged(const QString &currentText) {
    changeProject(ui->projects->currentText(), currentText);
}

void Guy::on_terminateButton_clicked() {
    if (ui->pods->currentRow() == -1)
        return;

    QMessageBox msgBox;
    msgBox.setText("Force delete");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setInformativeText(
            "Are you sure you want to force delete the pod <b>" + ui->pods->currentItem()->text() + "</b>?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
        case QMessageBox::Yes:
            qDebug() << "Just go on loading projects";
            QString program = "oc";
            QStringList arguments;
            arguments << "delete" << "pod" << ui->pods->currentItem()->text() << "--force";
            QString result = sendCommand(program, arguments);
            QMessageBox report;
            report.setText(result);
            report.exec();
            changeProject(ui->projects->currentText());
            break;
    }
}

void Guy::on_pushButton_clicked() {
    LogMainWindow *lw = new LogMainWindow("none", "purge");
    lw->showMaximized();
}

void Guy::on_projects_currentTextChanged(const QString &arg1) {

}

void Guy::on_pushButton_3_clicked() {
    showActivePorts();
}

QStringList Guy::splitToList(QString command) {
    QStringList tmpList = command.split(QRegExp("\""));
    bool inside = false;
    QStringList list;
    foreach(QString
    s, tmpList) {
        if (inside) {
            list.append(s);
        } else {
            list.append(s.split(QRegExp("\\s+"), QString::SkipEmptyParts));
        }
        inside = !inside;
    }
    return list;
}

void Guy::showPIDInfo() {
    QPushButton *infoButton = qobject_cast<QPushButton *>(sender());
    pidInfoProcess = new QProcess();
    QString command = ui->pidInfo->text().replace("%PID", infoButton->property("pid").toString());
    QStringList commandList = splitToList(command);
    QString program = "tasklist";
    if (!commandList.isEmpty() && commandList.first() != "") {
        program = commandList.first();
        commandList.removeFirst();
    }

    QStringList arguments;
    arguments << " /FI" << "\"pid eq " + infoButton->property("pid").toString() + "\"" << "/FO" << "CSV" << "/V";
    if (!commandList.isEmpty() && commandList.first() != "") {
        arguments = commandList;
    }
    pidInfoProcess->start(program, arguments);

    connect(pidInfoProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receivePIDInfo()));
    connect(pidInfoProcess, SIGNAL(readyReadStandardError()), this, SLOT(receivePIDInfo()));

}

void Guy::receivePIDInfo() {
    disconnect(pidInfoProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receivePIDInfo()));
    disconnect(pidInfoProcess, SIGNAL(readyReadStandardError()), this, SLOT(receivePIDInfo()));

    pidInfoProcess->waitForFinished();
    QString error = pidInfoProcess->readAllStandardError();
    QString msg = pidInfoProcess->readAllStandardOutput();


    pidInfoProcess->close();
    pidInfoProcess->kill();

    if (!error.isEmpty()) {
        QMessageBox::critical(this, "Error while requesting pid infos", error);

        return;
    }
    QStringList lines = msg.split("\n");

    QStringList header = lines.first().split(",");
    lines.removeFirst();
    QStringList process = lines.first().split(",");

    QString out = "Prozess: " + process.at(0).mid(1, process.at(0).length() - 2) + "\n\n";

    for (int i = 0; i < header.size(); i++) {
        out += header.at(i).mid(1, header.at(i).length() - 2) + ": " +
               process.at(i).mid(1, process.at(i).length() - 2) + "\n";
    }

    QMessageBox::information(this, "PID INFO", out);
}

void Guy::terminateProcess() {
    QPushButton *terminatorButton = qobject_cast<QPushButton *>(sender());
    terminationProcess = new QProcess();
    QString command = ui->terminateConnection->text().replace("%PID", terminatorButton->property("pid").toString());
    QStringList commandList = splitToList(command);
    QString program = "taskkill";
    if (!commandList.isEmpty() && commandList.first() != "") {
        program = commandList.first();
        commandList.removeFirst();
    }

    QStringList arguments;
    arguments = splitToList("tasklist /FI \"pid eq 9768\" /FO CSV /V");
    if (!commandList.isEmpty() && commandList.first() != "") {
        arguments = commandList;
    }
    terminationProcess->start(program, arguments);

    connect(terminationProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(waitForTerminated()));
    connect(terminationProcess, SIGNAL(readyReadStandardError()), this, SLOT(waitForTerminated()));

}

void Guy::waitForTerminated() {
    QString error = terminationProcess->readAllStandardError();
    QString msg = terminationProcess->readAllStandardOutput();

    terminationProcess->close();
    terminationProcess->kill();

    if (!error.isEmpty()) {
        QMessageBox::critical(this, "Error while requesting ports", error);

        return;
    }

    QMessageBox::information(this, "Terminated", msg);

    showActivePorts();
}

void Guy::showActivePorts() {

    ui->portTable->setEnabled(false);
    ui->progressBar->setValue(99);
    ui->status->setText("Loading active ports");


    portProcess = new QProcess();
    QString program = "netstat";
    QString command = ui->getActivePorts->text();
    QStringList commandList = splitToList(command);
    if (!commandList.isEmpty() && commandList.first() != "") {
        program = commandList.first();
        commandList.removeFirst();
    }

    QStringList arguments;
    arguments << "-ano";
    if (!commandList.isEmpty() && commandList.first() != "") {
        arguments = commandList;
    }
    portProcess->start(program, arguments);
    connect(portProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(receivePorts()));
    connect(portProcess, SIGNAL(readyReadStandardError()), this, SLOT(receivePorts()));


}

void Guy::receivePorts() {

    QString error = portProcess->readAllStandardError();
    if (!error.isEmpty()) {
        QMessageBox::critical(this, "Error while requesting ports", error);
        return;
    }

    ui->portTable->setSortingEnabled(false);
    while (ui->portTable->rowCount() > 0) {
        ui->portTable->removeRow(0);
    }

    QString netstat = portProcess->readAllStandardOutput();
    QStringList rows = netstat.split("\r\n");
    int ignoreCount = 4;
    for (QString row : rows) {
        if (ignoreCount > 0) {
            ignoreCount--;
            continue;
        }

        QString protocol;
        QString lAddress;
        QString rAddress;
        QString state;
        QString pid;

        while (row.startsWith(" ")) {
            row = row.mid(1);
        }
        protocol = row.mid(0, row.indexOf(" "));
        row = row.mid(protocol.length() + 1);

        while (row.startsWith(" ")) {
            row = row.mid(1);
        }
        lAddress = row.mid(0, row.indexOf(" "));
        row = row.mid(lAddress.length() + 1);
        lAddress = lAddress.mid(lAddress.lastIndexOf(":") + 1);

        while (row.startsWith(" ")) {
            row = row.mid(1);
        }
        rAddress = row.mid(0, row.indexOf(" "));
        row = row.mid(rAddress.length() + 1);

        while (row.startsWith(" ")) {
            row = row.mid(1);
        }
        state = row.mid(0, row.indexOf(" "));
        row = row.mid(state.length() + 1);

        while (row.startsWith(" ")) {
            row = row.mid(1);
        }
        pid = row.mid(0, row.indexOf(" "));
        row = row.mid(pid.length() + 1);

        QPushButton *terminator = new QPushButton("Terminate");
        QPushButton *pidInfo = new QPushButton(pid);
        terminator->setProperty("pid", pid);
        pidInfo->setProperty("pid", pid);

        connect(terminator, SIGNAL(clicked(bool)),
                this, SLOT(terminateProcess()));

        connect(pidInfo, SIGNAL(clicked(bool)),
                this, SLOT(showPIDInfo()));

        ui->portTable->insertRow(ui->portTable->rowCount());
        ui->portTable->setItem(ui->portTable->rowCount() - 1, 0, new QTableWidgetItem(protocol));
        ui->portTable->setItem(ui->portTable->rowCount() - 1, 1, new QTableWidgetItem(lAddress));
        ui->portTable->setItem(ui->portTable->rowCount() - 1, 2, new QTableWidgetItem(rAddress));
        ui->portTable->setCellWidget(ui->portTable->rowCount() - 1, 3, pidInfo);
        ui->portTable->setCellWidget(ui->portTable->rowCount() - 1, 4, terminator);
    }
    ui->portTable->setSortingEnabled(true);

    ui->progressBar->setValue(0);
    ui->status->setText("Ready");
    ui->portTable->resizeColumnsToContents();
    ui->portTable->resizeRowsToContents();
    ui->portTable->setEnabled(true);

}

void Guy::on_secrets_clicked() {
    Secrets *lw = new Secrets(ui->projects->currentText());
    lw->show();
}

void Guy::on_save_clicked() {
    QSettings settings("guy.ini",
                       QSettings::IniFormat);
    settings.setValue("logLength", ui->logLength->value());
    settings.setValue("terminateConnection", ui->terminateConnection->text());
    settings.setValue("getActivePorts", ui->getActivePorts->text());
    settings.setValue("getPIDInfo", ui->pidInfo->text());
}
