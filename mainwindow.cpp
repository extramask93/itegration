#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "console.h"
#include "editor.h"
#include "interpreter.h"
#include <QDebug>
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    console = new Console(this);
    rsSettings = new SettingsDialog(this); //settings dialog
    serial = new SerialPort{this};
    interpreter = new Interpreter(console,this);
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
    ui->actionSend->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rsSettings;
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
    console->setEnabled(true);
    QWidget::connect(console,SIGNAL(commandIssued(QString)),interpreter,SLOT(processCommand(QString)));
    QWidget::connect(interpreter,SIGNAL(robotCommandIssued(QByteArray)),serial,SLOT(writeS(QByteArray)));
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
int MainWindow::compile()
{
    if(okToContinue())
    {
        if(interpreter->processScript(currentFileName))
        {
            return 1;
        }
        else
        {
            return 0;
        }
        console->prepareCommandLine();
    }
    else
        return 1;
}

void MainWindow::trySend()
{
    if(compile())
        return;
    if(!serial->isOpen())
    {
        console->printError("Serial connection not established");
        return;
    }
    Numberer numberer;
    QStringList commands=numberer.number(currentFileName);
    foreach (auto cmd, commands) {
        serial->write(cmd.toStdString().c_str());
        qDebug()<<cmd;
    }
    console->printMessage("File sent");

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
    serial->setPort(QSerialPortInfo(settings.name));
    serial->setDataBits(settings.dataBits);
    serial->setFlowControl(settings.flowControl);
    serial->setParity(settings.parity);
    serial->setStopBits(settings.stopBits);
    if(serial->open(QIODevice::ReadWrite))
    {
        console->printMessage("Connection established");
        //console->setEnabled(true);
        console->setLocalEchoEnabled(settings.localEchoEnabled);
        ui->actionConnect->setIcon(QIcon(":/rc/connected.png"));
        connected = true;
        ui->actionSettings->setEnabled(false);
        ui->actionSend->setEnabled(true);
    }
    else{
        QMessageBox::critical(this,tr("Connection Error"),serial->errorString()+serial->error());
    }
}

void MainWindow::closeSerialPort()
{
    if(serial->isOpen())
    {
        serial->close();
        console->printMessage("Disconnected");
    }
    //console->setEnabled(false);
    ui->actionSettings->setEnabled(true);
    ui->actionSend->setEnabled(false);
    connected=false;
    ui->actionConnect->setIcon(QIcon(":/rc/disconnected.png"));
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
void MainWindow::about()
{

}
