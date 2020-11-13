#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTime"
#include "QRandomGenerator"
#include <QtGui>
#include "QFileDialog"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_2->setText(QString::number(QRandomGenerator::global()->bounded(1000, 9000)));
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(newtcpConnection()));
    server->listen(QHostAddress::LocalHost, ui->lineEdit_2->text().toInt());
    qDebug()<<"server open";
     m_nNextBlockSizeServer=0;
    connect(ui->lineEdit, SIGNAL(returnPressed()),SLOT(on_pushButton_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
      socketClient = new QTcpSocket(this);
      socketClient->connectToHost("localhost", ui->lineEdit_3->text().toInt());
      socketClient->waitForConnected();
      connect(socketClient, SIGNAL(disconnected()), SLOT(deleteLater()));
      m_nNextBlockSizeClient=0;
}

void MainWindow::newtcpConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), SLOT(deleteLater()));
    qDebug()<<"connected!";
}



void MainWindow::readyRead()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_5);

    for(;;){
        if (!m_nNextBlockSizeServer) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSizeServer;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSizeServer) {
            break;
        }
        QTime   time;
        QString type;
        QString name;
        QString message;
        in >> time >>type>>name;//>>message;
        //qDebug()<<"got:"<<time<<" "<<type<<" "<<message<<" "<<name;
        m_nNextBlockSizeServer = 0;

        if(type=="text"){
            QString message;
            in>>message;
            qDebug()<<"got:"<<time<<" "<<type<<" "<<message<<" "<<name;
            ui->textBrowser->append(name+"["+time.toString()+"]:"+message);
        }

        else if(type=="file") {
            QByteArray file;
            in>>file;
            qDebug()<<"got:"<<time<<" "<<type<<" "<<file<<" "<<name<<"isempty:"<<file.isEmpty();
            ui->textBrowser->append(name+"["+time.toString()+"]:file message");

            auto reply = QMessageBox::question(this,
                                   QString::fromUtf8("Вам прислали файл!"),
                                   QString("Пользователь "+name+" прислал вам файл! Хотите сохранить его где-то?"),QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes){
                QString fileName = QFileDialog::getSaveFileName(this,
                                            QString::fromUtf8("Сохранить файл"),
                                            QDir::currentPath(),
                                            "All files (*.*)");
                QFile newfile(fileName);
                newfile.open(QIODevice::WriteOnly);
                newfile.write(file);
                newfile.close();

                if(QMessageBox::question(this, QString::fromUtf8("Вы сохранили файл!"),
                                         QString("Хотите открыть файл, который только что сохранили?"),QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes){
                     QDesktopServices::openUrl(QUrl(fileName));
                }

            }

        }

    }


}

void MainWindow::on_pushButton_clicked()//послать текстовое сообщение
{
    if(socketClient->state() == QAbstractSocket::ConnectedState)
        {
        QByteArray  arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);

            out << quint16(0) << QTime::currentTime() <<QString("text")<<QString(ui->lineEdit_4->text())<<QString(ui->lineEdit->text());

            out.device()->seek(0);
            out << quint16(arrBlock.size() - sizeof(quint16));
            socketClient->write(arrBlock); //write the data itself
            socketClient->waitForBytesWritten();
            ui->textBrowser->append(ui->lineEdit_4->text()+"["+QTime::currentTime().toString()+"]:"+ui->lineEdit->text());
            ui->lineEdit->clear();

    }
}

void MainWindow::on_pushButton_3_clicked()//послать файл
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Послать файл"),
                                QDir::currentPath());
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray by;
    while (!file.atEnd()) {
       by.append(file.readAll());
    }

    if(socketClient->state() == QAbstractSocket::ConnectedState)
        {
        QByteArray  arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);


            out << quint16(0) << QTime::currentTime() <<QString("file")<<QString(ui->lineEdit_4->text())<<by;

            out.device()->seek(0);
            out << quint16(arrBlock.size() - sizeof(quint16));
            socketClient->write(arrBlock); //write the data itself
            socketClient->waitForBytesWritten();
            ui->textBrowser->append(ui->lineEdit_4->text()+"["+QTime::currentTime().toString()+"]:file message");
            ui->lineEdit->clear();

    }
    file.close();
}

void MainWindow::on_pushButton_4_clicked()
{

}
