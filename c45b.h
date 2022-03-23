#ifndef C45B_H
#define C45B_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QTextStream>

class C45B : public QObject
{
    Q_OBJECT

private:
    QSerialPort *port;
    QTextStream *out;
    QTimer *timer;

public:
    explicit C45B(QObject *parent = nullptr);
    ~C45B();
    void connectToBootloader();
    void upload(QString file);
    bool open();
    void log(QString s);
    void setLogStream(QTextStream *o);

public slots:

};

#endif // C45B_H
