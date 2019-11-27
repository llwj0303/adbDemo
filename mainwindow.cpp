#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QTextStream>

//test github
InstallApkThread::InstallApkThread(QObject *parent)
    :QThread(parent)
{

}

void InstallApkThread ::run()
{
    QProcess p;
    p.start(QApplication::applicationDirPath() + "/adb/installapk.bat");

    if (p.waitForStarted())
    {
        p.waitForFinished();

        m_str = QString::fromLocal8Bit(p.readAllStandardOutput());
//        qDebug()<<m_str;
    }
    else
        qDebug() << "Failed to start";
}

//*******************************************
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),m_printTimer(0),
    ui(new Ui::MainWindow)
{
    this->setFixedSize(400, 300);

    ui->setupUi(this);

    setWindowTitle(tr("Install Apk"));
    ui->pushButton->setEnabled(false);
//    ui->pushButton->setText(tr("Install apk"));
//    ui->pickFileBtn->setText(tr("Select"));

    m_installThread = NULL;
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(installApk()));
    connect(ui->pickFileBtn, SIGNAL(clicked()), this, SLOT(openFileDialog()));
}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_installThread)
        delete m_installThread;
}

void MainWindow::installApk()
{
    if(m_installThread && m_installThread->isRunning())
        return;
    ui->textEdit->clear();
    ui->pushButton->setEnabled(false);
//    m_num = 0;

    ui->textEdit->setText(QString(tr("start install apk")));
    if(!m_installThread)
    {
        m_installThread = new InstallApkThread(this);
    }
    m_installThread->start(QThread::LowestPriority);//设置为次线程

    if(!m_printTimer)
    {
        m_printTimer = new QTimer(this);
        connect(m_printTimer, SIGNAL(timeout()), this, SLOT(printStr()));
    } 
    m_printTimer->setInterval(2000);
    m_printTimer->start();
}

void MainWindow::printStr()
{
    ui->textEdit->append(QString(tr("wait...")));
//    m_num++;
    if(m_installThread->isFinished())//是否都会有返回值？
    {
        qDebug()<<m_installThread->getOutPutStr();
        if(m_installThread->getOutPutStr().contains("Success", Qt::CaseInsensitive))
        {
            ui->textEdit->append(QString(tr("Install Apk Success!")));
            if(m_printTimer->isActive())
                m_printTimer->stop();
        }
        else
        {
            ui->textEdit->append(QString(tr("Install Failed!")));
            if(m_printTimer->isActive())
                m_printTimer->stop();
        }
        ui->pushButton->setEnabled(true);
    }
//    else if(m_num >= 30)
//    {
//        ui->textEdit->append(QString(tr("Install Failed!")));
//        if(m_printTimer->isActive())
//            m_printTimer->stop();

//        ui->pushButton->setEnabled(true);
//    }

}

void MainWindow::openFileDialog()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Open APK Files"),
                                                QDir::current().absolutePath(),
                                                tr("APK Files (*.apk)"));
    if(!file.isEmpty())
    {
        QDir::setCurrent(QFileInfo(file).absolutePath());
        ui->fileNameLineEdit->setText(file);

        ui->pushButton->setEnabled(true);

        QString batFilePath = QApplication::applicationDirPath() + "/adb/installapk.bat";
        QFile batFile(batFilePath);

        if (!batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(NULL, "提示", "无法创建文件");
            return;
        }
        QTextStream out(&batFile);
        out<<"PUSHD %~dp0"<<"\n"<<"adb install -r "<<"\""<<file<<"\"" <<endl;
        out.flush();
        batFile.close();
    }
}

