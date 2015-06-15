#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpSever = NULL;

    connect(ui->horizontalSliderHeuMin, SIGNAL(valueChanged(int)), ui->spinBoxHeuMin, SLOT(setValue(int)));
    connect(ui->horizontalSliderHeuMin, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->spinBoxHeuMin, SIGNAL(valueChanged(int)), ui->horizontalSliderHeuMin, SLOT(setValue(int)));
    connect(ui->spinBoxHeuMin, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->horizontalSliderHeuMax, SIGNAL(valueChanged(int)), ui->spinBoxHeuMax, SLOT(setValue(int)));
    connect(ui->horizontalSliderHeuMax, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->spinBoxHeuMax, SIGNAL(valueChanged(int)), ui->horizontalSliderHeuMax, SLOT(setValue(int)));
    connect(ui->spinBoxHeuMax, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->horizontalSliderSatMin, SIGNAL(valueChanged(int)), ui->spinBoxSatMin, SLOT(setValue(int)));
    connect(ui->horizontalSliderSatMin, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->spinBoxSatMin, SIGNAL(valueChanged(int)), ui->horizontalSliderSatMin, SLOT(setValue(int)));
    connect(ui->spinBoxSatMin, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->horizontalSliderSatMax, SIGNAL(valueChanged(int)), ui->spinBoxSatMax, SLOT(setValue(int)));
    connect(ui->horizontalSliderSatMax, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->spinBoxSatMax, SIGNAL(valueChanged(int)), ui->horizontalSliderSatMax, SLOT(setValue(int)));
    connect(ui->spinBoxSatMax, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->horizontalSliderLumMin, SIGNAL(valueChanged(int)), ui->spinBoxLumMin, SLOT(setValue(int)));
    connect(ui->horizontalSliderLumMin, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->spinBoxLumMin, SIGNAL(valueChanged(int)), ui->horizontalSliderLumMin, SLOT(setValue(int)));
    connect(ui->spinBoxLumMin, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->horizontalSliderLumMax, SIGNAL(valueChanged(int)), ui->spinBoxLumMax, SLOT(setValue(int)));
    connect(ui->horizontalSliderLumMax, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));
    connect(ui->spinBoxLumMax, SIGNAL(valueChanged(int)), ui->horizontalSliderLumMax, SLOT(setValue(int)));
    connect(ui->spinBoxLumMax, SIGNAL(valueChanged(int)), this, SLOT(SendCurrentValues()));

    ui->spinBoxHeuMin->setValue(0);
    ui->spinBoxHeuMax->setValue(255);
    ui->spinBoxSatMin->setValue(0);
    ui->spinBoxSatMax->setValue(255);
    ui->spinBoxLumMin->setValue(0);
    ui->spinBoxLumMax->setValue(255);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SendCurrentValues()
{
    QByteArray data;

    data.resize(6);
    data[0] = ui->spinBoxHeuMin->value();
    data[1] = ui->spinBoxHeuMax->value();
    data[2] = ui->spinBoxSatMin->value();
    data[3] = ui->spinBoxSatMax->value();
    data[4] = ui->spinBoxLumMin->value();
    data[5] = ui->spinBoxLumMax->value();
    emit SendData(0, data);
}

void MainWindow::onRecvParamterData(QVector<int> vcParamterData)
{
    ui->spinBoxHeuMin->setValue(vcParamterData[0]);
    ui->spinBoxHeuMax->setValue(vcParamterData[1]);
    ui->spinBoxSatMin->setValue(vcParamterData[2]);
    ui->spinBoxSatMax->setValue(vcParamterData[3]);
    ui->spinBoxLumMin->setValue(vcParamterData[4]);
    ui->spinBoxLumMax->setValue(vcParamterData[5]);
}

void MainWindow::Disp(QImage image, int imageTpye)
{
    int tempWidth = ui->labVideo1->geometry().width();
    int tempHeight = ui->labVideo1->geometry().height();

    if (image.height() > 0)
    {
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth, tempHeight));

        if(imageTpye == 1)
        {
            ui->labVideo1->setPixmap(pix);
        }
        else if(imageTpye == 0)
        {
            ui->labVideo2->setPixmap(pix);
        }
    }
}

void MainWindow::on_pushButtonStart_clicked()
{
    if(tcpSever == NULL)
    {
        tcpSever = new MyTcpSever(this);
        tcpSever->listen(QHostAddress::Any, 6000);
        connect(tcpSever, SIGNAL(GetImage(QImage,int)), this, SLOT(Disp(QImage,int)));
        connect(this, SIGNAL(SendData(int, QByteArray)), tcpSever, SIGNAL(SendData(int, QByteArray)));
        connect(tcpSever, SIGNAL(GetParameterData(QVector<int>)), this, SLOT(onRecvParamterData(QVector<int>)));
        getHostInformation();
    }
}

void MainWindow::on_pushButtonStop_clicked()
{
    if(tcpSever != NULL)
        {
            tcpSever->CloseTcp();
    }
    ui->labVideo1->clear();
    ui->labVideo2->clear();
}

void MainWindow::on_pushButtonSave_clicked()
{
    QByteArray data;
    data.resize(0);
    emit SendData(1, data);
}

void MainWindow::on_pushButtonLoad_clicked()
{
    QByteArray data;
    data.resize(0);
    emit SendData(2, data);
}

void MainWindow::getHostInformation()
{
    QList<QNetworkInterface> list=QNetworkInterface::allInterfaces();
    QNetworkInterface interface=list.at(0);
    QList<QNetworkAddressEntry> entryList=interface.addressEntries();
    QNetworkAddressEntry entry=entryList.at(1);

    ui->textEdit->setText("Server started successfully, Host IP: " + entry.ip().toString() +", Port: 6000");
}
