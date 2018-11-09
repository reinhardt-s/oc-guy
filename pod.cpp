#include "pod.h"

Pod::Pod() {

}

Pod::Pod(QString project, QString name) {
    this->setName(name);
    this->setProject(project);
}

QString Pod::getProject() const {
    return project;
}

void Pod::setProject(const QString &value) {
    project = value;
}

QString Pod::getName() const {
    return name;
}

void Pod::setName(const QString &value) {
    name = value;
}
