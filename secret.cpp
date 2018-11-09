#include "secret.h"

Secret::Secret(QString name, QString value, QMap <QString, QString> data) {
    this->name = name;
    this->value = value;
    this->data = data;
}

QMap <QString, QString> Secret::getData() const {
    return data;
}

void Secret::setData(const QMap <QString, QString> &value) {
    data = value;
}

QString Secret::getValue() const {
    return value;
}

void Secret::setValue(const QString &value) {
    this->value = value;
}

QString Secret::getName() const {
    return name;
}

void Secret::setName(const QString &value) {
    name = value;
}
