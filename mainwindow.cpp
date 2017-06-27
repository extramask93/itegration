#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    rsSettings = new SettingsDialog(this);
    serial = new SerialPort{this};
    senddialog = new SendDialog{this};
    helpdialog = new HelpDialog(this);
    interpreter = new Interpreter(ui->console,serial,this);
    setWindowIcon(QIcon(":/robot.png"));
    ui->setupUi(this);
    ui->centralWidget->hide();
    setUIStyle();
    setupConsole();
    setupFileMenu();
    setupEditMenu();
    setupToolsMenu();
    setupDocking();
    setupSettingsMenu();
    setupSessionMenu();
    setupWindowMenu();
    setupActions();
    setupHelpMenu();
    setupStatusBar();
    setupCheckIcons();
    loadSettings();
    disableOnlineFunctionality(true);
    connect(this,SIGNAL(lineSent(int,int,int)),senddialog,SLOT(updateProgressBar(int,int,int)));
    connect(serial,SIGNAL(messageArrived(QByteArray)),ui->console,SLOT(printSerial(QByteArray)));
    connect(interpreter,SIGNAL(errorOccured(QString)),ui->console,SLOT(print(QString)));
    connect(serial,SIGNAL(writeTimeOut()),this,SLOT(inform()));
}


MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
    delete rsSettings;
}

void MainWindow::setupDocking()
{
    dock = new QDockWidget(tr("Terminal:"),this);
    dock2 = new QDockWidget(tr("Editor:"),this);
    dock2->setWidget(ui->editor);
    dock->setWidget(ui->console);
    dock->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    dock2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    addDockWidget(Qt::BottomDockWidgetArea,dock);
    addDockWidget(Qt::TopDockWidgetArea,dock2);
}

void MainWindow::updateStatusBar()
{
    auto cursor=ui->editor->textCursor();
    int line = cursor.blockNumber()+1;
    int column = cursor.columnNumber()+1;
    ui->statusBar->showMessage("Line: "+QString::number(line)+", Col: "+QString::number(column));
}

void MainWindow::setupStatusBar()
{
    connect(ui->editor,SIGNAL(cursorPositionChanged()),this,SLOT(updateStatusBar()));
}

void MainWindow::setupCheckIcons()
{

    if(interpreter->isCheckingOn())
    {
        ui->actionOffSyntaxCheck->setIcon(QIcon(":/rc/dictionary.png"));
        ui->actionCheckSyntax->setDisabled(false);
    }
    else
    {
        ui->actionOffSyntaxCheck->setIcon(QIcon(":/rc/dictionary_off.png"));
        ui->actionCheckSyntax->setDisabled(true);
    }
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
        QString filter{"Move Master Command (*.mmc);; Text Files (*.txt)"};
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", filter,&filter);
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

void MainWindow::saveSettings()
{
    QSettings settings_("Jozek","Robots");
    settings_.beginGroup("MWSettings");
    settings_.remove("");
    int i=0;
    foreach(auto str, recentFiles)
    {
        QString temp = "Recent" + QString::number(i++);
        settings_.setValue(temp,str);
    }

}

void MainWindow::run()
{
    if(!okToContinue())
        return;
    if(currentFileName.isEmpty())
        return;
    const int lengthOfExtension=4;
    QString nameWithExtension= strippedName(currentFileName);
    nameWithExtension.chop(lengthOfExtension);
    QString command = tr("N\"%1\"\r").arg(nameWithExtension);
    QByteArray array = command.toLocal8Bit();
    serial->writeS(array);
    serial->writeS("RN\r");
    ui->statusBar->showMessage("Running");
}

void MainWindow::stop()
{
    QString s="HLT\r";
    serial->writeS(s.toLocal8Bit());
    ui->statusBar->showMessage("Stopped");
}


void MainWindow::loadSettings()
{
    QSettings settings_("Jozek","Robots");
    settings_.beginGroup("MWSettings");
    auto temp=settings_.allKeys();
    foreach(auto entry,temp)
    {
        recentFiles.append(settings_.value(entry).toString());
    }
    updateRecentFilesAction();
}

void MainWindow::disableOnlineFunctionality(bool state)
{
    ui->actionSend->setDisabled(state);
    ui->actionRun->setDisabled(state);
    ui->actionStop->setDisabled(state);
    ui->console->setDisabled(state);
    ui->actionSettings->setEnabled(state);
    if(state)
        ui->actionConnect->setIcon(QIcon(":/rc/disconnected.png"));
    else
        ui->actionConnect->setIcon(QIcon(":/rc/connected.png"));
    isConnected=!state;
}

bool MainWindow::okToContinue()
{
    if(currentFileName.isEmpty() && (ui->editor->toPlainText().isEmpty()))
        return true;
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
    ui->console->setEnabled(true);
    QWidget::connect(ui->console,SIGNAL(commandIssued(QString)),interpreter,SLOT(processCommand(QString)));
}


bool MainWindow::saveFile(const QString &fileName)
{
    if(!(ui->editor->writeFile(fileName))){
        return false;
    }
    setCurrentFile(fileName);
    ui->statusBar->showMessage("Saved",2000);
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
    QString filter{"Move Master Command (*.mmc);; Text Files (*.txt)"};
    QString fName=QFileDialog::getSaveFileName(this,tr("Save file"),"",filter,&filter);
    if(fName.isEmpty())
    {
        return false;
    }
    return saveFile(fName);

}

void MainWindow::print()
{
    QString filter{"Text Files (*.txt)"};
    QString fName=QFileDialog::getSaveFileName(this,tr("Save file"),"",filter,&filter);
    if(fName.isEmpty())
    {
        return;
    }
    else
    {
        QFile file{fName};
        file.open(QIODevice::WriteOnly|QIODevice::Text);
        QStringList lines;
        try{
            Script script{currentFileName};
            lines=script.number().addLineBreaks().getContent();
        }
        catch(std::exception &ex)
        {
            QMessageBox::warning(nullptr,"Print error",ex.what(),QMessageBox::Ok);
        }

        QTextStream out{&file};
        foreach (auto line, lines) {
            out<<line;
        }
        file.close();
        ui->statusBar->showMessage("Printed",2000);
    }
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
    fileMenu->addAction(tr("&Print"),this,SLOT(print()),QKeySequence::Print);
    fileMenu->addAction(tr("E&xit"), this, SLOT(close()), QKeySequence::Quit);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(serial->isOpen())
        serial->close();
    if(okToContinue())
    {
        event->accept();
    }
    else
        event->ignore();
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
    toolsMenu->addAction(tr("&Send"),this, SLOT(trySend()),QKeySequence("F5"));
    toolsMenu->addAction(tr("&Run"),this,SLOT(run()));
    toolsMenu->addAction(tr("&Stop"),this,SLOT(stop()));
    toolsMenu->addAction(tr("&Check Syntax"),this, SLOT(),QKeySequence("F4"));
}

void MainWindow::setupSessionMenu()
{
    QMenu *sessionMenu= new QMenu(tr("Session"),this);
    menuBar()->addMenu(sessionMenu);
    sessionMenu->addAction(ui->actionExport_Session);
    sessionMenu->addAction(ui->actionImport_Session);
}
void MainWindow::setupSettingsMenu()
{
    QMenu *settingsMenu = new QMenu(tr("&Settings"),this);
    menuBar()->addMenu(settingsMenu);
    settingsMenu->addAction(ui->actionSettings);
    settingsMenu->addAction(ui->actionOmmitUnknown);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);
    helpMenu->addAction(tr("&Help"),helpdialog,SLOT(show()));
    helpMenu->addAction(tr("&About"), this, SLOT(about()));
}
void MainWindow::setupWindowMenu()
{
    QMenu *windowMenu = new QMenu(tr("&Window"),this);
    menuBar()->addMenu(windowMenu);
    windowMenu->addAction("Editor Window",this->dock2,SLOT(show()));
    windowMenu->addAction("Console Window",this->dock,SLOT(show()));
}
void MainWindow::setupActions()
{
   QWidget::connect(ui->actionNew, SIGNAL(triggered(bool)),this, SLOT(newFile()));
   QWidget::connect(ui->actionOpen, SIGNAL(triggered(bool)),this, SLOT(open()));
   QWidget::connect(ui->actionSave, SIGNAL(triggered(bool)),this, SLOT(save()));
   QWidget::connect(ui->actionConnect, SIGNAL(triggered(bool)),this, SLOT(checkState()));
   QWidget::connect(ui->actionCheckSyntax, SIGNAL(triggered(bool)),this, SLOT(compile())); //TODO
   QWidget::connect(ui->actionSend, SIGNAL(triggered(bool)),this, SLOT(trySend())); //TODO
   QWidget::connect(ui->actionOffSyntaxCheck,SIGNAL(triggered(bool)),interpreter,SLOT(toggleChecker()));
   QWidget::connect(interpreter,SIGNAL(changed()),this,SLOT(setupCheckIcons()));
   QWidget::connect(interpreter,SIGNAL(controlCommandIssued(QString)),ui->console,SLOT(clear(QString)));
   QWidget::connect(ui->actionRun,SIGNAL(triggered(bool)),this,SLOT(run()));
   QWidget::connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stop()));
   QWidget::connect(ui->actionOmmitUnknown,SIGNAL(triggered(bool)),interpreter,SLOT(setOmmiting(bool)));
   QWidget::connect(ui->actionExport_Session,SIGNAL(triggered(bool)),ui->console,SLOT(saveSession()));
   QWidget::connect(ui->actionImport_Session,SIGNAL(triggered(bool)),ui->console,SLOT(loadSession()));
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
    bool isError=false;
    if(okToContinue())
    {
        try
        {
            isError=interpreter->processScript(currentFileName);
            return isError;
        }
        catch(std::runtime_error &ex)
        {
            QMessageBox::critical(this,"Syntax check Error!",ex.what(),QMessageBox::Ok);
            return 1;
        }
        return 0;
    }
    else
        return 1;
}

void MainWindow::trySend()
{
    if(compile())
    {
        return;
    }

    if(!serial->isOpen())
    {
        QMessageBox::critical(this,"Connection Error!","Serial connection not established",QMessageBox::Ok);
        return;
    }
    Script currentScript{currentFileName};
    serial->writeFile(currentScript);

}
void MainWindow::checkState()
{
    if(isConnected)
        closeSerialPort();
    else
        openSerialPort();
}

void MainWindow::openSerialPort()
{

    auto settings = rsSettings->getSettings();
    serial->WriteSettings(settings.name,settings.baudRate,settings.dataBits,settings.parity,settings.stopBits,settings.flowControl);
    if(serial->open())
    {
        ui->console->printMessage("Connection established");
        ui->console->setLocalEchoEnabled(settings.localEchoEnabled);
        disableOnlineFunctionality(false);

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
        ui->console->printMessage("Disconnected");
    }
    ui->actionSettings->setEnabled(true);
    disableOnlineFunctionality();
    serial->terminate();
}

void MainWindow::setUIStyle()
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        QMessageBox::warning(this,"Warning","Unable to set stylesheet, file not found",QMessageBox::Ok);
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
void MainWindow::inform()
{
    QMessageBox::warning(this,"Error","write timeout, please try again",QMessageBox::Ok);
}
