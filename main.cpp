#include <QCoreApplication>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QTextStream>
#include "c45b.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    QString file;

#ifndef NOARGS
    if (argc < 2) {
        out << "to few arumgents" << Qt::endl;
        out << argv[0] << " <file>";
        return 1;
    } else {
        file = argv[1];
    }
#else
    file = "D:/megasync/htl/4_JG/dic/linux/build-c45b-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/Voltmeter.hex";
#endif

    C45B boot;
    if (!boot.open()) {
        return 1;
    }
    boot.setLogStream(&out);
    boot.connectToBootloader();
    boot.upload(file);
    return 0;
}
