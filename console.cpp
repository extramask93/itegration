#include "console.h"
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QTextCursor>
#include <QTextBlock>
#include <QApplication>
#include <QClipboard>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false)
{
    document()->setMaximumBlockCount(100);
    setCursorWidth(QFontMetrics(font()).width(QChar('x')));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setUndoRedoEnabled(false);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);
    setBackgroundVisible(false);
    setFrameStyle(QFrame::NoFrame);
    setTabStopWidth(40);
    setAcceptDrops(false);
    setPrefix(">> ");
    setPrefixColor(QColor(140,32,1));
    historyPos=-1;
    commandLineReady=false;
    prepareCommandLine();
}
void Console::printError(QString error)
{
    print(error);
}
void Console::printMessage(QString message)
{
    print(message);
}

void Console::setPrefix(const QString &prefix)
{
    this->prefix=prefix;
}

void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}
bool Console::inCommandLine() const
{
  return commandLineReady && document()->blockCount()-1 == textCursor().blockNumber() && textCursor().positionInBlock() >= prefix.length();
  //                                                                                          >>_
}

void Console::keyPressEvent(QKeyEvent *event)
{
  if (inCommandLine())
  {
    // clear selection that spans multiple blocks (or prefix characters) (would overwrite previous command lines):
    QTextCursor cur = textCursor();
    if (cur.hasSelection())
    {
      if (document()->findBlock(cur.selectionStart()) != document()->findBlock(cur.selectionEnd()) || // spans multiple blocks (including command line)
          cur.selectionStart()-cur.block().position() < prefix.length() || // spans prefix
          cur.selectionEnd()-cur.block().position() < prefix.length() ) // spans prefix
      {
        cur.clearSelection();
        if (cur.positionInBlock() < prefix.length())
          cur.setPosition(cur.block().position()+prefix.length());
        setTextCursor(cur);
      }
    }
    if (cur.positionInBlock() == prefix.length())
    {
      cur.setCharFormat(QTextCharFormat()); // make sure we don't pick up format from prefix
      setTextCursor(cur);
    }
    // react to keystroke:
    if (event->matches(QKeySequence::MoveToPreviousLine)) // history up
    {
      if (history.isEmpty() || historyPos >= history.size()-1)
        return;
      ++historyPos;
      int index = history.size()-historyPos-1;
      QTextCursor cur(document()->lastBlock());
      cur.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, prefix.length());
      cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
      cur.removeSelectedText();
      cur.setCharFormat(QTextCharFormat());
      cur.insertText(history.at(index));
      setTextCursor(cur);
    } else if (event->matches(QKeySequence::MoveToNextLine)) // history down
    {
      if (history.isEmpty() || historyPos <= 0)
        return;
      --historyPos;
      int index = history.size()-historyPos-1;
      QTextCursor cur(document()->lastBlock());
      cur.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, prefix.length());
      cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
      cur.removeSelectedText();
      cur.setCharFormat(QTextCharFormat());
      cur.insertText(history.at(index));
      setTextCursor(cur);
    } else if (event->matches(QKeySequence::Paste)) // paste text, do it manually to remove text char formatting and newlines
    {
      QString pasteText = QApplication::clipboard()->text();
      pasteText.replace("\n", "").replace("\r", "");
      cur.setCharFormat(QTextCharFormat());
      cur.insertText(pasteText);
      setTextCursor(cur);
    } else if (event->key() == Qt::Key_Return) // process command
    {
      processCommand();
    } else if (event->key() == Qt::Key_Backspace) // only allow backspace if we wouldn't delete last char of prefix
    {
      if (cur.positionInBlock() > prefix.length())
        QPlainTextEdit::keyPressEvent(event);
    } else if (!event->matches(QKeySequence::Close) &&
               !event->matches(QKeySequence::New) &&
               !event->matches(QKeySequence::Open) &&
               !event->matches(QKeySequence::Preferences) &&
               !event->matches(QKeySequence::Bold) &&
               !event->matches(QKeySequence::Italic) &&
               !event->matches(QKeySequence::InsertLineSeparator) &&
               !event->matches(QKeySequence::InsertParagraphSeparator) &&
               !event->matches(QKeySequence::Redo) &&
               !event->matches(QKeySequence::Undo) &&
               !event->matches(QKeySequence::DeleteStartOfWord))
    {
      QPlainTextEdit::keyPressEvent(event);
    }
  } else // cursor position not in command line
  {
    if (event->matches(QKeySequence::MoveToEndOfDocument) ||
        event->matches(QKeySequence::MoveToEndOfBlock) ||
        event->matches(QKeySequence::MoveToEndOfLine) ||
        event->matches(QKeySequence::MoveToStartOfDocument) ||
        event->matches(QKeySequence::MoveToStartOfBlock) ||
        event->matches(QKeySequence::MoveToStartOfLine) ||
        event->matches(QKeySequence::MoveToNextLine) ||
        event->matches(QKeySequence::MoveToNextWord) ||
        event->matches(QKeySequence::MoveToNextChar) ||
        event->matches(QKeySequence::MoveToPreviousLine) ||
        event->matches(QKeySequence::MoveToPreviousWord) ||
        event->matches(QKeySequence::MoveToPreviousChar) ||
        event->matches(QKeySequence::SelectAll) ||
        event->matches(QKeySequence::SelectEndOfDocument) ||
        event->matches(QKeySequence::SelectEndOfBlock) ||
        event->matches(QKeySequence::SelectEndOfLine) ||
        event->matches(QKeySequence::SelectStartOfDocument) ||
        event->matches(QKeySequence::SelectStartOfBlock) ||
        event->matches(QKeySequence::SelectStartOfLine) ||
        event->matches(QKeySequence::SelectNextLine) ||
        event->matches(QKeySequence::SelectNextWord) ||
        event->matches(QKeySequence::SelectNextChar) ||
        event->matches(QKeySequence::SelectPreviousLine) ||
        event->matches(QKeySequence::SelectPreviousWord) ||
        event->matches(QKeySequence::SelectPreviousChar) ||
        event->matches(QKeySequence::Copy) )
      QPlainTextEdit::keyPressEvent(event);
  }
}
void Console::prepareCommandLine(){ //must be called after successfull command evaluation
    QTextCursor cur(document()->lastBlock()); //set the cur to the last line
    if (!document()->lastBlock().text().isEmpty())
    {
      cur.movePosition(QTextCursor::EndOfBlock);
      cur.insertBlock();//new line
    }
    QTextCharFormat format;
    format.setForeground(QBrush(prefixColor));
    cur.setCharFormat(format);
    cur.insertText(prefix);
    setTextCursor(cur);
    commandLineReady = true;
}
void Console::setPrefixColor(const QColor&color)
{
    prefixColor=color;
}

void Console::processCommand()
{
    QString inputString = document()->lastBlock().text();
    if(inputString.startsWith(prefix))
        inputString.remove(0,prefix.length());
    if(!inputString.trimmed().isEmpty())
        history.append(inputString);
    historyPos=-1;
    inputString=inputString.trimmed(); //remove whitespaces
    QTextCursor cur(document()->lastBlock());
    cur.movePosition(QTextCursor::EndOfBlock);
    cur.insertBlock();
    setTextCursor(cur);
    commandLineReady=false;
    emit commandIssued(inputString);
    prepareCommandLine();
}
void Console::print(QString str)
{
  QTextCursor cur(document()->lastBlock());
  cur.setCharFormat(QTextCharFormat());
  cur.insertText(str);
  cur.movePosition(QTextCursor::EndOfBlock);
  cur.insertBlock();
  setTextCursor(cur);
  commandLineReady = false;
}
void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
