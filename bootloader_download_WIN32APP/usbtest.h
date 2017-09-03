#ifndef USBTEST_H
#define USBTEST_H

#include <QMainWindow>
#include "lusb0_usb.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include "SerialThread.h"

namespace Ui {
class USBTest;
}

class USBTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit USBTest(QWidget *parent = 0);
    ~USBTest();

private slots:
    void on_connectDevButton_clicked();
    void timeoutUiTimer();
    void serialInfoRecv(QString str);

    void on_checkConn_clicked();

private:
    Ui::USBTest *ui;
    SerialThread *m_serialThread;
};

#endif // USBTEST_H
