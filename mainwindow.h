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
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void newFile();
    bool saveFile(const QString &path = QString());
    bool saveAs();
    bool save();
    void open();
    void updateLineNumbers();
    void trySend();
private slots:
       void checkState();
       void compile();
       void on_actionSettings_triggered();

private:
    Ui::MainWindow *ui;
    SettingsDialog *rsSettings;
    Console *console;
    QSerialPort *serial;
    QString currentFileName;
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
    bool okToContinue();
    bool loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

};

#endif // MAINWINDOW_H
