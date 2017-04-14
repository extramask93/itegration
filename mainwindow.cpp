#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "console.h"
#include "editor.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowIcon(QIcon(":/robot.png"));
    ui->setupUi(this);
    setUIStyle();
    setupConsole();
    setupFileMenu();
    setupEditMenu();
    setupToolsMenu();
    setupSettingsMenu();
    setupActions();
    setupHelpMenu();
    setupDocking();
    rsSettings = new SettingsDialog(this); //settings dialog
    serial = new QSerialPort(this);
    //QWidget::connect(ui->editor, SIGNAL (textChanged()),this,SLOT(updateLineNumbers()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rsSettings;
}

void MainWindow::updateLineNumbers()
{
    qDebug("aaaa");
}

void MainWindow::setupDocking()
{
    QDockWidget *dock = new QDockWidget(tr("Terminal:"),this);
    dock->setWidget(console);
    addDockWidget(Qt::BottomDockWidgetArea,dock);
}

void MainWindow::newFile()
{
    if(okToContinue()){
        ui->editor->clear();
        setCurrentFile("");
    }
}
void MainWindow::open()
{
    if(okToContinue()){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Move Masted Files (*.mmc)");
        if(!fileName.isEmpty())
            loadFile(fileName);
    }
}
bool MainWindow::loadFile(const QString &fileName)
{
    if(!(ui->editor->readFile(fileName)))
        return false;
    setCurrentFile(fileName);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFileName=fileName;
    ui->editor->setWindowModified(false);
}

bool MainWindow::okToContinue()
{
    if(ui->editor->isWindowModified())
    {
        QMessageBox::StandardButton response = QMessageBox::warning(this,tr("Save the file?"),tr("File has been modified. Do you want to save changes?"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(response==QMessageBox::Yes)
            return save();
        else if(response==QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::setupConsole()
{
    console = new Console(this);
    console->setEnabled(true);
}


bool MainWindow::saveFile(const QString &fileName)
{
    if(!(ui->editor->writeFile(fileName))){
        return false;
    }
    setCurrentFile(fileName);
    return true;

}
bool MainWindow::save()
{
    if(currentFileName.isEmpty())
    {
        return saveAs();
    }
    else{
        return saveFile(currentFileName);
    }
}

bool MainWindow::saveAs()
{
    QString fName=QFileDialog::getSaveFileName(this,tr("Save file"),"",".mmc");
    if(fName.isEmpty())
    {
        return false;
    }
    return saveFile(fName);

}

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(open()), QKeySequence::Open);
    fileMenu->addAction(tr("&Save"),this, SLOT(save()),QKeySequence::Save);
    fileMenu->addAction(tr("&Save As..."),this, SLOT(saveAs()),QKeySequence::SaveAs);
    fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence::Quit);
}
void MainWindow::setupEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"),this);
    menuBar()->addMenu(editMenu);
    editMenu->addAction(tr("&Cut"),ui->editor, SLOT(cut()),QKeySequence::Cut);
    editMenu->addAction(tr("&Copy"),ui->editor, SLOT(copy()),QKeySequence::Copy);
    editMenu->addAction(tr("&Paste"),ui->editor, SLOT(paste()),QKeySequence::Paste);
    editMenu->addAction(tr("&SelectAll"),ui->editor, SLOT(selectAll()),QKeySequence::SelectAll);
    editMenu->addAction(tr("&Find"),ui->editor, SLOT(find()),QKeySequence::Find);
}
void MainWindow::setupToolsMenu()
{
    QMenu *toolsMenu = new QMenu(tr("&Tools"),this);
    menuBar()->addMenu(toolsMenu);
    toolsMenu->addAction(tr("&Connect"),this, SLOT(checkState()),QKeySequence("F3"));
    toolsMenu->addAction(tr("&Send"),this, SLOT(),QKeySequence("F5"));
    toolsMenu->addAction(tr("&Check Syntax"),this, SLOT(),QKeySequence("F4"));
}
void MainWindow::setupSettingsMenu()
{
    QMenu *settingsMenu = new QMenu(tr("&Settings"),this);
    menuBar()->addMenu(settingsMenu);
    settingsMenu->addAction(ui->actionSettings);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);
    helpMenu->addAction(tr("&About"), this, SLOT(about()));
}
void MainWindow::setupActions()
{
   QWidget::connect(ui->actionNew, SIGNAL(triggered(bool)),this, SLOT(newFile()));
   QWidget::connect(ui->actionOpen, SIGNAL(triggered(bool)),this, SLOT(open()));
   QWidget::connect(ui->actionSave, SIGNAL(triggered(bool)),this, SLOT(save()));
   QWidget::connect(ui->actionConnect, SIGNAL(triggered(bool)),this, SLOT(checkState()));
   QWidget::connect(ui->actionCheckSyntax, SIGNAL(triggered(bool)),this, SLOT(compile())); //TODO
   QWidget::connect(ui->actionSend, SIGNAL(triggered(bool)),this, SLOT(trySend())); //TODO
}
void MainWindow::compile()
{
    if(okToContinue())
    {
        SyntaxCheck check(this);
        check.checkFile(currentFileName);
        auto resultsT=check.errorlist;
        foreach (auto error, resultsT) {
            if(error.errorCode)
                console->print("'Error: "+error.errorString+" in line: "+QString::number(error.lineNr));
              //ui->editor->highlightError(std::get<1>(tpl));
        }
        ui->editor->paintErrors(resultsT);
        console->prepareCommandLine();
    }
}

void MainWindow::trySend()
{
    qDebug()<<"hello from slot";
    Numberer numberer;
    if(okToContinue())
    {
        //errorChecker(currentFileName);
        numberer.number(currentFileName);
    }
}
void MainWindow::checkState()
{
    if(connected)
        closeSerialPort();
    else
        openSerialPort();
}

void MainWindow::openSerialPort()
{
    auto settings = rsSettings->settings();
    serial->setBaudRate(settings.baudRate);
    serial->setDataBits(settings.dataBits);
    serial->setFlowControl(settings.flowControl);
    serial->setParity(settings.parity);
    serial->setStopBits(settings.stopBits);
    if(serial->open(QIODevice::ReadWrite))
    {
        console->setEnabled(true);
        console->setLocalEchoEnabled(settings.localEchoEnabled);
        //ui->connectButton->setIcon(QIcon(":/disconnected.png"));
        connected = true;
        //ui->settingsButton->setEnabled(false);
        //TODO show connected in console

    }
    else{
        QMessageBox::critical(this,tr("Connection Error"),serial->errorString());
        //TODO: show erron on console
    }
}

void MainWindow::closeSerialPort()
{
    if(serial->isOpen())
        serial->close();
        //TODO show disconnected on the console
    console->setEnabled(false);
    //ui->connectButton->setIcon(QIcon(":/connected.png"));
    connected=false;
    //ui->settingsButton->setEnabled(true);
}

void MainWindow::setUIStyle()
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

}


void MainWindow::on_actionSettings_triggered()
{
    rsSettings->show();
}
