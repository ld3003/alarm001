#include "usbtest.h"
#include "ui_usbtest.h"

#include <QDebug>
#include <serialthread.h>
#include <QFileDialog>
#include <QMessageBox>

USBTest::USBTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::USBTest)
{
    ui->setupUi(this);

    m_serialThread = new SerialThread(0);

    connect(m_serialThread,SIGNAL(serialInfoStr(QString)),this,SLOT(serialInfoRecv(QString)), Qt::QueuedConnection);


}

USBTest::~USBTest()
{
    delete ui;
}

void USBTest::on_connectDevButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("选择固件文件"));
    fileDialog->setDirectory(".");
    //fileDialog->setFilter(tr("固件文件(*.bin)"));
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        QMessageBox::information(NULL, tr("提示"), tr("您将要下载的固件文件为: ") + path);
        m_serialThread->m_fwFilePath = path;
        m_serialThread->m_burnState = SerialThread::__INIT;
        m_serialThread->start();
    } else {
        QMessageBox::information(NULL, tr("提示"), tr("您没有选择任何文件。"));
    }

    //m_serialThread->start();
}

void USBTest::timeoutUiTimer()
{
    QCoreApplication::processEvents();//避免界面冻结
    //
}

void USBTest::serialInfoRecv(QString str)
{
    ui->textBrowser->append(str);
}

void USBTest::on_checkConn_clicked()
{
    m_serialThread->m_burnState = SerialThread::__CHECK_DEV;
    m_serialThread->start();
    //
}
