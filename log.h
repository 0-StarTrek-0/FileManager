#ifndef LOG_H
#define LOG_H

#include <QDate>
#include <QFile>
#include <QTime>
#include <QDir>
#include <QTextStream>

class Log
{
public:
    Log();
    void SaveFile(QString Action, QString InformationFile = QString("None"));

private:
    void CreateFile();
    void SaveAction();
    void CloseFile();

    QString NameFile;
    QString FileInfo;
    QString FileAction;

    QFile* file;

    QString TimeNow();
    QString DateNow();

};

#endif // LOG_H
