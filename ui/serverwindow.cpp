#include "serverwindow.h"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow),
    m_logic(new ServerLogic(this))
{
    ui->setupUi(this);

    // Connect signals from logic to UI
    connect(m_logic, &ServerLogic::logMessage, this, &ServerWindow::logMessage);
    connect(m_logic, &ServerLogic::clientCountChanged, this, &ServerWindow::updateClientCount);

    // Set default values
    ui->portSpinBox->setValue(12345);
    ui->stopButton->setEnabled(false);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::on_startButton_clicked()
{
    quint16 port = ui->portSpinBox->value();
    m_logic->startServer(port);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void ServerWindow::on_stopButton_clicked()
{
    m_logic->stopServer();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void ServerWindow::on_broadcastButton_clicked()
{
    QString message = ui->broadcastLineEdit->text();
    if(!message.isEmpty()) {
        m_logic->broadcastMessage(message);
        ui->broadcastLineEdit->clear();
    }
}

void ServerWindow::updateClientCount(int count)
{
    ui->clientCountLabel->setText(QString("Connected clients: %1").arg(count));
}

void ServerWindow::logMessage(const QString &message)
{
    ui->logTextEdit->append(message);
}
