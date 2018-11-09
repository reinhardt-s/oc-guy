#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QProcess>
#include "pod.h"

namespace Ui {
    class LogWindow;
}

class LogWindow : public QWidget {
    Q_OBJECT

public:
    explicit LogWindow(QString pod, QString type, QWidget *parent = nullptr, QProcess *process = nullptr);

    ~LogWindow();

public
    slots:
            void

    stageCompleted();

    void onUpdate(QString data);

    void processOutput();

private:
    Ui::LogWindow *ui;
    QProcess *process;
    int stage = 0;
    QString currentProject = "";
    QList <Pod> pods;

    void processLog(QString pod);

    void processInfo(QString pod);

    void processPurge(int stage);
};

#endif // LOGWINDOW_H
