#ifndef AUDIOTAGINFO_H
#define AUDIOTAGINFO_H

#include "FoxSDKExport.h"



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
