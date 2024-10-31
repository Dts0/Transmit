#include "tcpserver.h"
#include <widget.h>

TcpServer::TcpServer(QObject *parent) : BaseParent(parent)
{
    server = new QTcpServer(this);
    connect(server,&QTcpServer::newConnection,this,&TcpServer::handleNewConnection);
}

bool TcpServer::Open(Para *para)
{
    return server->listen(QHostAddress::AnyIPv4,para->server_port);
}

void TcpServer::Close()
{
    if(socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
        socket = NULL;
    }
    server->close();
}

//void TcpServer::incomingConnection(qintptr handle)
//{
//    if (!socket) {
//        socket = new QTcpSocket(this);
//    } else {
//        disconnect(socket,&QTcpSocket::readyRead,this,&TcpServer::handleReadyRead);
//        socket->disconnectFromHost();
//    }
//    socket->setSocketDescriptor(handle);
//    connect(socket,&QTcpSocket::readyRead,this,&TcpServer::handleReadyRead);
//}

void TcpServer::handleReadyRead()
{
    Widget* wid = (Widget*)parent();
    QByteArray data = socket->readAll();
    QString    logString  = "";

    logString = "TCPS" + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
    logString += QString::asprintf(" read(%d):", data.size());
    for(int i = 0; i < data.size(); i++)
    {
        logString += QString::asprintf("%02X ", (unsigned char)data.at(i));
    }
    logString += "\n";
    wid->appendLog(logString);
    emit sig_recv_data(data);
}

void TcpServer::handleNewConnection()
{
    Widget* wid = (Widget*)parent();
    QString    logString  = "";
    socket = server->nextPendingConnection();
    connect(socket,&QTcpSocket::readyRead,this,&TcpServer::handleReadyRead);

    logString = "TCPS new connected: " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
    logString += "\n";
    wid->appendLog(logString);
}

void TcpServer::slot_send_data(QByteArray hex)
{
    Widget* wid = (Widget*)parent();
    if(socket && socket->isWritable()) {
        QString    logString  = "";
        logString = "TCPS" + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());
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
