#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "console.h"
#include "editor.h"
#include "interpreter.h"
#include "qasyncqueue.h"
#include <QDebug>
#include <QSerialPortInfo>
//global queue holds data to send
constexpr const unsigned int sQ = 20;
QAsyncQueue<QByteArray> queue{sQ};

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
    //TODO
    QString shownName = tr("Untitled");
    if(!currentFileName.isEmpty())
    {
        shownName=strippedName(currentFileName);
        recentFiles.removeAll(currentFileName);
        recentFiles.prepend(currentFileName);
        updateRecentFilesAction();
    }
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Script")));
}

QString MainWindow::strippedName(const QString &name)
{
    return QFileInfo(name).fileName();
}

void MainWindow::updateRecentFilesAction()
{
    QMutableStringListIterator i(recentFiles);
    while(i.hasNext())
    {
        if(!QFile::exists(i.next()))
            i.remove();
    }
    for(int j=0;j<MaxRecentFiles;++j)
    {
        if(j<recentFiles.count())
        {

            QString text=tr("&%1 %2").arg(j+1).arg(strippedName(recentFiles[j]));
            recentFilesAction[j]->setText(text);
            recentFilesAction[j]->setData(recentFiles[j]);
            recentFilesAction[j]->setVisible(true);
        }
        else
            recentFilesAction[j]->setVisible(false);
    }
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
    for(int i=0;i<MaxRecentFiles;i++)
    {
        recentFilesAction[i]=new QAction(this);
        recentFilesAction[i]->setVisible(false);
        connect(recentFilesAction[i],SIGNAL(triggered()),this,SLOT(openRecentFile()));
    }
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(tr("&New"), this, SLOT(newFile()), QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(open()), QKeySequence::Open);
    fileMenu->addAction(tr("&Save"),this, SLOT(save()),QKeySequence::Save);
    fileMenu->addAction(tr("&Save As..."),this, SLOT(saveAs()),QKeySequence::SaveAs);
    fileMenu->addSeparator();
    for(int i=0;i<MaxRecentFiles;++i)
    {
        fileMenu->addAction(recentFilesAction[i]);
    }
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
void MainWindow::openRecentFile()
{
    if(okToContinue())
    {
        QAction *action = qobject_cast<QAction*>(sender());
        if(action)
            loadFile(action->data().toString());
    }
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
    QList<QByteArray> cmds;
    foreach (auto l, commands) {
        cmds.append(l.toUtf8());

    }
    queue.pushMany(cmds.begin(),cmds.end());
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
    serial->WriteSettings(settings.name,settings.baudRate,settings.dataBits,settings.parity,settings.stopBits,settings.flowControl);
    if(serial->open())
    {
        console->printMessage("Connection established");
        //console->setEnabled(true);
        console->setLocalEchoEnabled(settings.localEchoEnabled);
        ui->actionConnect->setIcon(QIcon(":/rc/connected.png"));
        connected = true;
        ui->actionSettings->setEnabled(false);
        ui->actionSend->setEnabled(true);
        serial->start();
    }
    else{
        QMessageBox::critical(this,tr("Connection Error"),serial->port.errorString()+serial->port.error());
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
    serial->terminate();
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
