#include "senddialog.h"
#include "ui_senddialog.h"

SendDialog::SendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendDialog)
{
    ui->setupUi(this);
}

SendDialog::~SendDialog()
{
    delete ui;
}

void SendDialog::updateProgressBar(int max, int min, int current)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setMinimum(min);
    ui->progressBar->setValue(current);
}
