#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    emit cancelRead();
    emit cancelWrite();

    delete newfilewrite;
    delete newfileread;
    delete ui;
}

void MainWindow::on_Button_File_Sistem_clicked()
{
    QString PathFile;
    PathFile = QFileDialog::getOpenFileName(this, "Открыть файл...");
    ui->line_Path_File->setText(PathFile);
}
void MainWindow::on_Button_Read_File_clicked()
{
    QString NameFile = ui->line_Path_File->text();

    if(!NameFile.isEmpty())
        NameFile = ui->line_Path_File->text();
    else
    {
        NameFile = QFileDialog::getOpenFileName(this, "Прочитать файл...");
        ui->line_Path_File->setText(NameFile);
    }
    if(!DataFile.isEmpty())
        DataFile.clear();

    newfileread = new FileReading(NameFile);
    connect(newfileread, SIGNAL(progressChanged(float,float,float,float,float)),SLOT(onProgressRead(float,float,float,float,float)));
    connect(newfileread, SIGNAL(readingData(int,QByteArray)),              SLOT(onReadingData(int,QByteArray)));
    connect(newfileread, SIGNAL(readingInfo(int,QString)),                 SLOT(onReadingInformation(int,QString)));
    connect(ui->Button_Cancellation, SIGNAL(clicked(bool)), newfileread,   SLOT(cancel()));
    connect(ui->Button_Pause, SIGNAL(clicked()),            newfileread,   SLOT(pause()));

    QThreadPool::globalInstance()->start(newfileread);
    ui->Button_Cancellation ->setEnabled(true);
    ui->Button_Pause        ->setEnabled(true);
    ui->Button_Save_as      ->setEnabled(false);
}
void MainWindow::on_Button_Save_as_clicked()
{
    if(DataFile.isEmpty())
    {
        ui->statusbar->showMessage("Нет данных для записи в файл...", 2000);
        return;
    }
    QString NameFile = QFileDialog::getSaveFileName(this, "Cохранить файл как...", QDir::currentPath(), FileExtensions);

    newfilewrite = new FileWriting(NameFile, DataFile);
    connect(newfilewrite, SIGNAL(progressChanged(float,float,float,float,float)),SLOT(onProgressWrite(float,float,float,float,float)));
    connect(newfilewrite, SIGNAL(writingData(int)),                              SLOT(onWritingData(int)));
    connect(ui->Button_Cancellation, SIGNAL(clicked(bool)), newfilewrite,        SLOT(cancel()));

    QThreadPool::globalInstance()->start(newfilewrite);
    ui->Button_Cancellation ->setEnabled(true);
    ui->Button_Pause        ->setEnabled(true);
    ui->Button_Read_File    ->setEnabled(false);
}

void MainWindow::on_Button_Cancel_clicked()
{
    QApplication::quit();
}
void MainWindow::on_Button_Pause_clicked(bool checked) {}
void MainWindow::on_Button_Cancellation_clicked(bool checked) {}

void MainWindow::onProgressRead(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed)
{
    ui->ProgressBar_Load->setValue(procent);
    ui->label_time->setText("Оставшееся время: " + QString::number(remainingtime/1000, 'f', 2) + " cек");
    ui->label_speed_byte->setText("Скорость чтения:     " + QString::number(speedread/1024, 'f', 2) + " кБайт/cек");
    ui->label_last_time->setText("Прошедшее время:           " + QString::number(lasttime/1000, 'f', 2) + " cек");
    ui->label_medium_speed->setText("Средняя скорость чтения: " + QString::number(mediumspeed/1024, 'f', 2) + " кБайт/cек");
}
void MainWindow::onReadingInformation(int resultCode, const QString& infomation)
{
    switch (resultCode)
    {
    case FileReading::Result_sucf:
        ui->statusbar->showMessage("Информация о файле прочитана", 2000);
        ui->Window_Information->setText(infomation);
        break;
    case FileReading::Result_fail:
        ui->statusbar->showMessage("Ошибка чтения данных", 2000);
        break;
    case FileReading::Result_cancel:
        ui->statusbar->showMessage("Чтение данных отменено", 2000);
        break;
    default:
        break;
    }
}

void MainWindow::onProgressWrite(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed)
{
    ui->ProgressBar_Load->setValue(procent);
    ui->label_time->setText("Оставшееся время: " + QString::number(remainingtime/1000, 'f', 2) + " cек");
    ui->label_speed_byte->setText("Скорость записи:     " + QString::number(speedread/1024, 'f', 2) + " кБайт/cек");
    ui->label_last_time->setText("Прошедшее время:           " + QString::number(lasttime/1000, 'f', 2) + " cек");
    ui->label_medium_speed->setText("Средняя скорость запсиси: " + QString::number(mediumspeed/1024, 'f', 2) + " кБайт/cек");
}

void MainWindow::onWritingData(int resultCode)
{
    switch (resultCode)
    {
    case FileReading::Result_sucf:
        ui->statusbar->showMessage("Данные записаны", 2000);
        break;

    case FileReading::Result_fail:
        ui->statusbar->showMessage("Ошибка записи данных", 2000);

        ui->label_time->setText("Оставшееся время: 0.00 cек");
        ui->label_speed_byte->setText("Скорость записи:     0.00 кБайт/cек");
        ui->label_last_time->setText("Прошедшее время:           0.00 cек");
        ui->label_medium_speed->setText("Средняя скорость записи: 0.00 кБайт/cек");
        break;

    case FileReading::Result_cancel:
        ui->statusbar->showMessage("Запись данных отменена", 2000);
        ui->ProgressBar_Load->setValue(0);

        ui->label_time->setText("Оставшееся время: 0.00 cек");
        ui->label_speed_byte->setText("Скорость записи:     0.00 кБайт/cек");
        ui->label_last_time->setText("Прошедшее время:           0.00 cек");
        ui->label_medium_speed->setText("Средняя скорость записи: 0.00 кБайт/cек");
        break;

    default:
        break;
    }
    ui->Button_Cancellation ->setEnabled(false);
    ui->Button_Pause        ->setEnabled(false);
    ui->Button_Read_File    ->setEnabled(true);
}
void MainWindow::onReadingData(int resultCode, const QByteArray& data )
{
    switch (resultCode)
    {
        case FileReading::Result_sucf:
            DataFile = data;
            ui->statusbar->showMessage("Данные прочитаны", 2000);

            ui->Button_Cancellation ->setEnabled(false);
            ui->Button_Pause        ->setEnabled(false);
            ui->Button_Save_as      ->setEnabled(true);
            break;

        case FileReading::Result_fail:
            ui->statusbar->showMessage("Ошибка чтения данных", 2000);

            ui->Button_Cancellation ->setEnabled(false);
            ui->Button_Pause        ->setEnabled(false);
            ui->Button_Save_as      ->setEnabled(true);
            break;

        case FileReading::Result_cancel:
            ui->statusbar->showMessage("Чтение данных отменено", 2000);

            ui->ProgressBar_Load->setValue(0);
            ui->label_time->setText("Оставшееся время: 0.00 cек");
            ui->label_speed_byte->setText("Скорость чтения:     0.00 кБайт/cек");
            ui->label_last_time->setText("Прошедшее время:           0.00 cек");
            ui->label_medium_speed->setText("Средняя скорость чтения: 0.00 кБайт/cек");

            ui->Button_Cancellation ->setEnabled(false);
            ui->Button_Pause        ->setEnabled(false);
            ui->Button_Save_as      ->setEnabled(true);
            break;

        case FileReading::Result_pause:
            ui->statusbar->showMessage("Чтение данных временно остановлено", 2000);
            ui->Button_Pause->setText("Возобновить");
            break;

        case FileReading::Result_resume:
            ui->statusbar->showMessage("Чтение данных возобновлено", 2000);
            ui->Button_Pause->setText("Пауза");
            break;
        default:
            break;
    }
}
