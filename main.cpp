#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
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
    QString appname = "c45b";
    QString version = "v1.0";

    QCoreApplication::setApplicationName(appname);
    QCoreApplication::setApplicationVersion(version);

    QCommandLineParser parser;
    parser.setApplicationDescription("Help");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "input hexfile"));
    parser.addOptions({
                          {{"p", "port"},
                           QCoreApplication::translate("main", "device port"), QCoreApplication::translate("main", "port")},
                          {{"b", "baud"},
                           QCoreApplication::translate("main", "Overwrite existing files."), QCoreApplication::translate("main", "baud")},
                      });
    parser.process(app);

    QString port;
    int baud;
    if (parser.isSet("port")) {
        port = parser.value("port");
    } else {
        port = "null";
    }

    if (parser.isSet("baud")) {
        baud = parser.value("baud").toInt();
    } else {
        baud = 19200;
    }


    qDebug() << "port: " << parser.value("port");
    qDebug() << "baud: " << parser.value("baud");
    return 0;

#ifndef DEBUG
    if (argc < 2) {
        out << "c45b cli " << version << Qt::endl;
        out << "to few arumgents" << Qt::endl;
        out << "usage:" << argv[0] << " <file>" << Qt::endl;
        return 1;
    } else if (QString(argv[1]).contains("-v")) {
        out << version << Qt::endl;
        return 0;
    } else {
        file = argv[1];
    }
#else
    // file = "D:/megasync/htl/4_JG/dic/linux/build-c45b-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/Voltmeter.hex";
    file = "/home/gret/htl/4_JG/dic/atmel-ws/SOSBlink/SOSBlink/Release/SOSBlink.hex";
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
