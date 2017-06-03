#include "editor.h"
#include "highlighter.h"
#include <QTextStream>
#include <QTextEdit>
#include <QtWidgets>
#include <QDebug>

Editor::Editor(QWidget *parent):QPlainTextEdit(parent)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);
    setWindowTitle("Editor:");
    highlighter = new Highlighter(document());
    QWidget::connect(this,SIGNAL(textChanged()),this,SLOT(setTextEdited()));
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
   // highlightCurrentLine();
}
void Editor::keyPressEvent(QKeyEvent* e)
{
    if((e->key())==Qt::Key_Return){
        //TODO make line numbers in document
    }
    QPlainTextEdit::keyPressEvent(e);
}
void Editor::setTextEdited()
{
    setWindowModified(true);
}

bool Editor::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::ReadWrite|QIODevice::Truncate | QFile::Text)){
        QString txt = toPlainText();
        QTextStream out(&file);
        out<<txt;
        file.close();
        return true;}
    else
        return false;
}
//void Editor::clearErrors()
//void Editor::paintErrors(QList<Result> errors)
//{
//    textColor()
//    foreach(auto error, errors){
//        QTextBlock block_ = document()->findBlockByNumber(error.lineNr-1);
//        QTextCursor cursor_(block_);
//        QTextBlockFormat format = cursor_.blockFormat();
//        format.setBackground(Qt::red);
//        cursor_.setBlockFormat(format);
//    }

//}

bool Editor::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::ReadWrite|QFile::Text)){
        setPlainText(file.readAll());
        return true;}
    else
        return false;
}

int Editor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}



void Editor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void Editor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void Editor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
void Editor::find()
{

}
