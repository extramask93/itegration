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


////////////
//bool Editor::okToContinue()
//{
//    if (document()->isModified()) {
//        int r = QMessageBox::warning(this, tr("MDI Editor"),
//                        tr("File %1 has been modified.\n"
//                           "Do you want to save your changes?")
//                        .arg(strippedName(curFile)),
//                        QMessageBox::Yes | QMessageBox::No
//                        | QMessageBox::Cancel);
//        if (r == QMessageBox::Yes) {
//            return save();
//        } else if (r == QMessageBox::Cancel) {
//            return false;
//        }
//    }
//    return true;
//}

//bool Editor::saveFile(const QString &fileName)
//{
//    if (writeFile(fileName)) {
//        setCurrentFile(fileName);
//        return true;
//    } else {
//        return false;
//    }
//}
//bool Editor::save()
//{
//    if (isUntitled) {
//        return saveAs();
//    } else {
//        return saveFile(curFile);
//    }
//}

//bool Editor::saveAs()
//{
//    QString fileName =
//            QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
//    if (fileName.isEmpty())
//        return false;
//    return saveFile(fileName);
//}
//bool Editor::readFile(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI Editor"),
//                             tr("Cannot read file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

//    QTextStream in(&file);
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//    setPlainText(in.readAll());
//    QApplication::restoreOverrideCursor();
//    return true;
//}

//bool Editor::writeFile(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI Editor"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

//    QTextStream out(&file);
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//    out << toPlainText();
//    QApplication::restoreOverrideCursor();
//    return true;
//}
//Editor *Editor::open(QWidget *parent)
//{
//    QString fileName =
//            QFileDialog::getOpenFileName(parent, tr("Open"), ".");
//    if (fileName.isEmpty())
//        return 0;

//    return openFile(fileName, parent);
//}

//Editor *Editor::openFile(const QString &fileName, QWidget *parent)
//{
//    Editor *editor = new Editor(parent);
//    if (editor->readFile(fileName)) {
//        editor->setCurrentFile(fileName);
//        return editor;
//    } else {
//        delete editor;
//        return 0;
//    }
//}

//void Editor::closeEvent(QCloseEvent *event)
//{
//    if (okToContinue()) {
//        event->accept();
//    } else {
//        event->ignore();
//    }
//}
//void Editor::newFile()
//{
//    static int documentNumber = 1;

//    curFile = tr("document%1.txt").arg(documentNumber);
//    setWindowTitle(curFile + "[*]");
//    action->setText(curFile);
//    isUntitled = true;
//    ++documentNumber;
//}
//QString Editor::strippedName(const QString &fullFileName)
//{
//    return QFileInfo(fullFileName).fileName();
//}



