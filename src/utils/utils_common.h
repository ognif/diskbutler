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

#include <QString>
#include <QDateTime>
#include <QCryptographicHash>
#include <QFile>
#include "FoxSDKExport.h"



extern QString humanReadableSector(const qint64 src_size);
extern QDateTime ConvertDateTime(const SFileDateTime &dateTime);
extern QByteArray CalcChecksum(const QString &filePath);
extern const TCHAR* convertToFoxValue(const QString& strValue);
extern QString convertToQT(const TCHAR * strValue);
extern QString humanReadableSize(const qint64 src_size, qint64 *dst_size);

extern int32 MSFToInt32(const int minutes, const int seconds, const int frames);
extern QString MSFToStr(const int minutes, const int seconds, const int frames);
extern QString MSFInt32ToStr(int32 index);
extern int32 MSFStrToInt32(QString str);
extern bool fileExists(QString strFile);
//extern QMainWindow* getMainWindow();
extern wchar_t *machineString(QString input);
