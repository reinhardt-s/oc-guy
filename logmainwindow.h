#ifndef LOGMAINWINDOW_H
#define LOGMAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QProcess>
#include <QDateTime>
#include <QLineEdit>
#include <QListWidget>
#include <QSettings>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include "pod.h"
#include "searchhit.h"

namespace Ui {
    class LogMainWindow;
}

class LogMainWindow : public QMainWindow {
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);

public:
    explicit LogMainWindow(QString pod, QString type, QWidget *parent = nullptr, QProcess *process = nullptr);

    ~LogMainWindow();

    QString getPod() const;

    void setPod(const QString &value);

public
    slots:
            void

    stageCompleted();

    void onUpdate(QString data);

    void processOutput();


protected
    slots:
            void

    searchText();

    void gotoHighlight(int);

private
    slots:


            void

    on_actionSave_triggered();

private:
    Ui::LogMainWindow *ui;
    QProcess *process;
    int stage = 0;
    QString currentProject = "";
    QList <Pod> pods;
    QString pod;
    QLabel *searchLabel;
    QLineEdit *searchLine;
    QListWidget *hits;

    QList <SearchHit> searchHits;

    void saveLog();

    void processLog(QString pod);

    void processInfo(QString pod);

    void processPurge(int stage);
};

#endif // LOGMAINWINDOW_H
