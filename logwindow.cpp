#include "logwindow.h"
#include "ui_logwindow.h"

LogWindow::LogWindow(QString pod, QString type, QWidget *parent, QProcess *process) :
        QWidget(parent),
        ui(new Ui::LogWindow) {
    ui->setupUi(this);

    !process ? this->process = new QProcess() : this->process = process;
    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
    connect(this->process, SIGNAL(readyReadStandardError()), this, SLOT(processOutput()));


    if (type == "log") {
        processLog(pod);
    } else if (type == "info") {

        processInfo(pod);
    } else if (type == "purge") {
        processPurge(1);
    } else if (type == "forwardLog") {
        this->setWindowTitle("Port forwarding: " + pod);
    }
}

void LogWindow::processLog(QString pod) {
    QStringList arguments;
    arguments << "logs" << pod << "--tail=150" << "-f";
    this->setWindowTitle("LOG: " + pod);
    process->start("oc", arguments);
}

void LogWindow::processInfo(QString pod) {
    QStringList arguments;
    arguments << "get" << "-o" << "yaml" << "pod" << pod;
    this->setWindowTitle("INFO: " + pod);
    process->start("oc", arguments);

}

void LogWindow::processPurge(int stage) {
    QStringList arguments;
    this->setWindowTitle("PURGE! ");

    switch (stage) {
        case 1:
            connect(process, SIGNAL(finished(int)), this, SLOT(stageCompleted()));
            arguments << "get" << "pods" << "--all-namespaces" << "--field-selector"
                      << "status.phase!=ContainerCreating,status.phase!=Running,metadata.namespace!=default" << "-o"
                      << "jsonpath={range .items[*].metadata}{@.namespace} {@.name};{end}";
            break;
        case 2:
            if (!pods.isEmpty()) {
                if (currentProject == pods.first().getProject()) {
                    arguments << "delete" << "pod" << pods.first().getName() << "--force";
                    pods.removeFirst();
                } else {
                    arguments << "project" << pods.first().getProject();
                    currentProject = pods.first().getProject();
                }
            } else {
                stageCompleted();
                return;
            }
            break;
        case 3:
            arguments << "adm" << "prune" << "builds" << "--keep-complete=1" << "--keep-failed=0" << "--orphans=true"
                      << "--keep-younger-than=1h0m0s" << "--confirm";
            break;
        case 4:
            arguments << "adm" << "prune" << "deployments" << "--keep-complete=1" << "--keep-failed=0"
                      << "--orphans=true" << "--keep-younger-than=1h0m0s" << "--confirm";
            break;
        case 5:
            arguments << "adm" << "prune" << "images" << "--all" << "--keep-tag-revisions=1"
                      << "--keep-younger-than=1h0m0s" << "--registry-url=docker-registry-default.app.netcom-netz.priv"
                      << "--confirm";
            break;
        case 6:
            ui->log->appendPlainText("THE PURGE IS OVER!");
            return;
    }


    process->start("oc", arguments);

}

void LogWindow::stageCompleted() {
    if (pods.isEmpty() || stage == 1)
        stage++;
    if (stage < 7) {
        processPurge(stage);
    }
}

LogWindow::~LogWindow() {
    delete process;
    delete ui;
}

void LogWindow::onUpdate(QString data) {
    ui->log->appendPlainText(data);
}

void LogWindow::processOutput() {
    QString out = process->readAllStandardOutput();
    QString err = process->readAllStandardError();
    if (stage == 1) {
        QStringList podList = out.split(";");
        for (QString pod : podList) {
            QStringList details = pod.split(" ");
            if (details.size() > 1)
                pods.append(Pod(details[0], details[1]));
        }
    } else {
        if (out.length() > 1)
            ui->log->appendPlainText(out);
        if (err.length() > 1)
            ui->log->appendPlainText(err);
    }
}
