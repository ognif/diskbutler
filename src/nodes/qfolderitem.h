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

#ifndef QFOLDERITEM_H
#define QFOLDERITEM_H

#include "qdataitem.h"
#include "FoxSDKExport.h"
#include "utils_common.h"

class QFolderItem : public QDataItem
{
public:
    static QFolderItem* create(QDataItem *parent, HSESSION hSession
                               , const SFileEntry& entry
                               , int &countFile, int &countFolder);

    void ParseGenericInfo();
    void CreateAndLoadChildren(int &countFile, int &countFolder);
    virtual QString GetLBA() {return QString("%1").arg(mInfo.nAddress);}
    virtual QString getPath() {
        return convertToQT(mInfo.lpszFilePath);
    }
    virtual QString createAttributeString();

private:
    QFolderItem(QDataItem *parent, HSESSION hSession
                , const SFileEntry& entry
                , int &countFile, int &countFolder);

protected:
    HSESSION mHSession;
    SFileEntry mInfo;
};

#endif // QFOLDERITEM_H
