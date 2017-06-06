#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDockWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <tuple>
#include "settingsdialog.h"
#include "console.h"
#include "highlighter.h"
#include "editor.h"
#include "numberer.h"
#include "syntaxcheck.h"
#include "interpreter.h"
#include "serialport.h"
#include "qasyncqueue.h"
#include <QDebug>
#include <QListIterator>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    static const int MaxRecentFiles=4;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void newFile();
    bool saveFile(const QString &path = QString());
    void updateStatusBar();
    bool saveAs();
    bool save();
    void open();
    void trySend();
    void readData();
private slots:
    void checkState();
    int compile();
    void on_actionSettings_triggered();
    void about();
    void openRecentFile();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    SettingsDialog *rsSettings;
    Console *console;
    SerialPort *serial;
    QString currentFileName;
    Interpreter *interpreter;
    QStringList recentFiles;
    QAction *(recentFilesAction[MaxRecentFiles]);
    bool connected = false;
    void openSerialPort();
    void closeSerialPort();
    void setupEditor();
    void initFields();
    void setUIStyle();
    void setupFileMenu();
    void setupEditMenu();
    void setupToolsMenu();
    void setupSettingsMenu();
    void setupHelpMenu();
    void setupToolBar();
    void setupActions();
    void setupConsole();
    void setupDocking();
    void setupStatusBar();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &name);
    void updateRecentFilesAction();
    void loadSettings();

};

#endif // MAINWINDOW_H
