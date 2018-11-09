#include "logmainwindow.h"
#include "ui_logmainwindow.h"

LogMainWindow::LogMainWindow(QString pod, QString type, QWidget *parent, QProcess *process) :
        QMainWindow(parent),
        ui(new Ui::LogMainWindow) {
    ui->setupUi(this);

    searchLabel = new QLabel("Search");
    searchLine = new QLineEdit();
    hits = new QListWidget();

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(searchLabel);
    ui->toolBar->addWidget(searchLine);
    ui->toolBar->addWidget(hits);

    !process ? this->process = new QProcess() : this->process = process;
    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
    connect(this->process, SIGNAL(readyReadStandardError()), this, SLOT(processOutput()));
    connect(this->searchLine, SIGNAL(returnPressed()), this, SLOT(searchText()));
    connect(this->hits, SIGNAL(currentRowChanged(int)), this, SLOT(gotoHighlight(int)));


    this->setPod(pod);

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

void LogMainWindow::gotoHighlight(int row) {

    if (row == -1)
        return;

    ui->log->setTextCursor(searchHits.at(row).getCursor());
}

void LogMainWindow::searchText() {

    hits->clear();
    searchHits.clear();
    if (searchLine->text().isEmpty()) {
        return;
    }

    QTextCursor highlightCursor(ui->log->document());
    QTextCursor cursor(ui->log->document());
    bool found = false;


    cursor.beginEditBlock();

    QTextCharFormat plainFormat(highlightCursor.charFormat());


    while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
        highlightCursor = ui->log->document()->find(searchLine->text(), highlightCursor);

        if (!highlightCursor.isNull()) {
            found = true;
            searchHits.append(
                    SearchHit(QString::number(highlightCursor.blockNumber()) + ": " + highlightCursor.selectedText(),
                              highlightCursor));
            hits->addItem(searchHits.last().getValue());
            highlightCursor.movePosition(QTextCursor::WordRight,
                                         QTextCursor::KeepAnchor);
        }
    }

    cursor.endEditBlock();
}

void LogMainWindow::processLog(QString pod) {
    QSettings settings("guy.ini",
                       QSettings::IniFormat);

    QStringList arguments;
    arguments << "logs" << pod << "--tail=" + QString::number(settings.value("logLength").toInt()) << "-f";
    this->setWindowTitle("LOG: " + pod);
    process->start("oc", arguments);
}

void LogMainWindow::processInfo(QString pod) {
    QStringList arguments;
    arguments << "get" << "-o" << "yaml" << "pod" << pod;
    this->setWindowTitle("INFO: " + pod);
    process->start("oc", arguments);
}

void LogMainWindow::processPurge(int stage) {
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

void LogMainWindow::stageCompleted() {
    if (pods.isEmpty() || stage == 1)
        stage++;
    if (stage < 7) {
        processPurge(stage);
    }
}

void LogMainWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "Closing log";
    process->close();
    process->kill();
    delete process;
    delete ui;
}

LogMainWindow::~LogMainWindow() {

}

void LogMainWindow::onUpdate(QString data) {
    ui->log->appendPlainText(data);
}

void LogMainWindow::processOutput() {
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
    ui->statusbar->showMessage(
            "Last new input at: " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzzz"));
    searchText();
}

void LogMainWindow::saveLog() {

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Log File"), pod, tr("Log File (*.txt)"));

    if (fileName == "")
        return;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(QByteArray::fromStdString(ui->log->toPlainText().toStdString()));
    file.close();
}


QString LogMainWindow::getPod() const {
    return pod;
}

void LogMainWindow::setPod(const QString &value) {
    pod = value;
}

void LogMainWindow::on_actionSave_triggered() {
    saveLog();
}
