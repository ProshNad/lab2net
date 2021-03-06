#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTcpServer"
#include "QTcpSocket"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pushButton_2_clicked();
    void newtcpConnection();

    void on_pushButton_clicked();
    void readyRead();
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *socket;
    QTcpSocket *socketClient;
    quint16     m_nNextBlockSizeServer;
    quint16     m_nNextBlockSizeClient;
};

#endif // MAINWINDOW_H
