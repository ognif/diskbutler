#ifndef BASSDECODE_H
#define BASSDECODE_H


#include "FoxSDKDefinitions.h"
#include "utils_common.h"
#include <vector>

namespace gBassLib
{

	bool	IsLibLoaded();
    bool    IsMixerLoaded();
    


    bool                GetPlayTime(const QString& strFilePath, size_t& nPlayTime);
    bool                GetPrecisePlayTime(const QString& strFilePath, double& fPlayTime);
    bool                PlayToCard(const QString& strFilePath);
    void                StopPlayToCard();
    QStringList         TryTags(const QString& strFileName);
    bool                LoadPlugLibrary(const QString& strPlugName);
    
}


#endif 

