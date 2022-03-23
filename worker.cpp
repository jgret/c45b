#include "worker.h"
#include <QDebug>

Worker::Worker(QObject *parent)
    : QThread{parent}
{

}

Worker::Worker(QObject *parent, QSerialPort *port) : QThread{parent} {
    this->port = port;
    this->state = 0;
}

void Worker::changeState(int i) {
    qDebug() << "State changed to " << i;
    this->state = i;
}

void Worker::run() {
    while (1) {
        switch (this->state)
        {
            case (0):
                break;
            case (1):
                qDebug() << "Write U";
                this->port->write("U\r\n");
                QThread::sleep(100);
                break;
        }
    }
}
