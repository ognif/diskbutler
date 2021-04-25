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

#include "qfolderitem.h"
#include "qfileitem.h"

QFolderItem* QFolderItem::create(QDataItem *parent, HSESSION hSession
                                 , const SFileEntry& entry
                                 , int &countFile, int &countFolder)
{
    return new QFolderItem(parent, hSession, entry, countFile, countFolder);
}

QFolderItem::QFolderItem(QDataItem *parent, HSESSION hSession
                         , const SFileEntry& entry
                         , int &countFile, int &countFolder)
    : QDataItem(parent)
    , mHSession(hSession)
    , mInfo(entry)
{
    SetType(QDataItem::Folder);
    SetDefaultIcon();
    ParseGenericInfo();
    CreateAndLoadChildren(countFile, countFolder);
}

void QFolderItem::ParseGenericInfo()
{

    SetName(convertToQT(mInfo.lpszFileName));

    SetText0(GetName());
    //info.lba = mInfo.nAddress;
    qlonglong nTemp = (qlonglong)mInfo.nFileSize;
    SetDataSize(nTemp,true);
    nTemp = (qlonglong)mInfo.nAddress;
    SetDataLBA(nTemp);
    ParseAndSetDateTime(mInfo);

    SetFullPath(convertToQT(mInfo.lpszFilePath)
                + PATHSEPSTRING
                + convertToQT(mInfo.lpszFileName));


}

void QFolderItem::CreateAndLoadChildren(int &countFile, int &countFolder)
{
    //setExpanded(true);

    HDIR hDir = 0;

    const TCHAR *pFullPath = convertToFoxValue(GetFullPath());
    int res = ::OpenDirectory(mHSession, pFullPath, &hDir);
    delete [] pFullPath;

    if (res != BS_SDK_ERROR_NO) {
        return;
    }

    countFile = 0;
    countFolder = 0;
    for (int i = 0; ; ++i) {
        SFileEntry entry;
        res = ::ReadDirectory(hDir, i, &entry);
        if (res != BS_SDK_ERROR_NO)
            break;

        if (entry.nAttrib & BS_FA_DIRECTORY) {
            int tempCountFile = 0, tempCountFolder = 0;
            QFolderItem::create(this, mHSession, entry, tempCountFile, tempCountFolder);
            countFile += tempCountFile;
            countFolder += tempCountFolder;
        } else {
            QFileItem::create(this, mHSession, entry);
            countFile += 1;
        }
    }
    SetDataItemCount(countFile);
    SetDataNodeCount(countFolder);
    countFolder += 1;
    ::CloseDirectory(hDir);
}

QString QFolderItem::createAttributeString()
{
    QString res;
    int32 attr = mInfo.nAttrib;

    if (attr & BS_FA_ARCHIVE)
    {
        res += "A ";
    }

    if (attr & BS_FA_HIDDEN)
    {
        res += "H ";
    }

    if (attr & BS_FA_ADVANCED_HIDDEN)
    {
        res += "H+ ";
    }

    if (attr & BS_FA_READONLY)
    {
        res += "R ";
    }

    if (attr & BS_FA_SYSTEM)
    {
        res += "S ";
    }

    return res.trimmed();
}
