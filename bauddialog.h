#ifndef BAUDDIALOG_H
#define BAUDDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class baudDialog;
}

class baudDialog : public QDialog
{
    Q_OBJECT
    unsigned int currentBaudValue;
public:
    explicit baudDialog(QWidget *parent = 0);
    int getBaud();

    ~baudDialog();
private slots:
    void check();
private:
    Ui::baudDialog *ui;
};

#endif // BAUDDIALOG_H
