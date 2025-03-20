#ifndef FILEWRITE_H
#define FILEWRITE_H

#include <QObject>
#include <QRunnable>
#include <QFile>
#include <QFileInfo>
#include <QElapsedTimer>

class FileWriting : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum ResultCode
    {
        Result_sucf,
        Result_fail,
        Result_cancel
    };
public:
    FileWriting(const QString& FileName, QByteArray& DataFile, quint64 SizeChunk = 102'400);
    ~FileWriting();

    void run();

public slots:
    void cancel();

signals:
    void progressChanged(float procent, float remainingtime, float speedread, float lasttime, float mediumspeed);
    void writingData(int resultCode);


private:
    QFile File;
    QByteArray& Data;
    quint64 SizeChunkWrite;

    QElapsedTimer time;
    QElapsedTimer last_time;

    QAtomicInt CancelMarker;
};

#endif // FILEWRITE_H
