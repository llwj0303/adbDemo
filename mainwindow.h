#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

class InstallApkThread;

class InstallApkThread: public QThread
{
    Q_OBJECT
public:
    InstallApkThread(QObject * parent = 0);

    QString getOutPutStr()
    { return m_str;}

protected:
    void run();

private:
    QString  m_str;
    friend class MainWindow;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void installApk();
    void printStr();
    void openFileDialog();

private:
    Ui::MainWindow *ui;

    InstallApkThread  *m_installThread;
    QTimer            *m_printTimer;
    int                m_num;

};

#endif // MAINWINDOW_H
