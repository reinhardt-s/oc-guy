#include "loadingthread.h"

LoadingThread::LoadingThread(QObject *parent) :
        QThread(parent) {
    this->project = project;
}


void LoadingThread::run() {

    emit guyEnabled(false);

    if (pod.length() == 0) {
        emit valueChanged(25);
        changeProject();
        emit valueChanged(50);
        loadPods();
    }
    emit valueChanged(90);
    if (pod.length() > 0) {
        loadPorts();
    }
    emit valueChanged(100);

    emit statusChanged("Loaded.");
    emit guyEnabled(true);
    emit valueChanged(0);


}

QString LoadingThread::getProject() const {
    return project;
}

void LoadingThread::setProject(const QString &value) {
    project = value;
}

QString LoadingThread::getPod() const {
    return pod;
}

void LoadingThread::setPod(const QString &value) {
    pod = value;
}


void LoadingThread::changeProject() {
    emit statusChanged("Changing project...");
    QString
    program = "oc";
    QStringList arguments;
    arguments << "project" << project;

    QString
    result = sendCommand(program, arguments);
    emit projectChanged(result);
}

void LoadingThread::loadPods() {
    emit statusChanged("Loading pods...");
    QString
    program = "oc";
    QStringList arguments;
    arguments << "get" << "po" << "-o" << "jsonpath={.items[*].metadata.name}";

    QString
    result = sendCommand(program, arguments);

    emit podsLoaded(result.split(" "));
}

void LoadingThread::loadPorts() {
    emit statusChanged("Loading ports...");
    QString
    program = "oc";
    QStringList arguments;
//    arguments << "get" << "svc"<< "-o" << "jsonpath='{.items[*].spec.ports[*].targetPort}'";
    arguments << "get" << "-o" << "jsonpath={.spec.containers[*].ports[*].containerPort}" << "pod" << pod;

    QString
    result = sendCommand(program, arguments);
    emit portsLoaded(result.split(" "));
}

QString LoadingThread::sendCommand(QString program, QStringList arguments) {
    QProcess *process = new QProcess();
    process->start(program, arguments);

    if (!process->waitForFinished(4000)) {
        qDebug() << "HALP";
        return "";
    }

    QByteArray result = process->readAll();
    QString
    out = QString(result);
    qDebug() << out;
    return out;
}

