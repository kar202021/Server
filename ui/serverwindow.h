#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include "business/serverlogic.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_broadcastButton_clicked();
    void updateClientCount(int count);
    void logMessage(const QString &message);

private:
    Ui::ServerWindow *ui;
    ServerLogic *m_logic;
};

#endif // SERVERWINDOW_H
