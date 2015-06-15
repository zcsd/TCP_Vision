#include "mytcpsocket.h"
#include <QHostAddress>
#include <QImage>
#include <QBuffer>
#include <QImageReader>

typedef struct TAG_MSG_PACK
{
    int nType;// 0 parameters, 1 save, 2 Load
    int nLen;
    char data[];
}MSG_PACK;

MyTcpSocket::MyTcpSocket(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(RcvData()));

    dataSize = 0;
    imageType = -1;
}

void MyTcpSocket::RcvData()
{
        QDataStream stream(this);

        if( this->bytesAvailable() < sizeof(quint32) )
            return;

        while(true)
        {
            if(dataSize > this->bytesAvailable() )
                return;

            if( this->bytesAvailable() < sizeof(quint32) )
                return;

            if(imageType==-1)
            {
                stream>>imageType;
                qDebug()<<"imageType: "<<imageType;
            }

            if( this->bytesAvailable() < sizeof(quint32) )
                return;

            if(dataSize == 0)
            {
                stream >> dataSize; //size of received data
                qDebug()<<"dataSize:"<<dataSize;
            }

            if( dataSize > this->bytesAvailable() || dataSize == 0)
                {
                return;
                }

            if(imageType==1)//frame,rgb
            {
                memset(image1Buf, 0, 76800*3);
                this->read(image1Buf, dataSize);
                image=QImage((unsigned char*)image1Buf, 320, 240, 960, QImage::Format_RGB888);
            }else if(imageType==0) //imgThresh,gray
            {
                memset(image2Buf, 0, 76800);
                this->read(image2Buf, dataSize);
                image=QImage((unsigned char*)image2Buf, 320, 240, 320, QImage::Format_Indexed8);
            }else if(imageType == 3)
                {
                memset(ParaBuf,0,1024);
                this->read(ParaBuf, dataSize);
                m_vcParamterData<<(int)ParaBuf[0]<<(int)ParaBuf[1]<<(int)ParaBuf[2]<<(int)ParaBuf[3]<<(int)ParaBuf[4]<<(int)ParaBuf[5];
                emit GetParameterData(m_vcParamterData);
                QString str = tr("Recv ImageType = 3, %1,%2,%3").arg(imageType).arg((int)ParaBuf[0]).arg((int)ParaBuf[5]);
                qDebug()<<str;
            }

            if( !image.isNull() )
            {
                qDebug()<<"--------------------GetImage-------------------";
                emit GetImage(image, imageType);
            }
            else
            {
                qDebug()<<" Image Is Null";
            }
            imageType = -1;
            dataSize = 0;
        }
}

int MyTcpSocket::onSendData(int nType, QByteArray ba)
{
    int nRes = 0;
    int nLen = ba.size();
    MSG_PACK *msgPack;
    char *pSZMsgPack = (char*)malloc(sizeof(MSG_PACK)+nLen);
    msgPack = (MSG_PACK *)pSZMsgPack;
    msgPack->nType = nType;
    msgPack->nLen = nLen;
    memcpy(pSZMsgPack+sizeof(MSG_PACK), ba.data(), nLen);
    nRes = write(pSZMsgPack, sizeof(MSG_PACK)+nLen);
    printf("Write DataBuf: %d\n",nRes);
    free(pSZMsgPack);
    if(nRes == nLen+sizeof(MSG_PACK))
    {
        return nRes;
    }
//    this->write(ba);
}
