#ifndef SERVERLOGIC_H
#define SERVERLOGIC_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>

class ServerLogic : public QObject
{
    Q_OBJECT
public:
    explicit ServerLogic(QObject *parent = nullptr);
    ~ServerLogic();

    void startServer(quint16 port);
    void stopServer();
    void broadcastMessage(const QString &message);

signals:
    void logMessage(const QString &message);
    void clientCountChanged(int count);

private slots:
    void handleNewConnection();
    void handleClientDisconnected();
    void readClientData();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    QMap<QTcpSocket*, QString> m_clientIdentifiers;

    QString generateClientId(QTcpSocket *client);
};

#endif // SERVERLOGIC_H
