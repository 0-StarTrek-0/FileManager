#ifndef FILEWRITE_H
#define FILEWRITE_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QElapsedTimer>

class FileWriting : public QObject, public QRunnable
{
    Q_OBJECT
public:
    //Коды результата записи
    enum ResultCode
    {
        Result_sucf,
        Result_fail,
        Result_cancel,
        Result_pause,
        Result_resume
    };
public:
    FileWriting(const QString& FileName, QByteArray& DataFile, quint64 SizeChunk = 102'400);
    ~FileWriting();

    void run();

public slots:
    void cancel();
    void pause();

signals:
    void progressChanged(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed);
    void writingData(int resultCode);


private:
    QFile File;
    //Ссылка на данные
    QByteArray& Data;
    //Размер чанка записи
    quint64 SizeChunkWrite;

    //Переменные для засекания времени
    QElapsedTimer time;
    QElapsedTimer last_time;

    //Атомарные переменные
    QAtomicInt CancelMarker;
    QAtomicInt PauseMarker;
};

#endif // FILEWRITE_H
