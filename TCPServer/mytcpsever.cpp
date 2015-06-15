#include "mytcpsever.h"
#include <QDebug>

MyTcpSever::MyTcpSever(QObject *parent) :
    QTcpServer(parent)
{
    this->listen(QHostAddress::Any, 6000);    
}

void MyTcpSever::CloseTcp()
{
    for(int i=0;i<m_vcTcpSock.size();i++)
        {
        m_vcTcpSock[i]->close();
    }
    this->close();
}

void MyTcpSever::incomingConnection(qintptr socketDescriptor)
{
    MyTcpSocket *myTcpSock = new MyTcpSocket(this);
    if (myTcpSock->setSocketDescriptor(socketDescriptor))
    {
        connect(myTcpSock, SIGNAL(GetImage(QImage,int)), this, SIGNAL(GetImage(QImage,int)));
        connect(myTcpSock,SIGNAL(GetParameterData(QVector<int>)), this, SIGNAL(GetParameterData(QVector<int>)));
        connect(this, SIGNAL(SendData(int, QByteArray)), myTcpSock, SLOT(onSendData(int, QByteArray)));
        m_vcTcpSock.push_back(myTcpSock);
        qDebug()<<myTcpSock->state()<<socketDescriptor<<myTcpSock->peerAddress();
    }
    else
    {
        qDebug()<<"TcpSocket Fail...";
    }
}
