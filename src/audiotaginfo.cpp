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

#include <QtWidgets>
#include "audiotaginfo.h"



audioTagInfo::~audioTagInfo()
{
    if (m_CdTextHandle >= 0)
        ::CloseCDTextHandle(m_CdTextHandle);

    if (m_NetworkHandle >= 0)
        ::CloseNetworkTagsHandle(m_NetworkHandle);
};

bool audioTagInfo::inititalize(int32 inMode)
{
    nMode = inMode; //CSettingsManager::Instance().GetAudioGrabberSettings().m_CdTextMode;

    if ((nMode == BS_TCH_CDTEXT) || (nMode == BS_TCH_CDTEXT_INTERNETDB) || (nMode == BS_TCH_INTERNETDB_CDTEXT))
    {
        if (::ReadCDText(BS_READ_DEVICE, &m_CdTextHandle) != BS_SDK_ERROR_NO){
            m_CdTextHandle = -1;
            return false;
        }
    }


    if ((nMode == BS_TCH_INTERNETDB) || (nMode == BS_TCH_CDTEXT_INTERNETDB) || (nMode == BS_TCH_INTERNETDB_CDTEXT))
    {
        if (::TagsFromNetworkDialog(BS_READ_DEVICE, &m_NetworkHandle) != BS_SDK_ERROR_NO){
                m_NetworkHandle = -1;
                return false;
            }
    }

    return true;

}

audioTags audioTagInfo::getData(int nTrack) const
{
    audioTags result;

    if ((nMode == BS_TCH_CDTEXT) || (nMode == BS_TCH_CDTEXT_INTERNETDB))
    {
        if (m_CdTextHandle >= 0)
        {
            extractTextFromHandle(false, m_CdTextHandle, nTrack, BS_CDTCI_TITLE, result.m_Title);
            extractTextFromHandle(false, m_CdTextHandle, nTrack, BS_CDTCI_PERFORMER, result.m_Artist);
        }
    }


    if (m_NetworkHandle >= 0)
    {
        extractTextFromHandle(true, m_NetworkHandle, nTrack, BS_TCI_TITLE, result.m_Title);
        extractTextFromHandle(true, m_NetworkHandle, nTrack, BS_TCI_ARTIST, result.m_Artist);
        extractTextFromHandle(true, m_NetworkHandle, nTrack, BS_TCI_GENRE, result.m_Genre);
        extractTextFromHandle(true, m_NetworkHandle, -1, BS_TCI_TITLE, result.m_Album);
        ::GetNetworkTrackTagInt(m_NetworkHandle, nTrack, BS_TCI_YEAR, &result.m_Year);
        ::GetNetworkDiskTagInt(m_NetworkHandle, BS_TCI_YEAR, &result.m_AlbumYear);
    }

    if (nMode == BS_TCH_INTERNETDB_CDTEXT)
    {
        if (m_CdTextHandle >= 0)
        {
            extractTextFromHandle(false, m_CdTextHandle, nTrack, BS_CDTCI_TITLE, result.m_Title);
            extractTextFromHandle(false, m_CdTextHandle, nTrack, BS_CDTCI_PERFORMER, result.m_Artist);
        }
    }

    return result;
}

int32 audioTagInfo::extractTextFromHandle(bool network, int32 handle,int32 nTrackNumber,int32 nCDTCI, QString& rText)
{
    int32 nLen = 0;
    int32 res;

    if (nTrackNumber >= 0)
        res = network ? ::GetNetworkTrackTagString(handle, nTrackNumber, nCDTCI, 0, &nLen) : ::GetCDTextTrackTagString(handle, nTrackNumber, nCDTCI, 0, &nLen);
    else
        res = network ? ::GetNetworkDiskTagString(handle, nCDTCI, 0, &nLen) : BS_SDK_ERROR_NO;

    if(res != BS_SDK_ERROR_NO)
    {
        return res;
    }

    if(!nLen || nLen == 1)
    {
        rText = "";
        return BS_SDK_ERROR_NO;
    }

    TCHAR *pBuf = new TCHAR[nLen];

    if (nTrackNumber >= 0)
        res = network ? ::GetNetworkTrackTagString(handle, nTrackNumber, nCDTCI, pBuf, &nLen) : ::GetCDTextTrackTagString(handle, nTrackNumber, nCDTCI, pBuf, &nLen);
    else
        res = network ? ::GetNetworkDiskTagString(handle, nCDTCI, pBuf, &nLen) : BS_SDK_ERROR_NO;


    if(res != BS_SDK_ERROR_NO)
    {
        delete pBuf;
        return res;
    }

    pBuf[nLen-1] = _T('\0');
#if defined (WIN32)
      rText = QString::fromUtf16(pBuf);//QString::fromWCharArray(pBuf);
#else
      rText = QString::fromUtf8(pBuf);//QString::fromWCharArray(pBuf);
#endif


    delete pBuf;

    return BS_SDK_ERROR_NO;
}
