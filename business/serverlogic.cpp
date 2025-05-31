#include "serverlogic.h"
#include <QDateTime>

ServerLogic::ServerLogic(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &ServerLogic::handleNewConnection);
}

ServerLogic::~ServerLogic()
{
    stopServer();
}

void ServerLogic::startServer(quint16 port)
{
    if(!m_server->listen(QHostAddress::Any, port)) {
        emit logMessage("Server failed to start: " + m_server->errorString());
        return;
    }
    emit logMessage(QString("Server started on port %1").arg(port));
}

void ServerLogic::stopServer()
{
    foreach(QTcpSocket *client, m_clients) {
        client->disconnectFromHost();
    }
    m_server->close();
    emit logMessage("Server stopped");
    emit clientCountChanged(0);
}

void ServerLogic::broadcastMessage(const QString &message)
{
    QJsonObject jsonObj;
    jsonObj["message"] = message;
    jsonObj["timestamp"] = QDateTime::currentDateTime().toString();
    jsonObj["sender"] = "Server";

    QByteArray data = QJsonDocument(jsonObj).toJson();

    foreach(QTcpSocket *client, m_clients) {
        client->write(data);
    }
}

void ServerLogic::handleNewConnection()
{
    QTcpSocket *client = m_server->nextPendingConnection();
    m_clients.append(client);
    m_clientIdentifiers[client] = generateClientId(client);

    connect(client, &QTcpSocket::readyRead, this, &ServerLogic::readClientData);
    connect(client, &QTcpSocket::disconnected, this, &ServerLogic::handleClientDisconnected);

    emit logMessage(m_clientIdentifiers[client] + " connected");
    emit clientCountChanged(m_clients.size());
}

void ServerLogic::handleClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(!client) return;

    emit logMessage(m_clientIdentifiers[client] + " disconnected");
    m_clients.removeOne(client);
    m_clientIdentifiers.remove(client);
    client->deleteLater();

    emit clientCountChanged(m_clients.size());
}

void ServerLogic::readClientData()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if(!client) return;

    while(client->bytesAvailable() > 0) {
        QByteArray data = client->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(doc.isNull()) {
            emit logMessage("Invalid JSON from " + m_clientIdentifiers[client]);
            continue;
        }

        QJsonObject obj = doc.object();
        QString msg = QString("[%1] %2").arg(m_clientIdentifiers[client], obj["message"].toString());
        emit logMessage(msg);
    }
}

QString ServerLogic::generateClientId(QTcpSocket *client)
{
    return QString("Client-%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
}
