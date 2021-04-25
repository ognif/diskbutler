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

#ifndef AUDIOTAGINFO_H
#define AUDIOTAGINFO_H

#include "FoxSDKExport.h"
#include <QString>


struct audioTags
{
    QString m_Title;
    QString m_Artist;
    QString m_Genre;
    QString m_Album;
    int m_Year;
    int m_AlbumYear;
};

class audioTagInfo
{

public:
    audioTagInfo() :
        m_CdTextHandle(-1),
        m_NetworkHandle(-1)
    {
    }

    virtual ~audioTagInfo();

    bool inititalize(int32 inMode);
    audioTags getData(int nTrack) const;
    int32 getHandle() {return m_NetworkHandle;}
    int32 nMode;
private:
    int32 m_CdTextHandle;
    int32 m_NetworkHandle;

    static int32 extractTextFromHandle(bool network, int32 handle,int32 nTrackNumber,int32 nCDTCI, QString& rText);
};

#endif // AUDIOTAGINFO_H
