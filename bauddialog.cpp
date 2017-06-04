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
void baudDialog::check()
{
   auto str = ui->lineEdit->text();
   bool okz;
   str.toInt(&okz);
   if(okz)
       done(QDialog::Accepted);
   else
     QMessageBox::warning(this,tr("Error"),tr("Baud Rate is supposed to be non negative integer"),QMessageBox::Ok);
}

int baudDialog::getBaud()
{
    auto str = ui->lineEdit->text();
    return str.toInt();
}

baudDialog::~baudDialog()
{
    delete ui;
}
