#include "usbtest.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    USBTest w;
    w.show();

    return a.exec();
}
