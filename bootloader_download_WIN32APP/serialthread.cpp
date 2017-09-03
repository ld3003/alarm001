#include "serialthread.h"
#include <QFile>

#define RUNTEST; //qDebug() << "RUN_TEST" <<__LINE__ << __FILE__;

static int conv_hex_2_string(unsigned char *data , int datalen , char *str)
{
    int i=0;
    char tmp[4];
    memset(str,0x0,datalen*2+4);
    //debug_buf("xx111",data,datalen);
    for(i=0;i<datalen;i++)
    {
        snprintf(tmp,sizeof(tmp),"%02X",data[i]);
        strcat(str,tmp);
        //printf("[%s]",tmp);
    }

    return datalen*2+1;
}


SerialThread::SerialThread(QObject *parent):QThread(parent)
{
    m_burnState = SerialThread::__INIT;
}

void SerialThread::run()
{

    QByteArray tmpArray;

#if 1

    serialport = new QSerialPort(0);

    //init
    //m_burnState = SerialThread::__INIT;

    //QSerialPortInfo com_info;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "串口序号:" << info.serialNumber() << "串口名:" << info.portName();


        serialport->setPort(info);

        if (serialport->open(QIODevice::ReadWrite))
        {
            serialport->setBaudRate(QSerialPort::Baud115200);
            serialport->setParity(QSerialPort::NoParity);
            serialport->setDataBits(QSerialPort::Data8);
            serialport->setStopBits(QSerialPort::OneStop);
            serialport->setFlowControl(QSerialPort::NoFlowControl);

            serialport->clearError();
            serialport->clear();

            //发送测试命令
            serialport->write(QByteArray("+1234567890123\r\n"));

            //等待1s看下返回结果是否正确
            if (serialport->waitForReadyRead(1000))
            {
                tmpArray = serialport->readAll();
                qDebug() << "tmpArray" << tmpArray;

                if (strstr(tmpArray.data(),"OK") == tmpArray.data())
                {
                    serialInfoStr("成功发现设备!");
                    burnFW();



                }
            }

            serialport->close();
        }else{
            serialInfoStr("打开串口失败!");
        }
    }

    serialport->deleteLater();

#endif
    serialInfoStr("操作结束,释放线程资源!");



}

void SerialThread::burnFW()
{
    for(;;)
    {
        switch(m_burnState)
        {
        case SerialThread::__CHECK_DEV:
            return;
            break;
        case SerialThread::__INIT:
        {
            QByteArray serialRcvArray;
            //加载固件文件
            loadFW();

            serialport->write(QByteArray("+STARTBURN\r\n"));
            serialport->waitForReadyRead(1000);
            serialRcvArray = serialport->readAll();
            if (strstr(serialRcvArray.data(),"OK") == serialRcvArray.data())
            {
                serialInfoStr("开始烧录固件!");
                m_burnState = SerialThread::__BURNHEX;
            }else{

                serialInfoStr("开始烧录固件失败!");
                m_burnState = SerialThread::__BURNERR;
            }
            break;
        }
        case SerialThread::__INIT2:
        {
            QByteArray serialRcvArray;
            serialport->write(QByteArray("+ENDBURN\r\n"));
            serialport->waitForReadyRead(1000);
            serialRcvArray = serialport->readAll();
            if (strstr(serialRcvArray.data(),"OK") == serialRcvArray.data())
            {
                serialInfoStr("结束烧录!");
            }else{

                serialInfoStr("结束烧录失败!");
            }

            serialport->write(QByteArray("+APP\r\n"));
            serialport->waitForReadyRead(1000);
            serialRcvArray = serialport->readAll();
            if (strstr(serialRcvArray.data(),"OK") == serialRcvArray.data())
            {
                serialInfoStr("运行固件!");
            }else{

                serialInfoStr("运行固件失败!");
            }
            return;
            break;
        }
        case SerialThread::__BURNHEX:
        {
            int length;
            unsigned char tmpbuf[512];
            QByteArray serialRcvArray;

            length = getFWSeq(tmpbuf,32+16);

            if (length == 0)
            {
                serialInfoStr("写入结束!");
                m_burnState = SerialThread::__BURNSUCCESS;
            }else{
                char tmpbufConvStr[512];
                conv_hex_2_string(tmpbuf,length,tmpbufConvStr);

                qDebug() << "tmpbufConvStr" << tmpbufConvStr;

                serialport->write(QByteArray("+BURNHEX="));
                serialport->write(QByteArray(tmpbufConvStr,strlen(tmpbufConvStr)));

                serialport->write(QByteArray("\r\n"));
                for(int ii=0;ii<10;ii++)
                {
                    if (serialport->waitForReadyRead(1000) == true)
                    {
                        serialRcvArray += serialport->readAll();
                        qDebug() << "__BURNHEX ACK " << serialRcvArray;
                        if (strstr(serialRcvArray.data(),"OK") > 0)
                        {
                            QString str;
                            str = "已经写入 %" + QString::number(int(((float)m_fwWriteLength/(float)m_fwTotalLength)*100));
                            serialInfoStr(str);
                            m_burnState = SerialThread::__BURNHEX;
                            break;
                        }else{

                            if (ii >= 5)
                            {
                                serialInfoStr("写入失败失败!");
                                m_burnState = SerialThread::__BURNERR;
                                break;
                            }
                        }
                    }
                }
                serialRcvArray.clear();
            }
            break;
        }
        case SerialThread::__BURNERR:
            serialInfoStr("烧录失败!");
            return;
            break;
        case SerialThread::__BURNSUCCESS:


            serialInfoStr("烧录成功!");
            m_burnState = SerialThread::__INIT2;
            break;
        default:
            break;

        }
    }
    //
}

void SerialThread::loadFW()
{
    QFile f(m_fwFilePath);
    if(!f.open(QIODevice::ReadOnly))
    {
        serialInfoStr("固件文件打开错误");

    }else{

        //读取数据
        m_fwData = f.readAll();
        f.close();
    }

    m_fwTotalLength = m_fwData.length();
    m_fwWriteLength = 0;
}

int SerialThread::getFWSeq(unsigned char *buf , int length)
{
    int retlen = 0;
    int shengyu = (m_fwTotalLength - m_fwWriteLength);

    qDebug() << "m_fwTotalLength   " << m_fwWriteLength << "/" << m_fwTotalLength;

    qDebug() << "getFWSeq length   " << length << shengyu;

    if (shengyu == 0)
    {
        retlen = 0;
        goto ret;
    }

    if (shengyu < length)
    {
        retlen = shengyu;
        goto ret;
    }else{
        retlen = length;
    }

ret:

    //读取数据，并增加记录长度
    memcpy(buf,((unsigned char*)m_fwData.data())+m_fwWriteLength,retlen);
    m_fwWriteLength += retlen;

    return retlen;
    //
}

