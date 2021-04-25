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

#ifndef QMEDIUMITEM_H
#define QMEDIUMITEM_H

#include "qdataitem.h"
#include <QList>

#include "FoxSDKExport.h"
#include "utils_common.h"

class QMediumItem : public QDataItem
{
public:
    static QMediumItem* create(CommonTreeWidget *view);

    void ParseGenericInfo();
    void CreateAndLoadChildren();

    virtual QString getMediumType() {
        return convertToQT(mInfo.chMediumType);
    }
    virtual QString getMediumStatus();
    virtual QString getUPCCode() {
        return convertToQT(mInfo.chUPCEANCode);
    }
    virtual int getTrackCount() {return mInfo.nLastTrack;}
    virtual int getSessionCount() {return mInfo.nLastSession;}
    virtual qint64 getMediumSize() {return mInfo.dMediumSize;}
    //We return the new calculated size in property dialog
    virtual qint64 getMediumUsedSize() {return nMediumSize; /*mInfo.dMediumUsedSize;*/}
    virtual qint64 getMediumFreeSize();
    virtual int16 getImageFormats() {return nImageFormats;}
    QString getPerformer() {return strPerformer;}
    QString getTitle() {return strTitle;}
    QString getArranger() {return strArranger;}
    QString getComposer() {return strComposer;}
    QString getMessage() {return strMessage;}
    QString getSongWriter() {return strSongWriter;}
    int16 getExtendedMediumType() {return mInfo.nExtendedMediumType;}
    void setMediumSize(qint64 nSize) {nMediumSize=nSize;}
    void setNewSize() {SetDataSize(nMediumSize,true);}


private:
    QMediumItem(CommonTreeWidget *view);

protected:
    int32 ExtractTextFromHandle(int32 handle,int32 nCDTCI, QString& rText);
    SMediumInfo mInfo;
    int16 nImageFormats;
    QString strPerformer;
    QString strTitle;
    QString strArranger;
    QString strComposer;
    QString strMessage;
    qint64 nMediumSize;
    QString strSongWriter;
};

#endif // QMEDIUMITEM_H
