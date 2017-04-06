#ifndef NUMBERER_H
#define NUMBERER_H

#include <QObject>
class QFile;

class Numberer : public QObject
{
    Q_OBJECT
public:
    explicit Numberer(QObject *parent = 0);
    number(QString fileName);
signals:

public slots:
};

#endif // NUMBERER_H
