#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QImage>
#include <QVector>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = 0);

signals:
    void GetImage(QImage image, int imageType);
    void GetParameterData(QVector<int> vcParamterData);

public slots:
    void RcvData();
    int onSendData(int nType, QByteArray ba);

private:
    QImage image;
    char image2Buf[76800];
    char image1Buf[76800*3];
    char ParaBuf[1024];
    int dataSize;
    int imageType;
    QVector<int> m_vcParamterData;

};

#endif // MYTCPSOCKET_H
