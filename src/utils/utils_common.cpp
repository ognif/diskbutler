/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 20019 Ingo Foerster (pixbytesl@gmail.com).
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

  //qDebug() << QString::number(count);

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
  if (NULL != dst_size) {
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
  date_str.sprintf("%02d %02d %04d %02d:%02d:%02d", dateTime.nMonth, dateTime.nDay, dateTime.nYear + 1900, dateTime.nHour, dateTime.nMinute, dateTime.nSecond);

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

wchar_t* convertToFoxValue(QString strValue)
{
    wchar_t* pass;
    pass = (wchar_t*) malloc (sizeof(wchar_t)*strValue.length()+1);
    strValue.toWCharArray(pass);
    pass[strValue.length()]=0; // Null terminate the string

    return pass;
}

int32 MSFToInt32(const int minutes, const int seconds, const int frames)
{
  return (minutes * 60 + seconds) * 75 + frames;
}

QString MSFToStr(const int minutes, const int seconds, const int frames)
{
  QString indexStr;
  indexStr.sprintf("%02d:%02d:%02d", minutes, seconds, frames);
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
  sscanf(str.toStdString().c_str(), "%02d:%02d:%02d", &minutes, &seconds, &frames);
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

MainWindow* getMainWindow()
{
  QWidgetList widgets = qApp->topLevelWidgets();
  for (QWidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i) {
    QString temp = (*i)->objectName();
    if ((*i)->objectName() == "RibbonMainWindow")
      return qobject_cast<MainWindow *> (*i);
  }
  return NULL;
}
