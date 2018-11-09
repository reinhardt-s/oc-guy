#ifndef LOADINGTHREAD_H
#define LOADINGTHREAD_H

#include <QThread>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QDebug>
#include <QProcess>
#include <QLabel>

class LoadingThread : public QThread {
public:
    Q_OBJECT
public:
    explicit LoadingThread(QObject *parent = nullptr);

    void run();

    QString getProject() const;

    void setProject(const QString &value);

    QString getPod() const;

    void setPod(const QString &value);

    signals:
            void
    statusChanged(QString);

    void valueChanged(int);

    void guyEnabled(bool);

    void projectChanged(QString);

    void podsLoaded(QStringList);

    void portsLoaded(QStringList);

private:
    QString project;
    QString pod;

    void changeProject();

    void loadPods();

    void loadPorts();

    QString sendCommand(QString program, QStringList arguments);
};

#endif // LOADINGTHREAD_H
