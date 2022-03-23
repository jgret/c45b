#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QSerialPort>

class Worker : public QThread
{
private:
    QSerialPort *port;
    int state;
public:
    explicit Worker(QObject *parent = nullptr);
    Worker(QObject *parent, QSerialPort *port);
    void run() override;
signals:
    void changeState(int i);
};

#endif // WORKER_H
