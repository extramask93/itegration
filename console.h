#ifndef CONSOLE_H
#define CONSOLE_H
#include <QPlainTextEdit>
#include <QWidget>
#include <QColor>
#include "interpreter.h"
#include "iprinter.h"
#include "logger.h"
#include <QFileDialog>

namespace Ui {
class Console;
}

class Console : public QPlainTextEdit, public IPrinter
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);
    ~Console();
    void setPrefix(const QString &prefix);
    QString getPrefix()const {return prefix;}
    void processCommand();
    void putData(const QByteArray &data);
    bool inCommandLine() const;
    void setLocalEchoEnabled(bool set);
    void setPrefixColor(const QColor &color);
    virtual void  printMessage(QString message) override;
    virtual void printError(QString) override ;

private:
    bool localEchoEnabled;
    int historyPos;
    QColor prefixColor;
    QStringList history;
    QString prefix;
    bool commandLineReady;
    Logger* logger;


signals:
    void getData(const QByteArray &data);
    void commandIssued(QString command);
public slots:
    void print(QString str);
    void prepareCommandLine();
    void clear(QString command);
    void printSerial(QByteArray message);
    void saveSession();
    void loadSession();
protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;



};

#endif // CONSOLE_H
