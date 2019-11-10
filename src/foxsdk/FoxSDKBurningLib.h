#ifndef __BS_LIBRARY_H__
#	define __BS_LIBRARY_H__


#	ifndef BS_API
#		define BS_API __declspec(dllimport)
#	endif

#	include "FoxSDKExport.h"


#	ifdef __cplusplus
extern "C" {
#	endif


//#	pragma comment (lib, "mp3DecS.lib")

//// MFC as Shared DLL
// In diesem Fall sind folgende Libraries zu unterdr�cken:
// Debug-Version:	libcmtd.lib;libcmt.lib
// Release-Version:	libcmtd.lib;libcmt.lib
//// Ende (MFC as Shared DLL)

//// MFC as Static Library
// In diesem Fall sind folgende Libraries zu unterdr�cken:
// Debug-Version:	libcmt.lib
// Release-Version:	libcmtd.lib
//// Ende (MFC as Static Library)


#	ifdef __cplusplus
}
#	endif


#endif	// __BS_LIBRARY_H__
