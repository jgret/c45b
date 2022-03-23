#include "c45b.h"

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QThread>
#include <QFile>
#include <QTextStream>

C45B::C45B(QObject *parent)
    : QObject{parent}
{
    this->port = new QSerialPort;
    this->out = NULL;
}

C45B::~C45B() {
    this->port->close();
    delete this->port;
}

bool C45B::open(QString portname, int baud) {

    QSerialPortInfo device;
    if (portname == "null") {
        QList<QSerialPortInfo> portinfo = QSerialPortInfo::availablePorts();
        if (portinfo.length() == 0) {
            qDebug() << "No devices found";
            return false;
        }
        device = portinfo[0];
    } else {
        device = QSerialPortInfo(portname);
    }

    this->port->setBaudRate(baud);
    this->port->setFlowControl(QSerialPort::SoftwareControl);
    this->port->setParity(QSerialPort::NoParity);
    this->port->setDataBits(QSerialPort::Data8);
    this->port->setStopBits(QSerialPort::OneStop);
    this->port->setPort(device);
    if (this->port->open(QIODevice::ReadWrite)) {
        this->log("Connected");
        return true;
    } else {
        this->log("Couldn't open device");
        return false;
    }

}

void C45B::connectToBootloader() {
    this->log("Wait for Reset MCU\n");
    QThread::msleep(100); // for dtr
    this->port->setDataTerminalReady(true);

    bool connected = false;
    QString version = "c45b2 v2.9N";

    do {

        port->putChar('U');
        port->putChar('U');
        port->putChar('U');
        port->putChar('U');
        port->putChar('\n');
        port->flush();
        this->port->waitForReadyRead(10);

        if (this->port->bytesAvailable()) {
            QByteArray ba;
            ba = this->port->readAll();

            QString resp = ba;
            if (resp.contains(version)) {
                connected = true;
                qDebug() << ba << ": connected";
            }
        }

        port->waitForReadyRead(100);

    } while (!connected);

    QByteArray resp = this->port->readLine();
    this->log(QString(resp));
}

void C45B::upload(QString file) {
    this->port->putChar('p');
    this->port->putChar('f');
    this->port->putChar('\r');
    this->port->putChar('\n');
    this->port->flush();

    this->port->waitForReadyRead(100);
    QByteArray resp = this->port->readAll();
    QString cmd(resp);
    int indexf = resp.indexOf('f');
    if (resp.at(indexf+1) != '+') {
        this->log(QString(resp));
        this->log("Error response code for flash command\n");
    }

    QFile hexfile(file);
    if (hexfile.open(QIODevice::ReadOnly)) {
        QTextStream in(&hexfile);
        QString line;
        QByteArray ba;
        int count = 1;

        while (in.readLineInto(&line)) {
            QString page = line.append("\r\n");
            qDebug() << page;
            this->port->write(page.toLocal8Bit());
            this->port->flush();

            this->port->waitForReadyRead(5000);
            ba = this->port->readAll(); // XOFF data XON
            if (ba.at(0) != '.') { // 1 on windows?
                log(QString("Unexpected response ").append(ba.at(0)) + "\n");
                return;
            }

            if (count % 16 == 0) {
                this->port->waitForReadyRead(5000);
                count = 0;
                ba = this->port->readAll();
                if (ba.at(0) != '*') { // no OFF
                    log(QString("Unexpected response ").append(ba.at(0)) + "\n");
                    return;
                }
            }
            count++;
        }

        if (this->port->waitForReadyRead(1000)) {
            ba = this->port->readAll();
            if (ba.at(1) == '+') {
                log("Flashed Successful\n");
            }
        }
    } else {
        this->log("file not found");
    }

    this->port->putChar('g');
    this->port->putChar('\r');
    this->port->putChar('\n');
    this->port->flush();

    if (this->port->waitForReadyRead(1000)) {
        qDebug() << this->port->readAll();
    }
}

void C45B::log(QString s) {
    if (this->out != NULL) {
        *this->out<< s;
        this->out->flush();
    }
}

void C45B::setLogStream(QTextStream *o)
{
    this->out = o;
}
