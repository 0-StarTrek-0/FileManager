#include "log.h"

Log::Log()
{}
Log::~Log()
{
    delete file;
}

void Log::SaveFile(QString Action, QString InformationFile)
{
    FileAction = Action;
    FileInfo = InformationFile;
    CreateFile();
    SaveAction();
    CloseFile();

}

void Log::CreateFile()
{
    NameFile = QDir::currentPath() + "\\" + DateNow();
    NameFile.replace(R"(.)", R"(-)");
    NameFile += ".txt";
    file = new QFile(NameFile);
    file->open(QIODevice::Append | QIODevice::Text);
}
void Log::SaveAction()
{
    static quint32 act = 1;
    if (file->isOpen())
    {
        QTextStream stream(file);
        stream << QString::number(act) << ") " << TimeNow() << "  ";
        stream << "Action - " << FileAction << "\n";
        stream << FileInfo << "\n";

        file->flush();
        act++;
    }
}

void Log::CloseFile()
{
    if (file->isOpen())
        file->close();
    delete file;
}

QString Log::TimeNow()
{
    QTime time;
    time = QTime::currentTime();
    QString nowtime;
    nowtime = time.toString("hh:mm:ss");
    return nowtime;
}
QString Log::DateNow()
{
    QDate date;
    date = QDate::currentDate();
    QString datenow;
    datenow = date.toString("dd.MM.yyyy");
    return datenow;
}
