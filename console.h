#ifndef CONSOLE_H
#define CONSOLE_H
#include <QPlainTextEdit>
#include <QWidget>

namespace Ui {
class Console;
}

class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);

    void putData(const QByteArray &data);

    void setLocalEchoEnabled(bool set);

private:
    bool localEchoEnabled;
signals:
    void getData(const QByteArray &data);
protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;



};

#endif // CONSOLE_H
