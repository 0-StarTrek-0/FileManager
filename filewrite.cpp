#include "filewrite.h"


FileWriting::FileWriting(const QString& FileName, QByteArray& DataFile, quint64 SizeChunk):
    QObject(NULL), File(FileName), Data(DataFile), CancelMarker(false), SizeChunkWrite(SizeChunk)
{
    //Определяем размер чанка для записи если он меньше - корректируем
    if(SizeChunkWrite < 102'400)
        SizeChunkWrite = 102'400;
    SizeChunkWrite *= 1024;
}

FileWriting::~FileWriting()
{}

void FileWriting::run()
{
    //Проверка открытия файла
    if(!File.open(QIODevice::WriteOnly))
    {
        emit writingData(Result_fail);
        return;
    }

    last_time.start();
    //Фиксируем начальный размер данных
    qsizetype size = Data.size();
    while(!Data.isEmpty())
    {
        //Проверка на отмену записи
        if(CancelMarker.testAndSetAcquire(true, true))
        {
            emit writingData(Result_cancel);
            return;
        }

        time.start();
        try
        {
            //Записываем часты данных Data размером SizeChunkWrite, затем удаляем данные с массивы Data и очищаем буффер
            //Если данных в Data меньше чем размер чанка, уменьшаем чанк до размера оставшихся данных
            if(Data.size() < SizeChunkWrite)
                SizeChunkWrite = Data.size();
            File.write(Data, SizeChunkWrite);
            Data.remove(0, SizeChunkWrite);
            File.flush();
        }
        catch (...)
        {
            //Вывод в основное окно ошибки записи
            emit writingData(Result_fail);
            return;
        }

        //Вычисление основных характеристик
        float speedread = static_cast<float>(SizeChunkWrite) / time.elapsed();
        float lasttime = last_time.elapsed();
        float procent = 100* (1 - static_cast<float>(Data.size()) / size);
        float remainingtime = static_cast<float>(Data.size()) / speedread;
        float mediumspeed = static_cast<float>(size - Data.size()) / lasttime;
        //Отправка данных о процессе записи
        emit progressChanged(procent, remainingtime, speedread, lasttime, mediumspeed);
    }
    if(File.error() != QFile::NoError)
    {
        //Вывод в основное окно ошибки записи
        emit writingData(Result_fail);
        return;
    }
    //Вывод в основное успешности записи
    emit writingData(Result_sucf);
    File.close();
}

void FileWriting::cancel()
{
    CancelMarker.fetchAndAddAcquire(true);
}
