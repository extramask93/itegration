#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTextStream>
#include <QFile>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = 0);
    ~HelpDialog();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();
    void czytaj(int);

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
