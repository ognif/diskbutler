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

#include "qfileitem.h"
#include <QApplication>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QTemporaryFile>
#include <QDir>

QFileItem* QFileItem::create(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry)
{
    return new QFileItem(parent, hSession, entry);
}

QFileItem::QFileItem(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry)
    : QDataItem(parent)
    , mInfo(entry)
{
    SetType(QDataItem::File);
    SetDefaultIcon();
    ParseGenericInfo();
    ParseAllocationTable(hSession);
}

QIcon fileExtensionIcon(const QString & extension) {
    static QFileIconProvider iconProvider;

    QIcon icon;

    QTemporaryFile tmpFile(QDir::tempPath() + QDir::separator() +
                           QCoreApplication::applicationName() + "_XXXXXX." + extension);
    tmpFile.setAutoRemove(false);

    if (tmpFile.open()) {
        QString fileName = tmpFile.fileName();
        tmpFile.write(QByteArray());
        tmpFile.close();

        icon = iconProvider.icon(QFileInfo(fileName));

        tmpFile.remove();
    }


    return icon;
}

void QFileItem::ParseGenericInfo()
{

    SetName(convertToQT(mInfo.lpszFileName));

    SetText0(GetName());

    QFileInfo fileinfo(GetName());
    QIcon icon = fileExtensionIcon(fileinfo.suffix());
    setIcon(0, icon);

    //info.lba = m_info.nAddress;
    qlonglong nTemp = (qlonglong)mInfo.nFileSize;
    SetDataSize(nTemp,true);
    nTemp = (qlonglong)mInfo.nAddress;


    SetDataLBA(nTemp);
    ParseAndSetDateTime(mInfo);

    SetFullPath(convertToQT(mInfo.lpszFilePath)
                + PATHSEPSTRING
                + convertToQT(mInfo.lpszFileName));

    SetDataItemCount(1);
}

void QFileItem::ParseAllocationTable(const HSESSION hSession)
{
    //const SFileAllocationTable *allocTable = ((FileNode*)m_treeNode)->getAllocationTable();
    int32 tableSize = 0;
    const TCHAR *pBuf = convertToFoxValue(QDir::toNativeSeparators(GetFullPath()));

    int32 errCode = ::GetFileAllocationTable(hSession, pBuf, NULL, &tableSize);

    if (errCode != BS_SDK_ERROR_MORE_SPACE_NEEDED){
        delete[] pBuf;
        return;
    }

    SFileAllocationTable *allocTable = (SFileAllocationTable*)malloc(tableSize);

    errCode = ::GetFileAllocationTable(hSession, pBuf, allocTable, &tableSize);
    if (errCode == BS_SDK_ERROR_NO) {
        for (int i = 0; i < allocTable->NumExtents; i++) {
            mLocationVector.push_back(allocTable->Extents[i].Location);
            mLengthVector.push_back(allocTable->Extents[i].Length);
        }
    }
    delete[] pBuf;
    free(allocTable);
}

QString QFileItem::createAttributeString()
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
