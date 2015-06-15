#ifndef MYTCPSEVER_H
#define MYTCPSEVER_H

#include <QTcpServer>
#include <QImage>
#include <QVector>
#include "mytcpsocket.h"

class MyTcpSever : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpSever(QObject *parent = 0);
    void CloseTcp();

private:
    QVector<MyTcpSocket*> m_vcTcpSock;

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    void GetImage(QImage image, int imageType);
    void SendData(int nType, QByteArray ba);
    void GetParameterData(QVector<int> vcParamterData);

public slots:

};

#endif // MYTCPSEVER_H
