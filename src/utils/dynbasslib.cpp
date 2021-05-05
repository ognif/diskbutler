/*
 Comment from Ingo Foerster
 foerster@ifoerster.com
 This file was rrewritten for use of BASS Libs.
 Hire an freelancer who develops the same structure with ffmpeg libs
 and smae Interface funcktions, so we can select what kind of DLL / Decoder to use.
 
 Collect files:
 Ape with v1 and v2 Tags
 mp3 with ID3
 mp3 with ID3V2
 mp4 with iTunes
 ogg file with tags
 wma audio file with tags
 lyrics
 and what in hell is MF

Debug helper
	KStdioFile fDebugFile;
	KString strDebugFile = KString(_T("debug.txt"));
	fDebugFile.Open(strDebugFile, "w");
	fDebugFile.WriteString(strPlugName);
	fDebugFile.WriteString("\n");
	fDebugFile.WriteString("Success");
	int nRes = _BASS_ErrorGetCode();
	KString strError = KStringFormat(_T("return value: %d"), nRes);
	fDebugFile.WriteString(strError);
	fDebugFile.Close();
 */


#ifdef WIN32
#	include <windows.h>
#else
#	include <tchar.h>
#	include <dlfcn.h>

    typedef unsigned int	DWORD;
	typedef int		BOOL;
	typedef unsigned short	WORD;
	typedef char		CHAR;
	typedef short		SHORT;
	typedef long		LONG;
	typedef void*		HANDLE;

	#define TRUE 1
	#define FALSE 0
	#define E_FAIL 0x80004005L
	#define S_OK 0
	#define WINAPI

#endif


#include <cassert>
#include "dynbasslib.h"
#include "bass.h"
#include "bassmix.h"


#ifdef WIN32
	typedef HMODULE DllHandle;
	#define DllLoad(name) (LoadLibrary(name))
	#define DllUnload(handle) (FreeLibrary(handle))
	#define DllFindSym(handle,name) (GetProcAddress(handle,name))	
	#define DLLCONV __stdcall
#else
	typedef void* DllHandle;
	#define DllLoad(name) (dlopen(name, RTLD_LAZY | RTLD_GLOBAL))
	#define DllUnload(handle) (dlclose(handle));
	#define DllFindSym(handle,name) (dlsym(handle,name))
    #define DLLCONV 
#endif

namespace gBassLib
{
	struct	BASSINFO
	{
		int	channelmode;
		int	frequency;         // samplerate
		int	bitrate;
		int	time;                  // nr of seconds in song
		double timeMsec;
		BYTE	title[128];
		BYTE	artist[128];
		bool	bInComplete;				//if this music is completed 
		int	nBitsPerSample;
		BYTE	rev[1000];
	};

	///typedefs used to dynamic load the dll using LoadLibrary
	typedef BOOL	    (DLLCONV *BASS_SetConfig_Type)(DWORD option, DWORD value);
	typedef double	    (DLLCONV *BASS_ChannelBytes2Seconds_Type)(DWORD handle, QWORD pos);
    typedef QWORD       (DLLCONV *BASS_ChannelGetLength_Type)(DWORD handle, DWORD mode);
	typedef BOOL	    (DLLCONV *BASS_Init_Type)(int device, DWORD freq, DWORD flags, void *win, void *dsguid);
    typedef BOOL        (DLLCONV *BASS_ChannelStop_Type)(DWORD handle);
	typedef BOOL	    (DLLCONV *BASS_ChannelPlay_Type)(DWORD handle, BOOL restart);
    typedef BOOL        (DLLCONV *BASS_StreamFree_Type)(HSTREAM handle);
	typedef HSTREAM	    (DLLCONV *BASS_StreamCreateFile_Type)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
	typedef BOOL	    (DLLCONV *BASS_Free_Type)();
    typedef DWORD       (DLLCONV *BASS_ChannelGetData_Type)(DWORD handle, void *buffer, DWORD length);
    typedef DWORD       (DLLCONV *BASS_ChannelIsActive_Type)(DWORD handle);
    typedef QWORD       (DLLCONV *BASS_ChannelGetPosition_Type)(DWORD handle,DWORD mode);
    typedef char*       (DLLCONV *BASS_ChannelGetTags_Type)(DWORD handle,DWORD tags);
    typedef BOOL        (DLLCONV* BASS_ChannelGetInfo_Type)(DWORD handle, BASS_CHANNELINFO* info);
    typedef BOOL        (DLLCONV* BASS_ChannelSetAttribute_Type)(DWORD handle, DWORD attrib, float value);


#ifdef WIN32
#ifdef _UNICODE		
    typedef HPLUGIN(DLLCONV* BASS_PluginLoad_Type)(const WCHAR* file, DWORD flags);
#else
    typedef HPLUGIN(DLLCONV* BASS_PluginLoad_Type)(const char* file, DWORD flags);
#endif
#else
    typedef HPLUGIN(DLLCONV* BASS_PluginLoad_Type)(const char* file, DWORD flags);
#endif

    typedef BOOL    (DLLCONV *BASS_PluginFree_Type)(HPLUGIN handle);
	typedef int 	(DLLCONV *BASS_ErrorGetCode_Type)();


    typedef HSTREAM (DLLCONV* BASS_Mixer_StreamCreate_Type)(DWORD freq, DWORD chans, DWORD flags);
    typedef BOOL    (DLLCONV* BASS_Mixer_StreamAddChannel_Type)(HSTREAM handle, DWORD channel, DWORD flags);
    typedef QWORD   (DLLCONV* BASS_Mixer_ChannelGetPosition_Type)(DWORD handle, DWORD mode);
    typedef BOOL    (DLLCONV* BASS_Mixer_ChannelSetMatrix_Type)(DWORD handle, void* matrix);

    typedef const char* (DLLCONV* BASS_TAGS_Read_Type)(DWORD handle, const char* fmt);
    typedef BOOL(DLLCONV* BASS_TAGS_SetUTF8_Type)(BOOL enable);


    
	static BASS_SetConfig_Type			        _BASS_SetConfig = NULL;
	static BASS_ChannelBytes2Seconds_Type       _BASS_ChannelBytes2Seconds = NULL;
	static BASS_ChannelGetLength_Type           _BASS_ChannelGetLength = NULL;
	static BASS_Init_Type                       _BASS_Init = NULL;
	static BASS_ChannelPlay_Type                _BASS_ChannelPlay= NULL;
    static BASS_ChannelStop_Type			    _BASS_ChannelStop= NULL;
	static BASS_StreamCreateFile_Type		    _BASS_StreamCreateFile = NULL;
    static BASS_Free_Type                       _BASS_Free = NULL;
    static BASS_StreamFree_Type 			    _BASS_StreamFree = NULL;
    static BASS_ChannelGetData_Type			    _BASS_ChannelGetData= NULL;
    static BASS_ChannelIsActive_Type		    _BASS_ChannelIsActive= NULL;
    static BASS_ChannelGetPosition_Type		    _BASS_ChannelGetPosition=NULL;
    static BASS_ChannelGetTags_Type			    _BASS_ChannelGetTags=NULL;
    static BASS_PluginLoad_Type                 _BASS_PluginLoad=NULL;
    static BASS_PluginFree_Type                 _BASS_PluginFree=NULL;
	static BASS_ErrorGetCode_Type			    _BASS_ErrorGetCode = NULL;
    static BASS_Mixer_StreamCreate_Type		    _BASS_Mixer_StreamCreate = NULL;
    static BASS_Mixer_StreamAddChannel_Type     _BASS_Mixer_StreamAddChannel = NULL;
    static BASS_ChannelGetInfo_Type             _BASS_ChannelGetInfo = NULL;
    static BASS_Mixer_ChannelGetPosition_Type   _BASS_Mixer_ChannelGetPosition = NULL;
    static BASS_Mixer_ChannelSetMatrix_Type     _BASS_Mixer_ChannelSetMatrix = NULL;
    static BASS_ChannelSetAttribute_Type        _BASS_ChannelSetAttribute = NULL;
    static BASS_TAGS_Read_Type                  _BASS_TAGS_Read = NULL;
    static BASS_TAGS_SetUTF8_Type               _BASS_TAGS_SetUTF8 = NULL;

	/// Helper class to load the mp3lib dll
	class BassLibLoader
	{
		DllHandle m_hMod, m_hModMix, m_hModtags;
        
       void unloadPlugins(){
           
           //Unload all Plugins, loaded
		  /*
           for(auto&& x: inputPlugins)
               _BASS_PluginFree(x);
		   */
		    	for(std::vector<HPLUGIN>::iterator it = inputPlugins.begin(); it != inputPlugins.end(); it++)
		  	{
		  	 _BASS_PluginFree(*it);
		  }
    
           inputPlugins.clear();
        }

		
		void Unload()
		{
			if( m_hMod )
			{
                unloadPlugins();
				DllUnload(m_hMod);
			}

            if (m_hModMix)
            {
                DllUnload(m_hModMix);
            }

            if (m_hModtags)
            {
                DllUnload(m_hModtags);
            }

			_BASS_SetConfig = 0;
			_BASS_ChannelBytes2Seconds = 0;
			_BASS_ChannelGetLength = 0;
			_BASS_Init = 0;
			_BASS_ChannelPlay = 0;
			_BASS_StreamCreateFile = 0;
            _BASS_Free = 0;
            _BASS_ChannelStop = 0;
            _BASS_StreamFree = 0;
            _BASS_ChannelGetData = 0;
            _BASS_ChannelIsActive = 0;
            _BASS_ChannelGetPosition = 0;
            _BASS_ChannelGetTags=0;
            _BASS_PluginLoad=0;
            _BASS_PluginFree=0;
            _BASS_Mixer_StreamCreate = 0;
            _BASS_Mixer_StreamAddChannel = 0;
            _BASS_ChannelGetInfo = 0;
            _BASS_Mixer_ChannelGetPosition = 0;
            _BASS_Mixer_ChannelSetMatrix = 0;
            _BASS_ChannelSetAttribute = 0;
            _BASS_TAGS_SetUTF8 = 0;
            _BASS_TAGS_Read = 0;

			bLoaded = false;
            mixLoaded = false;
		}

	public:
		BassLibLoader()
		{	
            QString RtnVal, mixRtnVal, tagsRtnVal;

#ifdef WIN32
            RtnVal = "bass.dll";
            mixRtnVal = "bassmix.dll";
            tagsRtnVal = "tags.dll";
#endif

#ifdef LINUX
            RtnVal = "libbass.so";
            mixRtnVal = "libbassmix.so";
            tagsRtnVal = "libtags.so";
            
#endif

#ifdef MAC
            RtnVal = "libbass.dylib";
            mixRtnVal = "libbassmix.dylib";
            tagsRtnVal = "libtags.dylib";
#endif

            m_hMod = DllLoad(convertToFoxValue(RtnVal));

			if (m_hMod)
			{
				if(	
					NULL == (_BASS_SetConfig = (BASS_SetConfig_Type)DllFindSym(m_hMod, "BASS_SetConfig")) ||
					NULL == (_BASS_ChannelBytes2Seconds = (BASS_ChannelBytes2Seconds_Type)DllFindSym(m_hMod, "BASS_ChannelBytes2Seconds")) ||
					NULL == (_BASS_ChannelGetLength = (BASS_ChannelGetLength_Type)DllFindSym(m_hMod, "BASS_ChannelGetLength")) ||
					NULL == (_BASS_Init = (BASS_Init_Type)DllFindSym(m_hMod, "BASS_Init")) ||
                    NULL == (_BASS_StreamFree = (BASS_StreamFree_Type)DllFindSym(m_hMod, "BASS_StreamFree")) ||
                    NULL == (_BASS_ChannelIsActive = (BASS_ChannelIsActive_Type)DllFindSym(m_hMod, "BASS_ChannelIsActive")) ||
                    NULL == (_BASS_PluginLoad = (BASS_PluginLoad_Type)DllFindSym(m_hMod, "BASS_PluginLoad")) ||
                    NULL == (_BASS_PluginFree = (BASS_PluginFree_Type)DllFindSym(m_hMod, "BASS_PluginFree")) ||
                    NULL == (_BASS_ChannelGetData = (BASS_ChannelGetData_Type)DllFindSym(m_hMod, "BASS_ChannelGetData")) ||
                    NULL == (_BASS_ChannelGetTags = (BASS_ChannelGetTags_Type)DllFindSym(m_hMod, "BASS_ChannelGetTags")) ||
                    NULL == (_BASS_ChannelGetPosition = (BASS_ChannelGetPosition_Type)DllFindSym(m_hMod, "BASS_ChannelGetPosition")) ||
					NULL == (_BASS_ErrorGetCode = (BASS_ErrorGetCode_Type)DllFindSym(m_hMod, "BASS_ErrorGetCode")) ||
					NULL == (_BASS_ChannelPlay = (BASS_ChannelPlay_Type)DllFindSym(m_hMod, "BASS_ChannelPlay")) ||
                    NULL == (_BASS_ChannelStop = (BASS_ChannelStop_Type)DllFindSym(m_hMod, "BASS_ChannelStop")) ||
                    NULL == (_BASS_Free = (BASS_Free_Type)DllFindSym(m_hMod, "BASS_Free")) ||
                    NULL == (_BASS_ChannelGetInfo = (BASS_ChannelGetInfo_Type)DllFindSym(m_hMod, "BASS_ChannelGetInfo")) ||
                    NULL == (_BASS_ChannelSetAttribute = (BASS_ChannelSetAttribute_Type)DllFindSym(m_hMod, "BASS_ChannelSetAttribute")) ||
					NULL == (_BASS_StreamCreateFile = (BASS_StreamCreateFile_Type)DllFindSym(m_hMod, "BASS_StreamCreateFile")) )
				{
					Unload();
				}
				else
				{
					bLoaded = true;
                    inputPlugins.clear();
                    
				}
			}
            
            m_hModMix = DllLoad(convertToFoxValue(mixRtnVal));
            if (m_hModMix)
            {
                if (
                    NULL == (_BASS_Mixer_StreamCreate = (BASS_Mixer_StreamCreate_Type)DllFindSym(m_hModMix, "BASS_Mixer_StreamCreate")) ||
                    NULL == (_BASS_Mixer_ChannelGetPosition = (BASS_Mixer_ChannelGetPosition_Type)DllFindSym(m_hModMix, "BASS_Mixer_ChannelGetPosition")) ||
                    NULL == (_BASS_Mixer_ChannelSetMatrix = (BASS_Mixer_ChannelSetMatrix_Type)DllFindSym(m_hModMix, "BASS_Mixer_ChannelSetMatrix")) ||  
                    NULL == (_BASS_Mixer_StreamAddChannel = (BASS_Mixer_StreamAddChannel_Type)DllFindSym(m_hModMix, "BASS_Mixer_StreamAddChannel")))
                {
                    Unload();
                }
                else
                {
                    mixLoaded = true;

                }
            }

            m_hModtags = DllLoad(convertToFoxValue(tagsRtnVal));
            if (m_hModtags)
            {
                if (
                    NULL == (_BASS_TAGS_Read = (BASS_TAGS_Read_Type)DllFindSym(m_hModtags, "TAGS_Read")) ||
                    NULL == (_BASS_TAGS_SetUTF8 = (BASS_TAGS_SetUTF8_Type)DllFindSym(m_hModtags, "TAGS_SetUTF8")))
                {
                    Unload();
                }
                else
                {
                    tagsLoaded = true;

                }
            }
		}
		~BassLibLoader()
		{
			Unload();
		}
		static bool	bLoaded;
        static bool mixLoaded;
        static bool tagsLoaded;
        
        static std::vector<HPLUGIN> inputPlugins;
	};

    bool BassLibLoader::bLoaded = false;
    bool BassLibLoader::mixLoaded = false;
    bool BassLibLoader::tagsLoaded = false;
    
    std::vector<HPLUGIN> BassLibLoader::inputPlugins;
    
    DWORD	g_BassHandle = 0;
    DWORD	g_BassMixHandle = 0;
    

    bool CheckBassLibLoaded()
    {
        static BassLibLoader loader;
        return BassLibLoader::bLoaded;
    }

    bool IsMixerLoaded()
    {
         return BassLibLoader::mixLoaded;
    }

    bool IsTagsLoaded()
    {
        return BassLibLoader::tagsLoaded;
    }

    bool IsLibLoaded()
    {
        return CheckBassLibLoaded();
    }
    
    bool LoadPlugLibrary(const QString& strPlugName){

        
        if (!CheckBassLibLoaded())
        {
            return false;
        }

#ifdef WIN32
        HPLUGIN temp = _BASS_PluginLoad((const WCHAR*)convertToFoxValue(strPlugName), BASS_UNICODE);
#else
        HPLUGIN temp = _BASS_PluginLoad(convertToFoxValue(strPlugName), 0);
#endif
        //int xError = _BASS_ErrorGetCode();

        if(temp){
            	BassLibLoader::inputPlugins.push_back(temp);
           	return true;
        }

	
        return false;
    }

    void StopPlayToCard()
    {

        if (g_BassHandle)
        {
            if (CheckBassLibLoaded())
            {
                _BASS_ChannelStop(g_BassHandle);
                _BASS_StreamFree(g_BassHandle);
                //_BASS_Free();
                
            }
            g_BassHandle = 0;
        }
    }
    
    bool PlayToCard(const QString& strFilePath)
    {
        /*
        DllHandle module_hMod = DllLoad(_T("bassmix.dll"));
        if (!module_hMod) return false;
        (_BASS_Mixer_StreamCreate = (BASS_Mixer_StreamCreate_Type)DllFindSym(module_hMod, "BASS_Mixer_StreamCreate"));
        (_BASS_Mixer_StreamAddChannel = (BASS_Mixer_StreamAddChannel_Type)DllFindSym(module_hMod, "BASS_Mixer_StreamAddChannel"));
        

        bool b2 = _BASS_Init(0, 44100, 0, NULL, NULL);
        g_BassMixHandle = _BASS_Mixer_StreamCreate(44100, 2, BASS_STREAM_DECODE | BASS_MIXER_END);
        g_BassHandle = _BASS_StreamCreateFile(false, (const WCHAR*)strFilePath, 0, 0, BASS_UNICODE | BASS_STREAM_DECODE);
        
        bool b1 = _BASS_Mixer_StreamAddChannel(g_BassMixHandle, g_BassHandle, 0);
        while (1) {
            BYTE buf[50000];
            int got = _BASS_ChannelGetData(g_BassMixHandle, buf, sizeof(buf));
            printf("pos: src=%lld mix=%lld\n", _BASS_ChannelGetPosition(g_BassHandle, BASS_POS_BYTE), _BASS_ChannelGetPosition(g_BassMixHandle, BASS_POS_BYTE));
            if (got < 0) break;
        }
        DllUnload(module_hMod);
        */


        StopPlayToCard();
        
        if (!CheckBassLibLoaded())
        {
            return false;
        }

		if (g_BassHandle)
		{
			_BASS_ChannelStop(g_BassHandle);
			_BASS_StreamFree(g_BassHandle);
			g_BassHandle = 0;
		}
        
		//Init is true or false. So we just check later if allready initialized
		//Anstatt 0 könnte man BASS_DEVICE_STEREO und oder BASS_DEVICE_FREQ
        if (!_BASS_Init(-1,44100, BASS_DEVICE_STEREO,NULL,NULL)) {
			if(_BASS_ErrorGetCode()!=BASS_ERROR_ALREADY) {
                return false;
			}
        }

        
#ifdef WIN32
        g_BassHandle = _BASS_StreamCreateFile(false, (const WCHAR*)convertToFoxValue(strFilePath), 0, 0, BASS_SAMPLE_LOOP | BASS_UNICODE);
#else
        g_BassHandle = _BASS_StreamCreateFile(false, convertToFoxValue(strFilePath), 0, 0, BASS_SAMPLE_LOOP);
#endif
        _BASS_ChannelPlay(g_BassHandle,true);
        
        return true;
    }
    
    //Fill an internal structure of basic Tags so we can Offer them to the user for filling CDTEXT
    //How the sturcture of Tags can be look like?
    
    QStringList TryTags(const QString& strFileName){
        
        QStringList tagsList;
        if (!CheckBassLibLoaded())
        {
            return tagsList;
        }

		if (g_BassHandle)
		{
			_BASS_ChannelStop(g_BassHandle);
			_BASS_StreamFree(g_BassHandle);
			g_BassHandle = 0;
		}
        
		//Init is true or false. So we just check later if allready initialized
		if (!_BASS_Init(-1, 44100, BASS_DEVICE_STEREO, NULL, NULL)) {
			if (_BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
                return tagsList;
			}
		}
        

#ifdef WIN32	
		g_BassHandle = _BASS_StreamCreateFile(false, (const WCHAR*)convertToFoxValue(strFileName), 0, 0, BASS_SAMPLE_LOOP | BASS_UNICODE);
#else
		g_BassHandle = _BASS_StreamCreateFile(false, convertToFoxValue(strFileName), 0, 0, BASS_SAMPLE_LOOP);
#endif

        _BASS_TAGS_SetUTF8(true);
        const char* fmt = "[%TITL|%ARTI|%ALBM|%GNRE|%YEAR|%CMNT|%TRCK|%COMP|%COPY|%SUBT|%AART]";
        const char* getTags = _BASS_TAGS_Read(g_BassHandle, fmt);

        if (*getTags)
        {
            QString tagsString (getTags);
            tagsString.remove('[');
            tagsString.remove(']');
            tagsList = tagsString.split("|");
        }

        _BASS_StreamFree(g_BassHandle);
        g_BassHandle = 0;

        return tagsList;
        
    }
    
    bool GetPrecisePlayTime(const QString& strFileName, double& fPlayTime)
    {
        if(!CheckBassLibLoaded())
            return false;
        
		if (g_BassHandle)
		{
			_BASS_ChannelStop(g_BassHandle);
			_BASS_StreamFree(g_BassHandle);
			g_BassHandle = 0;
		}
        
		//Init is true or false. So we just check later if allready initialized
		if (!_BASS_Init(-1, 44100, BASS_DEVICE_STEREO, NULL, NULL)) {
			if (_BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
				return false;
			}
		}
        

#ifdef WIN32	
		g_BassHandle = _BASS_StreamCreateFile(false, (const WCHAR*)convertToFoxValue(strFileName), 0, 0, BASS_SAMPLE_LOOP | BASS_UNICODE);
#else

		g_BassHandle = _BASS_StreamCreateFile(false, convertToFoxValue(strFileName), 0, 0, BASS_SAMPLE_LOOP);
#endif
        long len = _BASS_ChannelGetLength(g_BassHandle, BASS_POS_BYTE);
        double time = _BASS_ChannelBytes2Seconds(g_BassHandle, len);
        fPlayTime = time;
        _BASS_StreamFree(g_BassHandle);
        g_BassHandle = 0;
        
        
        return true;

    }
    
    bool GetPlayTime(const QString& strFileName, size_t& nPlayTime)
    {
        if(!CheckBassLibLoaded())
            return false;
        
		if (g_BassHandle)
		{
			_BASS_ChannelStop(g_BassHandle);
			_BASS_StreamFree(g_BassHandle);
			g_BassHandle = 0;
		}
        
		//Init is true or false. So we just check later if allready initialized
		if (!_BASS_Init(-1, 44100, BASS_DEVICE_STEREO, NULL, NULL)) {
			if (_BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
				return false;
			}
		}

#ifdef WIN32
		g_BassHandle = _BASS_StreamCreateFile(false, (const WCHAR*)convertToFoxValue(strFileName), 0, 0, BASS_SAMPLE_LOOP | BASS_UNICODE);
#else
		g_BassHandle = _BASS_StreamCreateFile(false, convertToFoxValue(strFileName), 0, 0, BASS_SAMPLE_LOOP);
		int xError = _BASS_ErrorGetCode();
#endif

        long len = _BASS_ChannelGetLength(g_BassHandle, BASS_POS_BYTE);
        double time = _BASS_ChannelBytes2Seconds(g_BassHandle, len);
        nPlayTime = (int)time;
        _BASS_StreamFree(g_BassHandle);
        g_BassHandle = 0;
        
        return true;
    }
    
}
