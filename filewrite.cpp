#include "filewrite.h"


FileWriting::FileWriting(const QString& FileName, QByteArray& DataFile, quint64 SizeChunk):
    QObject(NULL), File(FileName), Data(DataFile), CancelMarker(false), SizeChunkWrite(SizeChunk)
{
    if(SizeChunkWrite < 102'400)
        SizeChunkWrite = 102'400;
    SizeChunkWrite *= 1024;
}

FileWriting::~FileWriting()
{}

void FileWriting::run()
{
    if(!File.open(QIODevice::WriteOnly))
    {
        emit writingData(Result_fail);
        return;
    }

    last_time.start();
    qsizetype size = Data.size();
    while(!Data.isEmpty())
    {
        if(CancelMarker.testAndSetAcquire(true, true))
        {
            emit writingData(Result_cancel);
            return;
        }

        time.start();
        try
        {
            if(Data.size() < SizeChunkWrite)
                SizeChunkWrite = Data.size();
            File.write(Data, SizeChunkWrite);
            Data.remove(0, SizeChunkWrite);
            File.flush();
        }
        catch (...)
        {
            emit writingData(Result_fail);
            return;
        }

        //Вычисление основных характеристик
        float speedread = static_cast<float>(SizeChunkWrite) / time.elapsed();
        float lasttime = last_time.elapsed();
        float procent = 100* (1 - static_cast<float>(Data.size()) / size);
        float remainingtime = static_cast<float>(Data.size()) / speedread;
        float mediumspeed = static_cast<float>(size - Data.size()) / lasttime;
        emit progressChanged(procent, remainingtime, speedread, lasttime, mediumspeed);
    }
    if(File.error() != QFile::NoError)
    {
        emit writingData(Result_fail);
        return;
    }
    emit writingData(Result_sucf);
    File.close();
}

void FileWriting::cancel()
{
    CancelMarker.fetchAndAddAcquire(true);
}
