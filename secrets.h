#ifndef SECRETS_H
#define SECRETS_H

#include <QWidget>
#include <QProcess>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include <QTableWidgetItem>

#include "secret.h"

namespace Ui {
    class Secrets;
}

class Secrets : public QWidget {
    Q_OBJECT

public:
    explicit Secrets(QString project, QWidget *parent = nullptr);

    ~Secrets();

    QString getProject() const;

    void setProject(const QString &value);

protected
    slots:
            void

    processOutput();

private
    slots:
            void

    on_secretList_currentRowChanged(int currentRow);

private:
    Ui::Secrets *ui;
    QProcess *process;
    QString project;
    QList <Secret> secretList;

    void loadSecrets();
};

#endif // SECRETS_H
