#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(on_listWidget_itemClicked(QListWidgetItem*)));
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
void HelpDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
   int a;
   a=ui->listWidget->currentRow();
   czytaj(a);

}

void HelpDialog::on_pushButton_clicked()
{
}
void HelpDialog::czytaj(int a){
    QString S;
    S=":/help/";
    switch( a )
    {
    case 0:{S=S+"how.txt";}break;
        case 1:{S=S+"cl.txt";}break;
        case 2:{S=S+"cp.txt";}break;
        case 3:{S=S+"cr.txt";}break;
        case 4:{S=S+"ds.txt";}break;
        case 5:{S=S+"dc.txt";}break;
        case 6:{S=S+"eq.txt";}break;
        case 7:{S=S+"gc.txt";}break;
        case 8:{S=S+"go.txt";}break;
        case 9:{S=S+"gs.txt";}break;
        case 10:{S=S+"gt.txt";}break;
        case 11:{S=S+"he.txt";}break;
        case 12:{S=S+"ic.txt";}break;
        case 13:{S=S+"lg.txt";}break;
        case 14:{S=S+"mc.txt";}break;
        case 15:{S=S+"mj.txt";}break;
        case 16:{S=S+"mo.txt";}break;
        case 17:{S=S+"mra.txt";}break;
        case 18:{S=S+"ms.txt";}break;
        case 19:{S=S+"ne.txt";}break;
        case 20:{S=S+"ob.txt";}break;
        case 21:{S=S+"ovr.txt";}break;
        case 22:{S=S+"pd.txt";}break;
        case 23:{S=S+"rc.txt";}break;
        case 24:{S=S+"rt.txt";}break;
        case 25:{S=S+"sc.txt";}break;
        case 26:{S=S+"sd.txt";}break;
        case 27:{S=S+"sm.txt";}break;
        case 28:{S=S+"tbd.txt";}break;
        case 29:{S=S+"ti.txt";}break;
        case 30:{S=S+"wh.txt";}break;
    }
        QFile file(S);
        if(!file.open(QIODevice::ReadOnly));
        QTextStream in(&file);
        ui->textEdit->setText(in.readAll());
}
