#ifndef SENDDIALOG_H
#define SENDDIALOG_H

#include <QDialog>

namespace Ui {
class SendDialog;
}

class SendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendDialog(QWidget *parent = 0);
    ~SendDialog();
public slots:
    void updateProgressBar(int max,int min,int current);
private:
    Ui::SendDialog *ui;
};

#endif // SENDDIALOG_H
