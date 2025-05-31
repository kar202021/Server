#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>
#include <QDateTime>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

    static QString getIp(QTcpSocket *socket);
    
private slots:
    void newConnection();
    void readData(QTcpSocket *socket);
    void sendData();
    void clientDisconnected();
void handleSocketError(QAbstractSocket::SocketError error);
    void on_lineEditSendIP_textChanged(const QString &arg1);

private:
    Ui::ServerWindow *ui;
    QTcpServer *server;
//    QTcpSocket *clientSocket;
    QList<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> clientNames;

    QByteArray buffer;
    quint32 expectedSize = 0;
    QTcpSocket * sendingToSocket;
};

#endif // SERVERWINDOW_H
