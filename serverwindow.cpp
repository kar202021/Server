#include "serverwindow.h"
#include "ui_serverwindow.h"


ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &ServerWindow::newConnection);

    QString ip = ui->lineEditListenIP->text();  // Get IP from UI
    int port = ui->spinBoxListenPort->value();  // Get port from UI

    bool connectionStablished;
    if(ip=="Any"){
        connectionStablished = server->listen(QHostAddress::Any, port);
    }else{
        connectionStablished = server->listen(QHostAddress(ip), port);
    }

    if(connectionStablished){
        ui->textEdit->append("Server started on IP: " + ip +" port " + QString::number(port));
    } else {
        ui->textEdit->append("Server failed to start");
    }
}

ServerWindow::~ServerWindow()
{
    delete ui;
}
void ServerWindow::newConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();
    clients.append(socket);
    if(clients.size()==1) sendingToSocket = socket;
    QString clientId = getIp(socket); // QString("Client-%1").arg(clients.size());
    ui->listOfClients->append((QString::number(clients.size())) + ": " + getIp(socket));
    clientNames.insert(socket, clientId);
//    connect(socket, &QTcpSocket::readyRead, this, &ServerWindow::readData);
    connect(socket, &QTcpSocket::readyRead, [this, socket]() {
readData(socket);
    });
    connect(socket, &QTcpSocket::disconnected, this, &ServerWindow::clientDisconnected);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    ui->textEdit->append("New client connected <" + clientNames[socket] + "> ");
}
void ServerWindow::handleSocketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);  // Avoid unused parameter warning
    ui->textEdit->append("Socket Error: " + sendingToSocket->errorString());
}
QString ServerWindow::getIp(QTcpSocket *socket)
{
    QString output;
    QHostAddress address = socket->peerAddress();
    if (address.protocol() == QAbstractSocket::IPv6Protocol && address.toString().startsWith("::ffff:")) {
        QHostAddress ipv4Address;
        ipv4Address.setAddress(address.toIPv4Address());
        output = ipv4Address.toString();
    } else {
        output = "IPv4 Address:" + address.toString();
    }
    return output;
}

void ServerWindow::readData(QTcpSocket *socket) {
    buffer += socket->readAll();

    while (true) {
        // 1. Wait until we have at least 4 bytes (header)
        if (expectedSize == 0 && buffer.size() >= 4) {
//            expectedSize = qFromBigEndian<quint32>(buffer.mid(0, 4).constData());
            expectedSize = qFromBigEndian(*reinterpret_cast<const quint32*>(buffer.mid(0, 4).constData()));
            buffer.remove(0, 4);  // Remove header
        }

        // 2. Wait until full message is received
        if (expectedSize > 0 && static_cast<quint32>(buffer.size()) >= expectedSize) {
            QByteArray jsonData = buffer.mid(0, expectedSize);
            buffer.remove(0, expectedSize);
            expectedSize = 0;  // Reset for next message

            // Process JSON
            QJsonDocument doc = QJsonDocument::fromJson(jsonData);
            if (!doc.isNull()) {
                QJsonObject obj = doc.object();
                ui->textEdit->append(getIp(socket) + ": " + obj["message"].toString());
            } else {
                ui->textEdit->append("Invalid JSON received!");
            }
        } else {
            break;  // Wait for more data
        }
    }
}
void ServerWindow::sendData(){
    if (sendingToSocket->state() == QAbstractSocket::ConnectedState){
        QJsonObject obj;
        obj["message"] = ui->lineEdit->text();
        obj["timestamp"] = QDateTime::currentDateTime().toString();

        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        // Prepend 4-byte message length (big-endian)
        QByteArray header;
        header.resize(4);
        qToBigEndian<quint32>(data.size(), header.data());

        sendingToSocket->write(header + data);  // Send [HEADER][JSON]
        ui->textEdit->append("Sent: " + ui->lineEdit->text());
    }
}
void ServerWindow::clientDisconnected()
{
    ui->textEdit->append("Client disconnected");
    sendingToSocket->deleteLater();
}
void ServerWindow::on_lineEditSendIP_textChanged(const QString &arg1)
{
    if(clients.size()<=1) {
        std::cerr << ">>> ServerWindow::sendData>>> if(clients.size()<=1) {" << std::endl;
        return;
    }
    sendingToSocket = clients.at(0);
    foreach(auto client, clients){
        if(getIp(client) == ui->lineEditSendIP->text()){
            sendingToSocket = client;
            break;
    }
    }
}

