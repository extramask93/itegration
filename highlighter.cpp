#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    keywordFormat.setForeground(Qt::green);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bCF\\b" << "\\bDJ\\b" << "\\bDP\\b"
                    << "\\bDS\\b" << "\\bDW\\b" << "\\bHE\\b"
                    << "\\bHO\\b" << "\\bIP\\b" << "\\bMA\\b"
                    << "\\bMC\\b" << "\\bMJ\\b" << "\\bMO\\b"
                    << "\\bMP\\b" << "\\bMR\\b"
                    << "\\bMRA\\b" << "\\bMS\\b" << "\\bMT\\b"
                    << "\\bMTS\\b" << "\\bNT\\b" << "\\bOG\\b"
                    << "\\bOVR\\b" << "\\bPA\\b" << "\\bPO\\b"
                    << "\\bPC\\b" << "\\bPD\\b" << "\\bPT\\b"
                    << "\\SD\\b" << "\\bPW\\b"<<"\\bSP\\b"
                    << "\\bTI\\b" << "\\bTL\\b" << "\\bTLM\\b"
                    << "\\bCL\\b" << "\\bCP\\b" << "\\bDA\\b"
                    << "\\bDL\\b" << "\\bEA\\b" << "\\bED\\b"
                    << "\\EQ\\b" << "\\bGS\\b" << "\\bGT\\b"
                    << "\\bHLT\\b" << "\\bLG\\b" << "\\bNE\\b"
                    << "\\bNW\\b" << "\\bNX\\b" << "\\bRC\\b"
                    << "\\bRN\\b" << "\\bRT\\b" << "\\bSM\\b"
                    << "\\bGC\\b" << "\\bGF\\b" << "\\bGO\\b"
                    << "\\bGP\\b" << "\\bID\\b" << "\\bOB\\b"
                    << "\\bOC\\b" << "\\bOD\\b" << "\\bTB\\b"
                    << "\\bTBD\\b" << "\\bADD\\b" << "\\bSUB\\b"
                    << "\\bMUL\\b" << "\\bDIV\\b" << "\\bIC\\b"
                    << "\\bDC\\b" << "\\bAN\\b" << "\\bOR\\b"
                    << "\\bXO\\b" << "\\bSC\\b" << "\\bPL\\b"
                    << "\\bSF\\b" << "\\bPX\\b" << "\\bCR\\b"
                    << "\\bDR\\b" << "\\bER\\b" << "\\bLR\\b"
                    << "\\bPR\\b" << "\\bQN\\b" << "\\bSTR\\b"
                    << "\\bVR\\b" << "\\bWH\\b" << "\\bWT\\b"
                    << "\\bWTM\\b" << "\\bINP\\b" << "\\bN\\b"
                    << "\\bOPN\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp(";[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);


    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
