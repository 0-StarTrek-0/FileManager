#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QThreadPool>
#include <QElapsedTimer>
#include <QMessageBox>

#include "fileread.h"
#include "filewrite.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Button_Read_File_clicked();
    void on_Button_Cancel_clicked();
    void on_Button_Save_as_clicked();
    void on_Button_File_Sistem_clicked();
    void on_Button_Pause_clicked(bool checked);
    void on_Button_Cancellation_clicked(bool checked);

    void onProgressRead(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed);
    void onReadingData( int resultCode, const QByteArray& data );
    void onReadingInformation( int resultCode, const QString& infomation);

    void onProgressWrite(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed);
    void onWritingData( int resultCode);

signals:
    void cancelRead();
    void cancelWrite();

private:
    QByteArray DataFile;

    FileReading* newfileread;
    FileWriting* newfilewrite;

    QString FileExtensions{"Text files (*.txt);;Exel files (*.xlsx *.xlsb *.xlsm);;Images (*.png *.xpm *.jpg);;XML files (*.xml);;Music (*.mp3);;All files (*.*);;"};

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
