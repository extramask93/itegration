#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include "highlighter.h"
#include<QPlainTextEdit>
#include <QMessageBox>

class Editor : public QPlainTextEdit
{
    Q_OBJECT
private slots:
    void setTextEdited();
private:
        Highlighter *highlighter;
        QWidget *lineNumberArea;
public:
    Editor(QWidget *parent);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    bool writeFile(const QString &fileName);
    bool readFile(const QString &fileName);
    void keyPressEvent(QKeyEvent* e);
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(Editor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    Editor *codeEditor;
};
#endif // EDITOR_H
