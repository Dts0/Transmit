#include "tcpclient.h"
#include "widget.h"
#include <QCoreApplication>
#include <QTime>

TcpClient::TcpClient(QObject *parent) : BaseParent(parent)
{
    socket = new QTcpSocket(this);
    connect(socket,&QTcpSocket::readyRead,this,&TcpClient::handleReadyRead);
}

bool TcpClient::Open(Para *para)
{
    socket->connectToHost(para->ip, para->client_port);
    QTime time;
    time.start();
    while((time.elapsed() < 10000) && (socket->state()!=QAbstractSocket::ConnectedState)) {
        QCoreApplication::processEvents();
    }
    return (socket->state()==QAbstractSocket::ConnectedState);
}

void TcpClient::Close()
{
    socket->disconnectFromHost();
}

void TcpClient::handleReadyRead()
{
    Widget* wid = (Widget*)parent();
    QByteArray data = socket->readAll();
    QString    logString  = "";

    logString = "TCPC" + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
    logString += QString::asprintf(" read(%d):", data.size());
    for(int i = 0; i < data.size(); i++)
    {
        logString += QString::asprintf("%02X ", (unsigned char)data.at(i));
    }
    logString += "\n";
    wid->appendLog(logString);
    emit sig_recv_data(data);
}

void TcpClient::slot_send_data(QByteArray hex)
{
    Widget* wid = (Widget*)parent();
    if(socket->state() == QAbstractSocket::ConnectedState) {
        QString    logString  = "";
        logString = "TCPC" + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
        logString += QString::asprintf(" write(%d):", hex.size());
        for(int i = 0; i < hex.size(); i++)
        {
            logString += QString::asprintf("%02X ", (unsigned char)hex.at(i));
        }
        logString += "\n";
        wid->appendLog(logString);
        socket->write(hex);
        socket->flush();
    }
}
