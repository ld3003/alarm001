#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class SerialThread : public QThread
{
    Q_OBJECT
public:
    SerialThread(QObject *parent = 0);
    QString m_fwFilePath;

protected:
    virtual void run();
    void burnFW();
    void loadFW();
    int getFWSeq(unsigned char *buf , int length);

    int m_fwWriteLength;
    int m_fwTotalLength;
    QByteArray m_fwData;


signals:
    void serialInfoStr(QString str);

public:
    enum {
        __INIT,
        __INIT2,
        __CHECK_DEV,
        __BURNERR,
        __BURNHEX,
        __BURNSUCCESS,
    };
    QSerialPort *serialport;
    int m_burnState;
};

#endif // SERIALTHREAD_H
