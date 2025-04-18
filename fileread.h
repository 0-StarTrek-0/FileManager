#ifndef FILEREADING_H
#define FILEREADING_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QWaitCondition>
#include <QElapsedTimer>

class FileReading : public QObject, public QRunnable
{
    Q_OBJECT
public:
    //Коды результата чтения
    enum ResultCode
    {
        Result_sucf,
        Result_fail,
        Result_cancel,
        Result_pause,
        Result_resume
    };
public:
    FileReading(const QString& FileName, quint64 SizeRead = 100'000);
    ~FileReading();

    void run();

public slots:
    void cancel();
    void pause();

signals:
    void progressChanged(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed);
    void readingData(int resultCode, const QByteArray& DataFile = QByteArray());
    void readingInfo(int resultCode, const QString& InformationFile);

private:
    QFile File;
    QFileInfo FileInfo;
    //Размер чанка для единоразового считывания с файла
    quint64 SizeChunkRead;

    //Переменные для засекания времени
    QElapsedTimer time;
    QElapsedTimer last_time;

    //Атомарные переменные
    QAtomicInt CancelMarker;
    QAtomicInt PauseMarker;
};

#endif // FILEREADING_H
