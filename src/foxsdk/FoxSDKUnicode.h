#pragma once

#ifdef WIN32
	#include <tchar.h>
#else
	#include "tchar.h"
#endif

#ifndef FOpen
#	ifdef UNICODE
#		define FOpen						_wfopen
#	else
#		define FOpen						fopen
#	endif
#endif

#ifndef _FindFirst
#	ifdef UNICODE
#		define _FindFirst					_wfindfirst
#	else
#		define _FindFirst					_findfirst
#	endif
#endif

#ifndef _FindData_t
#	ifdef UNICODE
#		define _FindData_t					_wfinddata_t
#	else
#		define _FindData_t					_finddata_t
#	endif
#endif

#ifndef StrCpy
#	ifdef UNICODE
#		define StrCpy						wcscpy
#	else
#		define StrCpy						strcpy
#	endif
#endif

#ifndef StrCmp
#	ifdef UNICODE
#		define StrCmp						wcscmp
#	else
#		define StrCmp						strcmp
#	endif
#endif

#ifndef StrLen
#	ifdef UNICODE
#		define StrLen						wcslen
#	else
#		define StrLen						strlen
#	endif
#endif

#ifndef StrnSet
#	ifdef UNICODE
#		define StrnSet						wcsnset
#	else
#		define StrnSet						strnset
#	endif
#endif

#ifndef VsPrintf
#	ifdef UNICODE
#		define VsPrintf						vswprintf
#	else
#		define VsPrintf						vsprintf
#	endif
#endif

#ifndef SPrintf
#	ifdef UNICODE
#		define SPrintf						swprintf
#	else
#		define SPrintf						sprintf
#	endif
#endif
