#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QNetworkInterface>
#include "mytcpsever.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:  
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MyTcpSever *tcpSever;

signals:
    void SendData(int nType, QByteArray ba);

private slots:
    void Disp(QImage image, int imageTpye);
    void SendCurrentValues();
    void onRecvParamterData(QVector<int> vcParamterData);
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonLoad_clicked();
    void getHostInformation();
};

#endif // MAINWINDOW_H
