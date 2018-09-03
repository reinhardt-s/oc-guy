#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);
    ~ProcessDialog();

private:
    Ui::ProcessDialog *ui;
};

#endif // PROCESSDIALOG_H
