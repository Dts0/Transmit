#include "serialport.h"
#include "widget.h"
#include <QIODevice>

SerialPort::SerialPort(QObject *parent) : BaseParent(parent)
{
    serial = new QSerialPort(this);
    connect(serial,&QSerialPort::readyRead,this,&SerialPort::handleReadyRead);
}

bool SerialPort::Open(Para *para)
{
    if(serial->isOpen())
        serial->close();
    serial->setPortName(QString("COM%1").arg(para->com));
    if(!serial->open(QIODevice::ReadWrite))
        return false;
    serial->setBaudRate(para->baud);
    serial->setParity(para->parity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    return true;
}

void SerialPort::Close()
{
    serial->close();
}

void SerialPort::handleReadyRead()
{
    Widget* wid = (Widget*)parent();
    QByteArray data = serial->readAll();
    QString    logString  = "";

    logString = serial->portName();
    logString += QString::asprintf(" read(%d):", data.size());
    for(int i = 0; i < data.size(); i++)
    {
        logString += QString::asprintf("%02X ", (unsigned char)data.at(i));
    }
    logString += "\n";
    wid->appendLog(logString);
    emit sig_recv_data(data);
}

void SerialPort::slot_send_data(QByteArray hex)
{
    Widget* wid = (Widget*)parent();
    if(serial->isOpen() && serial->isWritable()) {
        QString    logString  = "";
        logString = serial->portName();
        logString += QString::asprintf(" write(%d):", hex.size());
        for(int i = 0; i < hex.size(); i++)
        {
            logString += QString::asprintf("%02X ", (unsigned char)hex.at(i));
        }
        logString += "\n";
        wid->appendLog(logString);
        serial->write(hex);
        serial->flush();
    }
}
