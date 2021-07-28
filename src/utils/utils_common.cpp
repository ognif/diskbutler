/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 2021 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "utils_common.h"
#include <QDebug>

QString humanReadableSize(const qint64 src_size, qint64 *dst_size)
{
    QString readableStr;
    double readableSize;
    int count = 0;
    readableSize = src_size;
    while (readableSize >= 1024.0 && count < 5) {
        count++;
        readableSize /= 1024.0;
    }


    readableStr = QString().setNum(readableSize, 'f', 1) + " ";
    switch (count) {
    case 0:
        readableStr += ("bytes");
        break;
    case 1:
        readableStr += ("KB");
        break;
    case 2:
        readableStr += ("MB");
        break;
    default:
        readableStr += ("GB");
        break;
    }
    if (nullptr != dst_size) {
        *dst_size = readableSize;
    }
    return readableStr;
}

QString humanReadableSector(const qint64 src_size)
{
    QString readableStr = "%1 sectors";
    float sector = src_size/2048;
    if (src_size > 0) sector++;
    readableStr = readableStr.arg(sector);
    return readableStr;
}

QDateTime ConvertDateTime(const SFileDateTime &dateTime)
{
    QString date_str;
    date_str = QString("%1 %2 %3 %4:%5:%6").arg(dateTime.nMonth, 2, 10, QLatin1Char('0')).arg(dateTime.nDay, 2, 10, QLatin1Char('0')).
            arg(dateTime.nYear + 1900).arg(dateTime.nHour, 2, 10, QLatin1Char('0')).arg(dateTime.nMinute, 2, 10, QLatin1Char('0')).arg(dateTime.nSecond, 2, 10, QLatin1Char('0'));

    return QDateTime::fromString(date_str, "MM dd yyyy hh:mm:ss");
}

QByteArray CalcChecksum(const QString &filePath)
{
    QCryptographicHash crypto(QCryptographicHash::Sha1);
    QFile file(filePath);
    file.open(QFile::ReadOnly);
    int n = 0;
    while (n < 100000 && !file.atEnd()) {
        crypto.addData(file.read(8192));
        n++;
    }
    return crypto.result().toHex();
}

QString convertToQT(const TCHAR * strValue)
{
#if defined (WIN32)
    return QString::fromWCharArray(strValue);
#else
    return QString::fromUtf8(strValue);
#endif
}

const TCHAR* convertToFoxValue(const QString& strValue)
{

#if defined (WIN32)
    TCHAR *someVar=new TCHAR[strValue.size()+1];
    strValue.toWCharArray(someVar);
    //set last caharacter to null terminator
    someVar[strValue.size()]=L'\0';
    return reinterpret_cast<const TCHAR *>(someVar);
#else
    TCHAR *someVar=new TCHAR[strValue.size()+1];
    QByteArray pass = strValue.toUtf8();
    strcpy(someVar,pass.data());
    return reinterpret_cast<const TCHAR *>(someVar);
#endif

}

int32 MSFToInt32(const int minutes, const int seconds, const int frames)
{
    return (minutes * 60 + seconds) * 75 + frames;
}

QString MSFToStr(const int minutes, const int seconds, const int frames)
{
    QString indexStr;
    indexStr = QString("%1:%2:%3").arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')).arg(frames);
    return indexStr;
}

QString MSFInt32ToStr(int32 index)
{
    int frames = index % 75;
    int seconds = (index / 75) % 60;
    int minutes = index / 75 / 60;
    return MSFToStr(minutes, seconds, frames);
}

int32 MSFStrToInt32(QString str)
{
    int minutes, seconds, frames;
#if defined (WIN32)
    sscanf_s(str.toStdString().c_str(), "%02d:%02d:%02d", &minutes, &seconds, &frames);
#else
    sscanf(str.toStdString().c_str(), "%02d:%02d:%02d", &minutes, &seconds, &frames);
#endif
    
    return MSFToInt32(minutes, seconds, frames);
}

bool fileExists(QString strFile)
{
    bool isExists = false;
    QFile file(strFile);
    if( file.exists() )
    {
        isExists = true;
    }
    file.close();
    return isExists;
}

/*
QMainWindow* getMainWindow()
{
    foreach(QWidget *widget, qApp->topLevelWidgets())
    {
        if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(widget))
        {
            return mainWindow;
        }
    }

    return NULL;
}
*/

QAudioTrackItem *getAudioTrack(QDataItem *item)
{
    QAudioTrackItem *audio_track = nullptr;
    if(item){
        if(item->GetType()==QDataItem::AudioTrack || item->GetType()==QDataItem::File){
            if(item->GetType()==QDataItem::File){
                QTreeWidgetItem* parentItem = item->parent();
                if(parentItem){
                    if(((QDataItem*)parentItem)->GetType()==QDataItem::AudioTrack){
                        audio_track = dynamic_cast<QAudioTrackItem *>(parentItem);
                    }
                }
            }
            if(audio_track == nullptr){
                audio_track = dynamic_cast<QAudioTrackItem *>(item);
            }
            return audio_track;
        }
    }
    return nullptr;
}
