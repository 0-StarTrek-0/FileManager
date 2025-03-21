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
    //Попытка открытия файла и его проверка на открытие
    if(!File.open(QIODevice::ReadOnly))
    {
        //Уведомление о фейле чтения
        emit readingData(Result_fail);
        return;
    }

    QByteArray Data;
    QByteArray Chunk;
    QString InfoFile;

    //Сборка информации о файле в строку
    InfoFile.append("Информация о файле: "      + FileInfo.fileName()                +
                     "\nПуть к файлу: "         + FileInfo.absoluteFilePath()        +
                     "\nРазмер файла: "         + QString::number(FileInfo.size())   + " байт" +
                     "\nПоследнее изменения : " + FileInfo.lastModified().toString() +
                     "\nПоследнее чтение: "     + FileInfo.lastRead().toString()     + "\n\n\n");
    emit readingInfo(Result_sucf, InfoFile);

    last_time.start();
    do {
        //Проверка на нажатие кнопки паузы или отмены
        if(CancelMarker.testAndSetAcquire(true, true))
        {
            emit readingData(Result_cancel);
            return;
        }
        //Нажата кнопка паузы в ходим в бесконечный цикл ожидания, пока не будет изменен PauseMarker
        if(PauseMarker.testAndSetAcquire(true, true))
        {
            emit readingData(Result_pause);
            while(PauseMarker.testAndSetOrdered(true, true))
            {
                if(CancelMarker.testAndSetAcquire(true, true))
                {
                    emit readingData(Result_cancel);
                    return;
                }
                QThread::msleep(500);
            }
            //Обнуляем PauseMarker
            PauseMarker.deref();
            PauseMarker.deref();
            emit readingData(Result_resume);
        }

        //Считывание части файла размером с Chunk и запись в массив Data;
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
        float speedread = static_cast<float>(SizeChunkRead) / (time.elapsed());
        float remainingtime = static_cast<float>(File.size() - File.pos()) / speedread;
        float lasttime = last_time.elapsed();
        float mediumspeed = static_cast<float>(File.pos()) / lasttime;
        //Отправка данных в основное окно
        emit progressChanged(procent, remainingtime, speedread, lasttime, mediumspeed);
    } while(!Chunk.isEmpty());

    if(File.error() != QFile::NoError)
    {
        //Уведомление о фейле чтения
        emit readingData(Result_fail);
        return;
    }
    //Уведомление об успешности чтения
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

