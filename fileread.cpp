#include "fileread.h"

FileReading::FileReading(const QString &FileName, quint64 SizeRead):
    QObject(NULL), File(FileName), SizeChunkRead(SizeRead), FileInfo(FileName), PauseMarker(false), CancelMarker(false)
{
    if(SizeChunkRead < 10'240)
        SizeChunkRead = 10'240;
    SizeChunkRead *= 1024;
}

FileReading::~FileReading() {}

void FileReading::run()
{
    if(!File.open(QIODevice::ReadOnly))
    {
        emit readingData(Result_fail);
        return;
    }

    QByteArray Data;
    QByteArray Chunk;
    QString InfoFile;

    InfoFile.append("Информация о файле: "      + FileInfo.fileName()                +
                     "\nПуть к файлу: "         + FileInfo.absoluteFilePath()        +
                     "\nРазмер файла: "         + QString::number(FileInfo.size())   + " байт" +
                     "\nПоследнее изменения : " + FileInfo.lastModified().toString() +
                     "\nПоследнее чтение: "     + FileInfo.lastRead().toString()     + "\n\n\n");
    emit readingInfo(Result_sucf, InfoFile);

    last_time.start();
    do {
        if(CancelMarker.testAndSetAcquire(true, true))
        {
            emit readingData(Result_cancel);
            return;
        }
        if(PauseMarker.testAndSetAcquire(true, true))
        {
            emit readingData(Result_pause);
            while(PauseMarker.testAndSetAcquire(true, true))
            {
                qDebug() << "stoop";
                QThread::msleep(500);
            }
            emit readingData(Result_resume);
        }

        try
        {
            time.start();
            Chunk = File.read(SizeChunkRead);
            Data.append(Chunk);
        }
        catch (...)
        {
            emit readingData(Result_fail);
            return;
        }
        time.elapsed();

        //Вычисление основных характеристик
        float procent = static_cast<float>(100*File.pos()) / File.size();
        float speedread = static_cast<float>(SizeChunkRead * 1024) / (time.elapsed());
        float remainingtime = static_cast<float>(File.size() - File.pos()) / speedread;
        float lasttime = last_time.elapsed();
        float mediumspeed = static_cast<float>(File.pos()) / lasttime;
        emit progressChanged(procent, remainingtime, speedread, lasttime, mediumspeed);
    } while(!Chunk.isEmpty());

    if(File.error() != QFile::NoError)
    {
        emit readingData(Result_fail);
        return;
    }
    emit readingData(Result_sucf, Data);
    File.close();
}

void FileReading::cancel()
{
    CancelMarker.fetchAndAddAcquire(true);
}

void FileReading::pause()
{
    PauseMarker.fetchAndAddAcquire(true);
}

