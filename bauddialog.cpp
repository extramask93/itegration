#include "bauddialog.h"
#include "ui_bauddialog.h"

baudDialog::baudDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::baudDialog)
{
    ui->setupUi(this);
    connect(ui->okButton,SIGNAL (clicked()),this,SLOT (check()));
    connect(ui->cancelButton,SIGNAL (clicked()),this, SLOT (reject()));
}

int baudDialog::getBaud()
{
    return currentBaudValue;
}

void baudDialog::check()
{
   auto baudRateAsString = ui->lineEdit->text();
   bool isStringANumber;
   currentBaudValue=baudRateAsString.toUInt(&isStringANumber);
   if(isStringANumber)
       done(QDialog::Accepted);
   else
     QMessageBox::warning(this,tr("Error"),tr("Baud Rate is supposed to be non negative integer"),QMessageBox::Ok);
}

baudDialog::~baudDialog()
{
    delete ui;
}
