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

#ifndef QFILEITEM_H
#define QFILEITEM_H

#include "qdataitem.h"
#include "FoxSDKExport.h"
#include "utils_common.h"

class QFileItem : public QDataItem
{
public:
    static QFileItem* create(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry);

    void ParseGenericInfo();
    virtual QString GetLBA() {return QString("%1").arg(mInfo.nAddress);}
    virtual std::vector<uint32>* GetExtentLocation() {return &mLocationVector;}
    virtual std::vector<uint32>* GetExtentLength() {return &mLengthVector;}
    virtual QString createAttributeString();
    virtual QString getPath() {
        return convertToQT(mInfo.lpszFilePath);
    }


private:
    QFileItem(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry);
    void ParseAllocationTable(const HSESSION hSession);


protected:
    SFileEntry mInfo;
    std::vector<uint32> mLocationVector;
    std::vector<uint32> mLengthVector;

};

#endif // QFILEITEM_H
