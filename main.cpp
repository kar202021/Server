#include "ui/serverwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set application style and palette for consistent look
    QApplication::setStyle("Fusion");

    ServerWindow w;
    w.show();

    return a.exec();
}
