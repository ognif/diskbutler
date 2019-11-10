#ifndef __FOXSDK_DEFINITIONS_H__
#define __FOXSDK_DEFINITIONS_H__
#include "FoxSDKUnicode.h"
#include <time.h>
#ifndef _MSC_VER	
#include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>


#ifdef _MSC_VER
#pragma pack(push, 1) 
#endif

#	ifdef __cplusplus
#include <string.h>
extern "C" {
#	endif


//// needed defines
#ifdef _MSC_VER	
	typedef	__int8				int8;
	typedef __int16				int16;
	typedef __int32				int32;
	typedef __int64				int64;	

	typedef unsigned __int8		uint8;
	typedef unsigned __int16	uint16;
	typedef unsigned __int32	uint32;
	
	#if _MSC_VER >= 1400
		#undef ____stat64
		#define ____stat64 struct _stat64
	#endif
#else
		#undef ____stat64

		#define ____stat64 struct _stat64

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))

	typedef unsigned char			uint8;
	typedef unsigned short			uint16;
	typedef unsigned int			uint32;

	typedef signed char			int8;
	typedef	signed short			int16;
	typedef signed int			int32;
	typedef long long int			int64;

#endif




typedef void*	HSESSION;
typedef void*	HDIR;

#ifndef _MAX_PATH
#	define _MAX_PATH			260
#endif

//// end (needed defines)

//	Padding Definitions
#define INSERT_STRUCTURE_PADDING(nBytes)	\
	int8 x__the_private_padding[(nBytes)];		
		
#define STRUCTURE_PADDING_STRING_DATA	( (4*sizeof(void*) + 32)   +   2 * (_MAX_PATH) * sizeof(TCHAR) )
#define STRUCTURE_PADDING_HUGE			(4*sizeof(void*) + 32 * sizeof(TCHAR) )
#define STRUCTURE_PADDING_BIG			(3*sizeof(void*) + 16)
#define STRUCTURE_PADDING_NORMAL		(2*sizeof(void*) + 8)
#define STRUCTURE_PADDING_SMALL			(sizeof(void*) + 4)

	
//	End of Padding Definitions


//// BOOL
#	define int2bool(nTmp)						(0 != nTmp)
#	define BOOL2bool(bTmp)						(FALSE != bTmp)
#	define BS_BOOL2bool(bTmp)					(BS_FALSE != bTmp)
#	define int2BS_BOOL(nTmp)					((0 != nTmp) ? BS_TRUE : BS_FALSE)

#	define BS_NDEF								-1							// -> BS_NDEF is interpreted as BS_TRUE.
#	define BS_BOOL								int8
#	define BS_LP_BOOL							int8*
#	define BS_FALSE								0
#	define BS_TRUE								1
//// end (BOOL)


//// default values
#	define BS_DEFAULT_LANGUAGE_FILE				_T("Burning.lng")
#	define BS_DEFAULT_REGISTRY_KEY				_T("HKEY_CURRENT_USER")
#	define BS_DEFAULT_REGISTRY_PATH				_T("\\Software\\FoxBurnerSDK\\")
//// end (default values)


//// callbacks
	//// info levels
#	define BS_IL_INFO							0
#	define BS_IL_LOW_DEBUG						1
#	define BS_IL_MEDIUM_DEBUG					2
#	define BS_IL_HIGH_DEBUG						3
	//// end (info levels)


	//// structs
		//// aspectratios
#		define BS_AR_SQUARE_PIXELS			    0
#		define BS_AR_4TO3_DISPLAY			    1
#		define BS_AR_16TO9_DISPLAY			    2
#		define BS_AR_221TO2_DISPLAY				3
#		define BS_AR_UNKNOWN				    4
		//// end (aspectratios)


	struct SVideoFormat
	{
		BS_BOOL bUseable;
		int32 nWidth;
		int32 nHeight;
		int32 nBitRate;
		double fFPS;
		int32 nAspectRatio;
		int32 nPlaytime;

        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL - sizeof(int32))

#	ifdef __cplusplus
		SVideoFormat(void)
		{
			bUseable		= BS_NDEF;
			nWidth			= BS_NDEF;
			nHeight			= BS_NDEF;
			nBitRate		= BS_NDEF;
			fFPS			= BS_NDEF;
			nAspectRatio	= BS_NDEF;
			nPlaytime		= BS_NDEF;
		}
#	endif
	};

	struct SFileDateTime
	{
		uint8 nYear;	// Years since 1900
		uint8 nMonth;	// Month [1, 12]
		uint8 nDay;		// Day of month [1, 31]
		uint8 nHour;	// Hours [0, 23]
		uint8 nMinute;	// Minutes [0, 59]
		uint8 nSecond;	// Seconds [0, 59]
					
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
		
	};

	struct SSpeed
	{
		float fSpeed;
		int   nSpeedInKBPerSec;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
		
#   ifdef __cplusplus
		SSpeed(void)
		{
			fSpeed          = BS_NDEF;
			nSpeedInKBPerSec= BS_NDEF;
		}
#   endif
	};
	//// end (structs)

	struct SDRMLicenseInfo
	{
		const TCHAR*	szLicenseURL;
		const TCHAR*	szPostData;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL)
				
#	ifdef __cplusplus
		SDRMLicenseInfo(void)
		{
			szLicenseURL = NULL;
			szPostData = NULL;
		}
#	endif
	};

#define BS_DRM_EVENT_UNTRUSTED_URL			1
#define BS_DRM_EVENT_TAMPERED_URL			2
#define BS_DRM_EVENT_CORRUPTED_URL			3
#define BS_DRM_EVENT_ACQUISITION_TYPE		4
#define BS_DRM_EVENT_LICENSE_ACQUIRED		5
#define BS_DRM_EVENT_LICENSE_NOT_ACQUIRED	6
#define BS_DRM_EVENT_NEED_INDIVIDUALIZATION	7

#define BS_DRM_LICENSE_NONSILENT			1
#define BS_DRM_LICENSE_SILENT				2
#define BS_DRM_LICENSE_ABORT				3

struct SFileEntry;

	//// definitions
	typedef int32	(*GetTextEvent)						(int32 nTextID, TCHAR *pchText, int32 *pnLength, void *pUserData);
	typedef void	(*InfoTextEvent)					(const TCHAR *pcInfoText, int32 nLevel, void *pUserData);
	typedef void	(*ProcessEvent)						(float fPercent, float fDeviceBuffer, float fCache, double dBytesWritten, double dImageSize, void *pUserData);
	typedef void    (*BurnFileEvent)					(const TCHAR *pcFileName, void *pUserData);
	typedef void	(*EraseDoneEvent)					(const TCHAR *pcError, void *pUserData);
	typedef void	(*FinalizeEvent)					(void *pUserData);
	typedef void	(*BurnDoneEvent)					(const TCHAR *pcError, void *pUserData);
	typedef void	(*JobDoneEvent)						(void* pUserData);
	typedef void	(*AddFileEvent)						(const TCHAR *pcFullPath, const TCHAR *pcJolietName, const TCHAR *pcISOName, const TCHAR* pcUDFName , double dDateTime, double dFileSize, void *pUserData);
	typedef void	(*CreateDirEvent)					(const TCHAR *pcFullName, const TCHAR *pcJolietName, const TCHAR* pcISOName, const TCHAR* pcUDFName, void *pUserData);
	typedef void	(*RemoveFileEvent)					(const TCHAR *pcFullPath, const TCHAR *pcDestinationPath, const TCHAR *pcFile, void *pUserData);
	typedef void	(*AudioDecoderEvent)				(float fPercent, const TCHAR *pcFileName, int32 nType, void *pUserData);
	typedef void	(*AudioDecodeDoneEvent)				(const TCHAR *pcFileName, const TCHAR *pcError, int32 nErrorCode, void *pUserData);
	typedef void	(*VideoScannerEvent)				(float fPercent, const TCHAR *pcFileName, void *pUserData);
	typedef void	(*VideoScanDoneEvent)				(const TCHAR *pcFileName, const TCHAR *pcError, int32 nErrorCode, struct SVideoFormat CurrentFormat, void *pUserData);
	typedef void	(*StartVerifyEvent)					(void *pUserData);
	typedef void	(*VerifyFileEvent)					(const TCHAR *pcFileName, void *pUserData);
	typedef void	(*VerifySectorEvent)				(double nSector, double tSector, bool nSuccess, void *pUserData);
	typedef void	(*VerifyErrorEvent)					(const TCHAR *pcFileName, const TCHAR *pcError, void *pUserData);
	typedef void	(*VerifyDoneEvent)					(int32 nNumErrors, void *pUserData);
	typedef int32   (*RAWDataEvent)						(int32 nLBA, void* pBuffer, int32 nBufferSize, int32* nReaded, void* pUserData);
	typedef BS_BOOL	(*CompareFilesForArrangementEvent)	(const SFileEntry *pFile1, const SFileEntry *pFile2, void *pUserData);
	//// end (definitions)
//// end (callbacks)

/// communication interfaces
#		define BS_ASPI_INTERNAL					0
#		define BS_ASPI_WNASPI					1
#		define BS_ASPI_FROGASPI					2
///

//// device / medium
	//// structs
		//// MediumStatus
#		define BS_MS_EMPTY_DISK					0
#		define BS_MS_INCOMPLETE_DISK			1
#		define BS_MS_COMPLETE_DISK				2
#		define BS_MS_OTHER						3
		//// end (MediumStatus)


		//// LastSession
#		define BS_LS_EMPTY_SESSION				0
#		define BS_LS_INCOMPLETE_SESSION			1
#		define BS_LS_DAMAGED_SESSION			2
#		define BS_LS_COMPLETE_SESSION			3
		//// end (LastSession)

		//// File system type flags
#		define BS_FS_UNKNOWN					0
#		define BS_FS_ISO9660					1
#		define BS_FS_JOLIET						2
#		define BS_FS_UDF						4
#		define BS_FS_BOOTABLE					8
#		define BS_FS_ROCKRIDGE				   16
		//// end (File system)

		//// Track format types
#		define BS_TF_AUDIO						0
#		define BS_TF_DATA_MODE1					1
#		define BS_TF_DATA_MODE2					2
		//// end (Track format)

		//// Extended medium types
#		define BS_EMT_CD_ROM					0
#		define BS_EMT_CD_ROM_XA					1
#		define BS_EMT_CD_AUDIO					2
#		define BS_EMT_CD_MIXED_MODE				3
#		define BS_EMT_CD_ENHANCED				4
#		define BS_EMT_CD_MULTISESSION			5
#		define BS_EMT_DVD						6
#		define BS_EMT_BD						7
#		define BS_EMT_HDDVD						8
		//// end (Medium types)

		//// File formats for SaveTrack
#		define BS_FF_WAVE						0
#		define BS_FF_ISO						1
#		define BS_FF_BIN						2
#		define BS_FF_MPEG						3


		//// CD-Text Content Items
#		define BS_CDTCI_ARRANGER				0
#		define BS_CDTCI_COMPOSER				1
#		define BS_CDTCI_TITLE					2
#		define BS_CDTCI_MESSAGE					3
#		define BS_CDTCI_PERFORMER				4
#		define BS_CDTCI_SONG_WRITER				5

//// FreeDB / CDDB tags content items
#		define BS_TCI_CATEGORY					0
#		define BS_TCI_DISKID					1
#		define BS_TCI_ARTIST					2
#		define BS_TCI_TITLE						3
#		define BS_TCI_SUBMITTED_VIA				4
#		define BS_TCI_GENRE						5
#		define BS_TCI_EXTENDED_INFO				6
#		define BS_TCI_DISK_LENGTH				7
#		define BS_TCI_FRAME_OFFSET				8
#		define BS_TCI_REVISION					9
#		define BS_TCI_YEAR						10

		struct SMediumInfo
		{
			TCHAR chMediumType[25];
			int32 nMediumStatus;
			double dMediumSize;
			double dMediumUsedSize;
			double dMediumFreeSize;
			int32 nFirstSession;
			int32 nLastSession;
			int32 nFirstTrack;
			int32 nLastTrack;
			int32 nLastSessionStatus;
			int16 wMediumTypeCode;
			int16 nExtendedMediumType;

			/// 13 digit catalog code (NULL terminated)
			TCHAR chUPCEANCode[14];

			/// Identifier of the disc manufacturer.
			TCHAR chVendorID[20];
			/// Max supported write speed declared by manufacturer.
			float fMaxWriteSpeed;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_HUGE - sizeof(TCHAR [20]/*chVendorID*/) - sizeof(float/*fMaxWriteSpeed*/))

#	ifdef __cplusplus
			SMediumInfo(void)
			{
				StrCpy(chMediumType, _T("BS_NDEF"));
				nMediumStatus		= BS_NDEF;
				dMediumSize			= BS_NDEF;
				dMediumUsedSize		= BS_NDEF;
				dMediumFreeSize		= BS_NDEF;
				nFirstSession		= BS_NDEF;
				nLastSession		= BS_NDEF;
				nFirstTrack         = BS_NDEF;
				nLastTrack          = BS_NDEF;
				nLastSessionStatus	= BS_NDEF;
				wMediumTypeCode     = BS_NDEF;
				nExtendedMediumType = BS_NDEF;
				fMaxWriteSpeed		= BS_NDEF;
				StrCpy(chUPCEANCode, _T("BS_NDEF"));
				StrCpy(chVendorID, _T("BS_NDEF"));
			}
		
#	endif
		};

#		define BS_TM_NOT_APPLICABLE	0
#		define BS_TM_UNKNOWN		1
#		define BS_TM_PIO			2
#		define BS_TM_DMA			3

		struct SExtendedDeviceInformation
		{
			// All strings are null-terminated
			TCHAR	lpszName[_MAX_PATH];
			TCHAR	lpszRevision[_MAX_PATH];
			int32	nRegionCode;
			int32	nRegionCodeChangesLeft;
			TCHAR	lpszLoaderType[_MAX_PATH];
			TCHAR	lpszConnectionInterface[_MAX_PATH];
			TCHAR	lpszPhysicalInterface[_MAX_PATH];
			int32	nNumberOfVolumeLevels;
			int32	nBufferSize;
			TCHAR	lpszSerialNumber[_MAX_PATH];
			
			int32	nReadRetryCount;

			int32	nRegionCodeVendorResetsLeft;
			time_t	FirmwareCreationDate;

			int32	nIdeTransferMode;
			
			//Add new fields here, subtracting their size from the padding amount.
			//Use ( .. - sizeof(the_name_of_the_variable)) syntax.
			//Don't use the type of the variable instead.
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA - sizeof(int32/*nRegionCodeVendorResetsLeft*/) - sizeof(time_t/*FirmwareCreationDate*/) - sizeof(int32/*nIdeTransferMode*/))
			
				
			
			

#	ifdef __cplusplus
			SExtendedDeviceInformation()
			{
				memset(this, 0, sizeof(*this));
			}
#	endif
		};

		struct SSessionInfo
		{
			/// Session size in sectors
			long dSessionSize;
			/// Is it last disk's session
			BS_BOOL bLastSession;
			/// Address of first session's sector
			long dStartLBA;
			/// Number of first track of session
			long nFirstTrack;
			/// Number of last track of session
			long nLastTrack;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_BIG)

#	ifdef __cplusplus
			SSessionInfo(void)
			{
				dSessionSize = BS_NDEF;
				bLastSession = BS_NDEF;
				dStartLBA	 = BS_NDEF;
				nFirstTrack	 = BS_NDEF;
				nLastTrack	 = BS_NDEF;
			}
#	endif
		};

		struct STrackInfo
		{
			/// Track number
			int16	nTrackNumber;
			/// Session number of track
			int16	nSessionNumber;
			/// Track's start address
			int32	nStartLBA;
			/// Track size
			int32	nSize;
			/// One of BS_TF_* values
			int8	nFormat;
			/// Combination of BS_FS_* flags
			int8	nFileSystem;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL)

#	ifdef __cplusplus
			STrackInfo(void)
			{
				nTrackNumber   = BS_NDEF;
				nSessionNumber = BS_NDEF;
				nStartLBA	   = BS_NDEF;
				nSize          = BS_NDEF;
				nFormat		   = BS_NDEF;
				nFileSystem    = BS_NDEF;
			}
#	endif
		};

	//// end (medium / session information)


/* Device capabilities list */
#	define BS_READ_CDR							0x000000000001LL
#	define BS_READ_CDRW							0x000000000002LL
#	define BS_READ_DVD							0x000000000004LL
#	define BS_READ_DVDR							0x000000000008LL
#	define BS_READ_DVDRW						0x000000000010LL
#	define BS_READ_DVDRAM						0x000000000020LL
#	define BS_READ_DVDR_PLUS					0x000000000040LL
#	define BS_READ_DVDRW_PLUS					0x000000000080LL
#   define BS_READ_DVD_DL						0x000000000100LL
#	define BS_READ_DVD_MRDL						0x000000000200LL
#	define BS_READ_BLURAY_R						0x000000000400LL
#	define BS_READ_BLURAY_RE					0x000000000800LL
#	define BS_READ_BLURAY_ROM					0x000000001000LL
#	define BS_READ_HDDVD_R						0x000000002000LL
#	define BS_READ_HDDVD_RW						0x000000004000LL
#	define BS_READ_HDDVD_ROM					0x000000008000LL
#	define BS_READ_MOUNT_RAINER					0x000000020000LL
#	define BS_READ_CDRW_CAV						0x000000040000LL
#	define BS_WRITE_CDR							0x000000080000LL
#	define BS_WRITE_CDRW						0x000000100000LL
#	define BS_WRITE_DVDR						0x000000200000LL
#	define BS_WRITE_DVDRW						0x000000400000LL
#	define BS_WRITE_DVDRAM						0x000000800000LL
#	define BS_WRITE_DVDR_PLUS					0x000001000000LL
#	define BS_WRITE_DVDRW_PLUS					0x000002000000LL
#   define BS_WRITE_DVD_DL						0x000004000000LL
#	define BS_WRITE_DVD_MRDL					0x000008000000LL
#	define BS_WRITE_BLURAY_R					0x000010000000LL
#	define BS_WRITE_BLURAY_RE					0x000020000000LL
#	define BS_WRITE_HDDVD_R						0x000040000000LL
#	define BS_WRITE_HDDVD_RW					0x000080000000LL
#	define BS_WRITE_MOUNT_RAINER				0x000400000000LL
#	define BS_WRITE_CDRW_CAV					0x000800000000LL
#	define BS_WRITE_TEST						0x001000000000LL
#	define BS_UNDERRUN_PROTECTION				0x002000000000LL
#	define BS_SMART								0x004000000000LL
#	define BS_MULTISESSION						0x008000000000LL
#	define BS_CPRMAUTH							0x010000000000LL
#	define BS_DEFECTMANAGEMENT					0x020000000000LL
#	define BS_STREAMING							0x040000000000LL	
#	define BS_READ_DVD_RDL_PLUS					0x080000000000LL
#	define BS_READ_DVD_RWDL_PLUS				0x100000000000LL
#	define BS_WRITE_DVD_RDL_PLUS				0x200000000000LL
#	define BS_WRITE_DVD_RWDL_PLUS				0x400000000000LL
#	define BS_LAYER_JUMP_RECORDING				0x800000000000LL
// WARNING: Values below are not to be used with obsolete GetCapabilities() function.
//			Also they are not to be used in bitwise logic operations.
#	define BS_ANALOG_AUDIO_PLAYBACK					0xFFFF00000001LL
#	define BS_COMPOSITE_AUDIO_AND_VIDEO				0xFFFF00000002LL
#	define BS_DIGITAL_PORT_1						0xFFFF00000003LL
#	define BS_DIGITAL_PORT_2						0xFFFF00000004LL
#	define BS_MODE2_FORM1_READ						0xFFFF00000005LL
#	define BS_MODE2_FORM2_READ						0xFFFF00000006LL
#	define BS_CDDA_COMMANDS							0xFFFF00000007LL
#	define BS_CDDA_STREAM_IS_ACCURATE				0xFFFF00000008LL
#	define BS_R_W_SUBCHANNELS_READ					0xFFFF00000009LL
#	define BS_R_W_SUBCHANNELS_DEINT_AND_CORR		0xFFFF0000000ALL
#	define BS_C2_POINTERS							0xFFFF0000000BLL
#	define BS_ISRC_READ								0xFFFF0000000CLL
#	define BS_UPC_READ								0xFFFF0000000DLL
#	define BS_BARCODE_READ							0xFFFF0000000ELL
#	define BS_LOCK_MEDIA							0xFFFF0000000FLL
#	define BS_LOCK_STATE							0xFFFF00000010LL
#	define BS_PREVENT_JUMPER						0xFFFF00000011LL
#	define BS_EJECT_INDIVIDUAL_OR_MAGAZINE			0xFFFF00000012LL
#	define BS_SEPARATE_VOLUME_LEVELS				0xFFFF00000013LL
#	define BS_SEPARATE_CHANNEL_MUTE					0xFFFF00000014LL
#	define BS_CHANGER_SUPPORTS_DISC_PRESENT			0xFFFF00000015LL
#	define BS_CHANGER_SOFTWARE_SLOT_SELECTION		0xFFFF00000016LL
#	define BS_CHANGER_SIDE_CHANGE_CAPABLE			0xFFFF00000017LL
#	define BS_R_W_SUBCHANNELS_IN_LEAD_IN_READ		0xFFFF00000018LL
#	define BS_METHOD_2_ADDRESSING_FIXED_PACKETS		0xFFFF00000019LL
#	define BS_CD_TEXT_READ							0xFFFF0000001ALL
#	define BS_CD_TEXT_WRITE							0xFFFF0000001BLL
#	define BS_DAO_RAW								0xFFFF0000001CLL
#	define BS_DAO_16								0xFFFF0000001DLL
#	define BS_DAO_96_PACK							0xFFFF0000001ELL
#	define BS_DAO_96_RAW							0xFFFF0000001FLL
#	define BS_PACKET_WRITE							0xFFFF00000020LL
#	define BS_LABELFLASH							0xFFFF00000021LL
#	define BS_LIGHTSCRIBE							0xFFFF00000022LL
#	define BS_READ_BLURAY_R_XL						0xFFFF00000023LL
#	define BS_READ_BLURAY_RE_XL						0xFFFF00000024LL
#	define BS_WRITE_BLURAY_R_XL						0xFFFF00000025LL
#	define BS_WRITE_BLURAY_RE_XL					0xFFFF00000026LL

		//// end (device capabilities)


		//// mediumtype
#   define BS_UNKNOWN    	0
#   define BS_CD_ROM     	1
#   define BS_CD_R       	2
#   define BS_CD_RW      	3
#   define BS_DVD_ROM    	4
#   define BS_DVD_R      	5
#   define BS_DVD_RAM    	6
#   define BS_DVD_RW_RO  	7
#   define BS_DVD_RW     	8
#   define BS_DVD_RW_SR  	9
#   define BS_DVD_PLUSRW 	10
#   define BS_DVD_PLUSR  	11
#   define BS_DDCD_ROM   	12
#   define BS_DDCD_R     	13
#   define BS_DDCD_RW    	14
# 	define BS_DVD_RDL_PLUS 	15
# 	define BS_DVD_RWDL_PLUS 16
# 	define BS_DVD_MRDL	 	17
#	define BS_BLURAY_R		18
#	define BS_BLURAY_RE		19
#	define BS_BLURAY_ROM	20
#	define BS_BLURAY_R_RRM	21
#	define BS_HD_DVD_R		22
#	define BS_HD_DVD_RW		23
#	define BS_HD_DVD_ROM	24
#	define BS_HD_DVD_RAM	25
#	define BS_HD_DVD_R_DL	26
#	define BS_HD_DVD_RW_DL	27

		//// end (mediumtype)


	//// speed
#	define BS_MAX_SPEED							0
	//// end (speed)
//// end (device / medium)

//// Audio
	//// types
#	define BS_AUDIO_NO							0
#	define BS_AUDIO_PCM							1
#	define BS_AUDIO_MP3							2
#	define BS_AUDIO_OGG							3
	//// end (types)
//// end (project)

//// project
	//// types
#	define BS_BURNER_AUDIO						0
#	define BS_BURNER_CUE						1
#	define BS_BURNER_DATA						2
#	define BS_BURNER_VCD						3
#	define BS_BURNER_SVCD						4
#	define BS_BURNER_VIDEODVD					5
#	define BS_BURNER_UDFDVD						6
#	define BS_BURNER_ISOUDF						7
#	define BS_BURNER_BLURAY						8
#	define BS_BURNER_MIXEDMODE					9
#	define BS_BURNER_RAW						10
	//// end (types)
//// end (project)

//// UDF
	//// versions
#	define BS_UDF_VERSION_102						0x102
#	define BS_UDF_VERSION_150						0x150
#	define BS_UDF_VERSION_200						0x200
#	define BS_UDF_VERSION_201						0x201
#	define BS_UDF_VERSION_250						0x250
#	define BS_UDF_VERSION_260						0x260
	//// end(versions)

	//// partitions
#	define BS_UDF_PARTITION_PHYSICAL			0
#	define BS_UDF_PARTITION_VIRTUAL				1
#	define BS_UDF_PARTITION_SPARABLE			2
	//// end(partitions)
//// end(UDF)

//// write methods
#	define BS_WM_TAO							0
#	define BS_WM_DAO							1
#	define BS_WM_DAO96							2
//// end(write methods)

//// read mode
#	define BS_RM_USERDATA						0
#	define BS_RM_RAW							1
#	define BS_RM_RAW_SUBCHANNEL					2
//// end(write methods)

//// default device index
#	define BS_CURRENT_DEVICE					(-1)
#	define BS_BURN_DEVICE						BS_CURRENT_DEVICE
#	define BS_READ_DEVICE						(-2)
//// default device index


//// file handling
	//// structs
		//// SavePath
		#define BS_DONT_SAVE_PATH			    0
		#define BS_WHOLE_PATH				    1
		#define BS_PARENTDIR_ONLY			    2
		//// end (SavePath)


		//// FileAttributes
		#define BS_FA_READONLY					0x00000001
		#define BS_FA_HIDDEN					0x00000002
		#define BS_FA_SYSTEM					0x00000004
		#define BS_FA_DIRECTORY					0x00000010
		#define BS_FA_ARCHIVE					0x00000020
		#define BS_FA_ADVANCED_HIDDEN			0x00000040
		#define BS_FA_ALL						0x0000007F
		//// end (FileAttributes)


		struct SDirToCreate
		{
			const TCHAR* lpszDir;
			const TCHAR* lpszDestinationPath;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)

#	ifdef __cplusplus
			SDirToCreate(void)
				:lpszDir(0)
				,lpszDestinationPath(0)
			{
			}
#	endif
		};

		struct SDirToAdd
		{
			const TCHAR* lpszSourceDirPath;
			const TCHAR* lpszDestinationPath;
			const TCHAR* lpszFileSpecification;
			int32 nFileAttributes;
			BS_BOOL bRecursive;
			int32 nSavePath;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL)
			
#	ifdef __cplusplus
			SDirToAdd(void)
				:lpszSourceDirPath(0)
				,lpszDestinationPath(0)
				,lpszFileSpecification(0)
				,nFileAttributes(BS_FA_ALL)
				,bRecursive(BS_TRUE)
				,nSavePath(BS_PARENTDIR_ONLY)
			{
			}
#	endif
		};

		struct SDirToRemove
		{
			const TCHAR* lpszDestinationPath;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
	
#	ifdef __cplusplus
			SDirToRemove(void)
				:lpszDestinationPath(0)
			{
			}
#	endif
		};

		struct SFileToAdd
		{
			const TCHAR* lpszSourceFilePath;
			const TCHAR* lpszDestinationPath;
			const TCHAR* lpszFileName;
			BS_BOOL bVideoFile;
			int32 nSavePath;

            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
			
#	ifdef __cplusplus
			SFileToAdd(void)
				:lpszSourceFilePath(0)
				,lpszDestinationPath(0)
				,lpszFileName(0)
				,bVideoFile(BS_FALSE)
				,nSavePath(BS_DONT_SAVE_PATH)
			{
			}
#	endif
		};

		struct SFileToAddEx
		{
			const TCHAR* lpszSourceFilePath;
			const TCHAR* lpszDestinationPath;
			const TCHAR* lpszFileName;
			int32 nSavePath;
			int64 nFileOffset;
			int64 nSegmentSize;

            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
			
#	ifdef __cplusplus
			SFileToAddEx(void)
				:lpszSourceFilePath(0)
				,lpszDestinationPath(0)
				,lpszFileName(0)
				,nSavePath(BS_DONT_SAVE_PATH)
				,nFileOffset(0)
				,nSegmentSize(-1)
			{
			}
#	endif
		};

		struct SFileAudioProperty
		{
			const TCHAR* lpszSourceFilePath;
			const TCHAR* lpszTitle;
			const TCHAR* Performer;
			const TCHAR* SongWriter;
			const TCHAR* Composer;
			const TCHAR* Arranger;
			const TCHAR* Message;
			const TCHAR* lpszMCN_ISRC;
			int	nPause;					//Depending on bPauseInFrames if Frames or in Seconds. 
			const int32* pIndexes;
			int32		 nIndexesLength;
			BS_BOOL		bPauseInFrames;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_BIG - sizeof(BS_BOOL))
			
#	ifdef __cplusplus
			SFileAudioProperty(void)
				:lpszSourceFilePath(NULL)
				,lpszTitle(NULL)
				,Performer(NULL)
				,SongWriter(NULL)
				,Composer(NULL)
				,Arranger(NULL)
				,Message(NULL)
				,lpszMCN_ISRC(NULL)
				,nPause(2)
				,pIndexes(NULL)
				,nIndexesLength(0)
				,bPauseInFrames(BS_FALSE)
			{};
#	endif
		};

		struct SFileToRemove
		{
			const TCHAR* lpszFile;
			const TCHAR* lpszDestinationPath;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
			
#	ifdef __cplusplus
			SFileToRemove(void)
				:lpszFile(0)
				,lpszDestinationPath(0)
			{
			}
#	endif
		};

		struct SFileToRename
		{
			const TCHAR* lpszSourcePath;
			const TCHAR* lpszDestinationPath;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
			
#	ifdef __cplusplus
			SFileToRename(void)
				:lpszSourcePath(0)
				,lpszDestinationPath(0)
			{
			}
#	endif
		};

		struct SDirToRename
		{
			const TCHAR* lpszSourcePath;
			const TCHAR* lpszDestinationPath;
			
            INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)
			
#	ifdef __cplusplus
			SDirToRename(void)
				:lpszSourcePath(0)
				,lpszDestinationPath(0)
			{
			}
#	endif
		};
	//// end (structs)
//// end (file handling)


/// RAW writing definitions

	/// Track formats
	#	define BS_RTF_AUDIO				0
	#	define BS_RTF_MODE1				1
	#	define BS_RTF_MODE2_FORMLESS	2
	#	define BS_RTF_MODE2_FORM1		3
	#	define BS_RTF_MODE2_FORM2		4
	/// end (Track formats)

	/// Data type flags
	#	define BS_RDT_SYNC_HEADER	0x0001	// 16 bytes
	#	define BS_RDT_SUBHEADERS	0x0002	// 8 bytes
	#	define BS_RDT_DATA			0x0004	// depends on track type
	#	define BS_RDT_EDC_ECC		0x0008	// 0/288/0/280/4 bytes depend on track type
	#	define BS_RDT_SUBCH_PQ		0x0010	// 16 bytes
	#	define BS_RDT_SUBCH_PW		0x0020	// 96 bytes
	#   define BS_RDT_SUBCH_RW		0x0040  // 96 bytes
	/// end (Data type flags)


	struct SRAWTrackToAdd
	{
		int32	nNumber;			// 0 - Lead-in, 0xAA - lead-out
		int32	nIndex;				// 0 - pre-gap, 1..99 - user data
		int32	nFormat;			// BS_RTF_* value
		int32	nDataTypeMask;		// BS_RDT_* mask
		int32	nIgnoreDataMask;	// BS_RDT_* mask
		int32	nStartAddress;
		int32	nLength;
		int64	nOffset;			// Offset in image in bytes
			
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL)
		
#ifdef __cplusplus
		SRAWTrackToAdd()
			:nNumber(1)
			,nIndex(1)
			,nFormat(BS_RTF_MODE1)
			,nDataTypeMask(BS_RDT_DATA)
			,nIgnoreDataMask(0)
			,nStartAddress(0)
			,nLength(0)
			,nOffset(0)
		{
		}
#endif
	};
/// end (RAW writing definitions)


//// burning options
	//// SessionToContinue
	#define BS_CONTINUE_NO_SESSION			-1
	#define BS_CONTINUE_LAST_SESSION		0
	//// end (SessionToContinue)

#define BS_ISO_LEVEL_1		1
#define BS_ISO_LEVEL_2		2
#define BS_ISO_LEVEL_3		3
#define	BS_ISO_LEVEL_ROMEO	4

    struct SISOInfoEx
	{
		TCHAR ISOAbstractFileIdentifier[36];
		TCHAR ISOApplicationIdentifier[128];
		TCHAR ISOBiblioIdentifier[36];
		TCHAR ISOCopyrightFileIdentifier[36];
		TCHAR ISODataPreparerIdentifier[128];
		TCHAR ISOPublisherIdentifier[128];
		TCHAR ISOSetIdentifier[128];
		TCHAR ISOSystemIdentifier[31];
		BS_BOOL ISOAddSuffix;
		
		int32	ISOLevel;
		BS_BOOL ISOAllowManyDirectories;
		BS_BOOL	ISOAllowLowercaseNames;
		BS_BOOL	ISOAllowLongISO9660Names;
		BS_BOOL ISOAllowLongJolietNames;

		BS_BOOL	ISOUseCreationDateTime;
		SFileDateTime ISOCreationDateTime;
		
		BS_BOOL ISOUseModificationDateTime;
		SFileDateTime ISOModificationDateTime;
		
		BS_BOOL	ISOUseExpirationDateTime;
		SFileDateTime ISOExpirationDateTime;
		
		BS_BOOL ISOUseEffectiveDateTime;
		SFileDateTime ISOEffectiveDateTime;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA - 6*sizeof(BS_BOOL) - 4*sizeof(SFileDateTime))
		
#	ifdef __cplusplus
			SISOInfoEx(void)
				:ISOAddSuffix(BS_TRUE)
				,ISOLevel(BS_ISO_LEVEL_1)
				,ISOAllowManyDirectories(BS_FALSE)
				,ISOAllowLowercaseNames(BS_FALSE)
				,ISOAllowLongISO9660Names(BS_FALSE)
				,ISOAllowLongJolietNames(BS_FALSE)
				,ISOUseCreationDateTime(BS_FALSE)
				,ISOUseModificationDateTime(BS_FALSE)
				,ISOUseExpirationDateTime(BS_FALSE)
				,ISOUseEffectiveDateTime(BS_FALSE)
			{
				StrCpy(ISOAbstractFileIdentifier, _T("BS_NDEF"));
				StrCpy(ISOApplicationIdentifier, _T("BS_NDEF"));
				StrCpy(ISOBiblioIdentifier, _T("BS_NDEF"));
				StrCpy(ISOCopyrightFileIdentifier, _T("BS_NDEF"));
				StrCpy(ISODataPreparerIdentifier, _T("BS_NDEF"));
				StrCpy(ISOPublisherIdentifier, _T("BS_NDEF"));
				StrCpy(ISOSetIdentifier, _T("BS_NDEF"));
				StrCpy(ISOSystemIdentifier, _T("BS_NDEF"));
			}
#	endif
	};

	struct SUDFOptions
	{
		int32	Version;
		int32	PartitionType;
		BS_BOOL	WriteFileStreams;

		BS_BOOL	AvchdDisc;

        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL - sizeof(BS_BOOL/*AvchdDisc*/))

#	ifdef __cplusplus
		SUDFOptions(void)
			:Version(BS_UDF_VERSION_102)
			,PartitionType(BS_UDF_PARTITION_PHYSICAL)
			,WriteFileStreams(BS_FALSE)
			,AvchdDisc(BS_FALSE)
		{
		}
#	endif
	};

	#define BS_UDFOPT_VERSION			0x00000001	// SUDFOptionsEx.Version is valid
	#define BS_UDFOPT_PARTITION_TYPE	0x00000002	// SUDFOptionsEx.PartitionType is valid
	#define BS_UDFOPT_FILE_STREAMS		0x00000004	// SUDFOptionsEx.WriteFileStreams is valid
	#define BS_UDFOPT_AVCHD_DISC		0x00000008	// SUDFOptionsEx.AvchdDisc is valid
	#define BS_UDFOPT_IMPLEMENTATION_ID	0x00000010	// SUDFOptionsEx.ImplementationID is valid
	#define BS_UDFOPT_ALL				0xFFFFFFFF

	struct SUDFOptionsEx
	{
		int32	StructSize;
		int32	Mask;
		int32	Version;
		int32	PartitionType;
		BS_BOOL	WriteFileStreams;
		BS_BOOL	AvchdDisc;
		char	ImplementationID[23];
	};

	struct SDVDVideoOptions
	{
		BS_BOOL Padding;
		BS_BOOL ForceUppercase;
			
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL)

#ifdef __cplusplus
		SDVDVideoOptions()
			:Padding(BS_FALSE)
			,ForceUppercase(BS_TRUE)
		{						
		}
#endif
	};

    struct SBootInfoEx
	{
		TCHAR DeveloperID[24];
		BS_BOOL BootIndicator;
		int32 LoadSegment;
		int32 PlatformID;
		int32 Emulation;
		int32 SectorCount;
			
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_HUGE)

#	ifdef __cplusplus
			SBootInfoEx(void)
			{
				StrCpy(DeveloperID, _T("BS_NDEF"));
				BootIndicator = BS_NDEF;
				LoadSegment = 1984;
				PlatformID = 0;
				Emulation = 2;
				SectorCount = 1;
			}
#	endif
	};
    
    struct STags
    {
        TCHAR chArtist[128];
        TCHAR chAlbum[128];
        TCHAR chTitle[128];
        TCHAR chComment[255];

#    ifdef __cplusplus
        STags(void)
        {
            Empty();
        }
        
        void Empty(void)
        {
            StrCpy(chArtist, _T(""));
            StrCpy(chAlbum, _T(""));
            StrCpy(chTitle, _T(""));
            StrCpy(chComment, _T(""));
        }
#    endif
    };

	//// structs
	struct SOptions
	{
		TCHAR chVolumeLabel[128];
		int32 nWriteMethod;
		BS_BOOL bJolietFileSystem;
		BS_BOOL bBootable;
		TCHAR chBootImage[_MAX_PATH];
		BS_BOOL bFinalize;
		BS_BOOL bTestBurn;
		BS_BOOL bPerformOPC;
		BS_BOOL bVerifyAfterBurn;
		int32 nCacheSize;
		BS_BOOL bUnderrunProtection;
		BS_BOOL bEjectAfterBurn;
		BS_BOOL bAutoErase;
		BS_BOOL bRockRidgeFileSystem;

		//// only available for GUI functions
		int32 nCopies;
		//// Ende (only available for GUI functions)

		BS_BOOL bPadDataTracks;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA - sizeof(BS_BOOL))

#	ifdef __cplusplus
		SOptions(void)
		{
			Empty();
		}

		void Empty(void)
		{
			StrCpy(chVolumeLabel, _T("BS_NDEF"));
			nWriteMethod		= BS_NDEF;
			bJolietFileSystem	= BS_NDEF;
			bBootable			= BS_NDEF;
			StrCpy(chBootImage, _T("BS_NDEF"));
			bFinalize			= BS_NDEF;

			bTestBurn			= BS_NDEF;
			bPerformOPC			= BS_NDEF;
			bVerifyAfterBurn	= BS_NDEF;

			nCacheSize			= BS_NDEF;
			bUnderrunProtection	= BS_NDEF;
			bEjectAfterBurn		= BS_NDEF;
			bAutoErase          = BS_NDEF;
			bRockRidgeFileSystem= BS_NDEF;
			//// only available for GUI functions
			nCopies				= BS_NDEF;
			//// end (only available for GUI functions)
			bPadDataTracks		= BS_NDEF;
		}
#	endif
	};

	struct SCompressEncryptInfo
	{
		BS_BOOL		bCompression;
		int32		nCompressionLevel;
		
		BS_BOOL		bEncryption;
		const char* lpszPassword;

        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_SMALL)

#	ifdef __cplusplus
		SCompressEncryptInfo()
			:bCompression(BS_NDEF)
			,nCompressionLevel(0)
			,bEncryption(BS_NDEF)
			,lpszPassword(0)
		{
		}
#	endif
	};

	struct SISOVolumeInfo
	{
		TCHAR	chVolumeLabel[128];
		int32   nVolumeDescriptorAddress;
		int32   nVolumeSize;	  // in sectors
		int32	nRootAddress;
		int32   nPathTableAddress;
		int32   nPathTableSize;   // in bytes

		struct SISOInfoEx sInfoEx;

        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA)

#	ifdef __cplusplus
		SISOVolumeInfo()
		{
			StrCpy(chVolumeLabel, _T("BS_NDEF"));
			nVolumeDescriptorAddress = 0;
			nVolumeSize = 0;
			nRootAddress = 0;
			nPathTableAddress = 0;
			nPathTableSize = 0;
		}
#	endif
	};


	struct SBootVolumeInfo
	{		
		int32   nVolumeDescriptorAddress;		
		struct SBootInfoEx sInfoEx;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_HUGE)

#	ifdef __cplusplus
		SBootVolumeInfo()
		{			
			nVolumeDescriptorAddress = 0;			
		}
#	endif
	};

	struct SUDFVolumeInfo
	{
		TCHAR	chVolumeLabel[128];
		TCHAR	chPreparer[128];
		int32	nVersion;
		int32   nPartitionType;
		int32	nMVDSAddress;
		int32	nRVDSAddress;
		int32	nRootAddress;
		int32	nRootFEAddress;
		int32	nPartitionAddress;
		int32	nPartitionLength;
		int32	nLVDAddress;
		int32	nPVDAddress;
		int32	nFSDAddress;
		int32	nVATAddress;
		int32   nMetadataAddress;
		int32	nSparingAddress;
		int32	nFileCount;
		int32	nDirCount;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA)

#	ifdef __cplusplus
		SUDFVolumeInfo(void)
		{
			StrCpy(chVolumeLabel, _T("BS_NDEF"));
			StrCpy(chPreparer, _T("BS_NDEF"));
			nVersion = BS_NDEF;
			nFileCount = BS_NDEF;
			nDirCount = BS_NDEF;
			nMVDSAddress = BS_NDEF;
			nRVDSAddress = BS_NDEF;
			nRootAddress = BS_NDEF;
			nRootFEAddress = BS_NDEF;
			nPartitionAddress = BS_NDEF;
			nPartitionLength = BS_NDEF;
			nLVDAddress = BS_NDEF;
			nPVDAddress = BS_NDEF;
			nFSDAddress = BS_NDEF;
			nVATAddress = BS_NDEF;
			nMetadataAddress = BS_NDEF;
			nSparingAddress = BS_NDEF;
		}
#	endif
	};

	// Image types which can be created by SDK
	#define BS_IMG_ISO	1
	#define BS_IMG_BIN	2

	#define BS_IMGTASK_CREATE			1
	#define BS_IMGTASK_VERIFY			2	

	struct SReadErrorCorrectionParams
	{
		BS_BOOL	bErrorCorrection;
		BS_BOOL	bBlankBadSectors;
		uint8	nHardwareRetryCount;
		uint8	nSoftwareRetryCount;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL)

#ifdef __cplusplus
		SReadErrorCorrectionParams()
			:bErrorCorrection(BS_TRUE)
			,bBlankBadSectors(BS_TRUE)
			,nHardwareRetryCount(20)
			,nSoftwareRetryCount(0)
		{
		}
#endif
	};

	struct SCreateImageParams
	{
		TCHAR	lpszImagePath[_MAX_PATH];
		TCHAR	lpszBadSectorsFilePath[_MAX_PATH];
		int16	nImageType;
		int32	nVerifyBufferSectors;
		BS_BOOL	bFullCapacity;
		SReadErrorCorrectionParams cErrorParams;
					
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA - sizeof(int32) - sizeof(BS_BOOL))

#ifdef __cplusplus
		SCreateImageParams()
			:nImageType(BS_IMG_ISO)
			, nVerifyBufferSectors(1)
			, bFullCapacity(BS_TRUE)
		{
			StrCpy(lpszImagePath, _T(""));
			StrCpy(lpszBadSectorsFilePath, _T(""));
		}
#endif
	};



#define BS_ET_MP3		0
#define BS_ET_WMA		1
#define BS_ET_AAC		2
#define BS_ET_OGG		3
#define BS_ET_MP4		4

#define BS_BT_VARIABLE		0
#define BS_BT_CONSTANT		1

#define BS_TCH_NONE					0
#define BS_TCH_CDTEXT				1
#define BS_TCH_INTERNETDB			2
#define BS_TCH_CDTEXT_INTERNETDB	3
#define BS_TCH_INTERNETDB_CDTEXT	4

#define BS_INVALID_HANDLE		-1
#define BS_INVALID_TAG_HANDLE	BS_INVALID_HANDLE


	struct SAudioGrabbingParams
	{	
		int32 nBitrate;
		int32 nMinBitrate;
		int32 nMaxBitrate;
		uint8 nBitrateType;
		uint8 nEncoderType;
		uint8 nTagChoice;

		int32 nNetworkTagsHandle;
		
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_HUGE)

#ifdef __cplusplus
		SAudioGrabbingParams()
            :nBitrate(128)
            ,nMinBitrate(0)
            ,nMaxBitrate(0)
            ,nBitrateType(BS_BT_CONSTANT)
			,nEncoderType(BS_ET_MP3)
            ,nTagChoice(BS_TCH_NONE)

            ,nNetworkTagsHandle(BS_INVALID_TAG_HANDLE)
		{
		}
#endif
	};

	struct SDiskCopyOptions
	{
		uint8	nWriteMethod;
		uint8	nReadMode;
		BS_BOOL bVerifyAfterBurn;
		SReadErrorCorrectionParams cErrorParams;
		BS_BOOL bEjectAfterBurn;
		int32	nVerifyBufferSectors;
		BS_BOOL	bFullCapacity;

        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_NORMAL-sizeof(BS_BOOL) - sizeof(int32) - sizeof(BS_BOOL))

#ifdef __cplusplus
		SDiskCopyOptions()
			:nWriteMethod(BS_WM_DAO)
			,nReadMode(BS_RM_USERDATA)
			,bVerifyAfterBurn(BS_FALSE)
			,bEjectAfterBurn(BS_TRUE)
			,nVerifyBufferSectors(1)
			,bFullCapacity(BS_TRUE)
		{
		}
#endif
	};

	struct SFileEntry
	{
		TCHAR			lpszFileName[_MAX_PATH];
		TCHAR			lpszFilePath[_MAX_PATH];
		TCHAR			lpszFileOrigin[_MAX_PATH];
		int32			nAddress;
		int64			nFileSize;
		SFileDateTime	cDateTime;		// Specifies when the file is last modified.
		int32			nAttrib;

		SFileDateTime	cCreationTime;	// Specifies when the file or directory is created.
		SFileDateTime	cAccessTime;	// Specifies when the file is last read from or modified.

		void			*pUserParam;
				
#if defined(__APPLE__) && defined(__x86_64__)
        INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA - sizeof(SFileDateTime/*cCreationTime*/) - sizeof(SFileDateTime/*cAccessTime*/) - sizeof(void*/*UserParam*/) - 8/*structure alignment?*/)
#else
		INSERT_STRUCTURE_PADDING(STRUCTURE_PADDING_STRING_DATA - sizeof(SFileDateTime/*cCreationTime*/) - sizeof(SFileDateTime/*cAccessTime*/) - sizeof(void*/*UserParam*/));
#endif
	};

	struct SExtent
	{
		uint32 Location;
		uint32 Length;
	};

	struct SFileAllocationTable
	{
		int16	EmbeddedFileOffset;
		int8	NumExtents;
		SExtent	Extents[1];
	};

	//// end (structs)
//// end (burning options)

//// (S)VCD definitions

	#define BS_VCD_KEY_0		0
	#define BS_VCD_KEY_DEFAULT	101
	#define BS_VCD_KEY_RETURN	102
	#define BS_VCD_KEY_PREVIOUS 103
	#define BS_VCD_KEY_NEXT		104

	#define BS_VCD_INFINITE		0xFFFF

	#define BS_VCD_TRACK_ITEM_0		0
	#define BS_VCD_SEGMENT_ITEM_0	1000

//// end ((S)VCD definitions)

//// texts
	//// license key
	#define BS_SDK_KEY_INVALID              -1
	#define BS_SDK_KEY_VALID                0
	//// end (license key)


	//// error codes
	#define BS_SDK_ERROR_NO                 0
	#define BS_SDK_ERROR_NOT_ALLOWED        1
	#define BS_SCSI_ERROR_PARAM_10	        2
	#define BS_SDK_ERROR_BAD_REQUEST        3
	#define BS_SDK_ERROR_GENERAL			4
	#define BS_SCSI_ERROR_ASPI_01           5
	#define BS_SCSI_ERROR_ASPI_06		    6
	#define BS_SDK_ERROR_INVALID_PATH       7
	#define BS_SDK_ERROR_INVALID_SRC_PATH   8
	#define BS_SDK_ERROR_INVALID_DEST_PATH	9
	#define BS_SDK_ERROR_INVALID_FILE_NAME  10
	#define BS_SDK_ERROR_PATH_EXISTS        11
	#define BS_SDK_ERROR_FILE_EXISTS        12
	#define BS_SDK_CUE_ERROR_COMMAND_06     13
	#define BS_SDK_ERROR_INVALID_FILE_FORMAT 15
	#define BS_SDK_ERROR_FILE_OPEN          16
	#define BS_SDK_ERROR_CORRUPT_OR_INVALID_CUE_FILE 17
    #define BS_SDK_ERROR_BIN_FILE_NOT_FOUND 18
	#define BS_SDK_ERROR_NOT_IMPLEMENTED    19
	#define BS_SDK_ERROR_NOT_ALLOWED_FOR_THIS_BURNER 20
	#define BS_SCSI_ERROR_DISK_29           21
	#define BS_SCSI_ERROR_DISK_30           22
	#define BS_SCSI_ERROR_DISK_31           23
	#define BS_SCSI_ERROR_DISK_32           24
	#define BS_SDK_ERROR_UNKNOWN_TEXTID     25
	#define BS_SDK_ERROR_MORE_SPACE_NEEDED  26
	#define BS_SDK_ERROR_BURN_IN_PROGRESS	27
	#define BS_SDK_ERROR_INVALID_SESSION_NUMBER 28
	#define BS_SDK_ERROR_EMPTY_PASSWORD		29

	#define BS_SDK_ERROR_UNKNOWN            101
	#define BS_SCSI_ERROR_ASPI_02           102
	#define BS_SCSI_ERROR_ASPI_03           103
	#define BS_SDK_ERROR_INVALIDSRB         104
	#define BS_SDK_ERROR_BUFFERALIGNMENT    105
	#define BS_SCSI_ERROR_ASPI_04           106
	#define BS_SDK_ERROR_BUFFERTOOBIG       107
	#define BS_SDK_ERROR_TIMEOUT            108
	#define BS_SDK_ERROR_SRBTIMEOUT         109
	#define BS_SDK_ERROR_MESSAGEREJECT      110
	#define BS_SCSI_ERROR_ASPI_05           111
	#define BS_SDK_ERROR_PARITYERR          112
	#define BS_SCSI_ERROR_107               113
	#define BS_SDK_ERROR_SELECTIONTIMEOUT   114
	#define BS_SDK_ERROR_DATAOVERRUN        115
	#define BS_SDK_ERROR_UNEXPECTEDBUSFREE  116
	#define BS_SDK_ERROR_CHECKCONDITION     117
	#define BS_SCSI_ERROR_ASPI_08           118
	#define BS_SCSI_ERROR_ASPI_09           119
	#define BS_SCSI_ERROR_ASPI_10           120
	#define BS_SDK_ERROR_DATARECOVERERROR   121
	#define BS_SDK_ERROR_NOTREADY           122
	#define BS_SCSI_ERROR_DISK_33           123
	#define BS_SDK_ERROR_HARDWAREERROR      124
	#define BS_SDK_ERROR_UNITATTENTION      126
	#define BS_SDK_ERROR_DATAPROTECT        127
	#define BS_SDK_ERROR_ERASECHECK         128
	#define BS_SDK_ERROR_COPYABORTED        129
	#define BS_SDK_ERROR_ABORTEDCOMMAND     130
	#define BS_SDK_ERROR_VOLUMEOVERFLOW     131
	#define BS_SDK_ERROR_MISCOMPARE         132
	#define BS_SDK_ERROR_RESERVED           133
	#define BS_SDK_ERROR_FILEMARK           134
	#define BS_SDK_ERROR_ILLEGALLENGTH      136
	#define BS_SDK_ERROR_INCORRECTLENGTH    137
	#define BS_SDK_ERROR_ABORTED            138
	#define BS_SDK_ERROR_FILEINUSE          139
	#define BS_SDK_ERROR_CREATEFILE         140
	#define BS_SCSI_ERROR_ASPI_07           141
	#define BS_SCSI_ERROR_WRITE_12          142
	#define BS_SDK_ERROR_NOTSUPPORTED       143
	#define BS_SDK_ERROR_NEXTADDRESS        144
    #define BS_SDK_ERROR_TOO_MUCH_DATA      145
	#define BS_SDK_ERROR_MAXDIRS            146
	#define BS_SDK_ERROR_MAXFILES           147
	#define BS_SDK_ERROR_ERRINVALIDFILENAME 148
	#define BS_SDK_ERROR_IMPORTSESSION      149
	#define BS_SDK_ERROR_ISOIMAGENOTFOUND   150
	#define BS_SDK_INT_ERROR_1              151
	#define BS_SDK_INT_ERROR_2              152
	#define BS_SDK_INT_ERROR_3              153
	#define BS_SDK_INT_ERROR_4              154
	#define BS_SDK_INT_ERROR_5              155
    #define BS_SDK_CUE_ERROR_SENDING_CUE    156
	#define BS_SDK_CUE_ERROR_UEOL			157
	#define BS_SDK_CUE_ERROR_FIELD			158
	#define BS_SDK_CUE_ERROR_FILE			159
	#define BS_SDK_CUE_ERROR_COMMAND_01     160
    #define BS_SDK_INT_ERROR_FORMAT         161
	#define BS_SDK_ERROR_NOT_ALLOWED_FOR_THIS_UDF_VERSION	162
	#define BS_SDK_ERROR_INVALID_UDF_VERSION	163
	#define BS_SDK_ERROR_MP3LIB_NOT_FOUND		164
	#define BS_SDK_ERROR_INCOMPATIBLE_FS_TYPE	165
	#define BS_SDK_ERROR_COMPRESSION_CONFLICT	166
	#define BS_SDK_ERROR_ENCRYPTION_CONFLICT	167
	#define BS_SDK_ERROR_INVALID_DIR_INDEX		168
	#define BS_SDK_ERROR_INVALID_MCN		169
	#define BS_SDK_ERROR_INVALID_ISRC		170
	#define BS_ILE_TOO_LONG_DIRECTORY_NESTING	171
	#define BS_ILE_TOO_BIG_FILE					172
	#define BS_SDK_ERROR_INVALID_INDEX		173
	#define BS_SDK_ERROR_TOO_MUCH_INDEXES	174
	#define BS_SDK_ERROR_ENCLIB_NOT_FOUND	175
	#define BS_SDK_ERROR_CDTEXT_NOT_FOUND	176
	#define BS_SDK_ERROR_NETTAGS_DISK		177
	#define BS_SDK_ERROR_NETTAGS_CONNECT	178
	#define BS_SDK_ERROR_NETTAGS_SERVER		179
	#define BS_SDK_ERROR_NETTAGS_NOMATCH	180
	#define BS_SDK_ERROR_NETTAGS_INTERNAL	181
	#define BS_SDK_ERROR_UNSUPPORTED_MEDIUM 182
	#define BS_SDK_ERROR_DEVICE_LOCKED		183
    #define BS_SDK_ERROR_PLUGIN             184



	#define BS_SCSI_ERROR_01                200
	#define BS_SCSI_ERROR_02                201
	#define BS_SCSI_ERROR_AUDIO_01          202
	#define BS_SCSI_ERROR_AUDIO_02          203
	#define BS_SCSI_ERROR_AUDIO_03          204
	#define BS_SCSI_ERROR_AUDIO_04          205
	#define BS_SCSI_ERROR_AUDIO_05          206
	#define BS_SCSI_ERROR_001               207
	#define BS_SCSI_ERROR_002               208
	#define BS_SCSI_ERROR_UNIT_01           209
	#define BS_SCSI_ERROR_UNIT_02           210
	#define BS_SCSI_ERROR_UNIT_03           211
	#define BS_SCSI_ERROR_UNIT_04           212
	#define BS_SCSI_ERROR_UNIT_05           213
	#define BS_SCSI_ERROR_UNIT_06           214
	#define BS_SCSI_ERROR_UNIT_07           215
	#define BS_SCSI_ERROR_UNIT_08           216
	#define BS_SCSI_ERROR_UNIT_09           217
	#define BS_SCSI_ERROR_UNIT_10           218
	#define BS_SCSI_ERROR_004 				219
	#define BS_SCSI_ERROR_005 				220
	#define BS_SCSI_ERROR_UNIT_11           221
	#define BS_SCSI_ERROR_UNIT_12           222
	#define BS_SCSI_ERROR_UNIT_13           223
	#define BS_SCSI_ERROR_UNIT_14           224
	#define BS_SCSI_ERROR_006 				225
	#define BS_SCSI_ERROR_007 				226
	#define BS_SCSI_ERROR_008 				227
	#define BS_SCSI_ERROR_009 				228
	#define BS_SCSI_ERROR_010 				229
	#define BS_SCSI_ERROR_011 				230
	#define BS_SCSI_ERROR_LOG_01            231
	#define BS_SCSI_ERROR_ATT_01            232
	#define BS_SCSI_ERROR_ATT_02            233
	#define BS_SCSI_ERROR_ATT_03            234
	#define BS_SCSI_ERROR_WRITE_01          235
	#define BS_SCSI_ERROR_WRITE_02          236
	#define BS_SCSI_ERROR_WRITE_03          237
	#define BS_SCSI_ERROR_WRITE_04          238
	#define BS_SCSI_ERROR_WRITE_05          239
	#define BS_SCSI_ERROR_EXT_01            240
	#define BS_SCSI_ERROR_EXT_02            241
	#define BS_SCSI_ERROR_TARGET_01         242
	#define BS_SCSI_ERROR_TARGET_02         243
	#define BS_SCSI_ERROR_TARGET_03         244
	#define BS_SCSI_ERROR_TARGET_04         245
	#define BS_SCSI_ERROR_READ_01           246
	#define BS_SCSI_ERROR_READ_02           247
	#define BS_SCSI_ERROR_012 				248
	#define BS_SCSI_ERROR_013 				249
	#define BS_SCSI_ERROR_CIRC_01           250
	#define BS_SCSI_ERROR_CRC_01            251
	#define BS_SCSI_ERROR_DECOM_01          252
	#define BS_SCSI_ERROR_DRIVE_01          253
	#define BS_SCSI_ERROR_DRIVE_02          254
	#define BS_SCSI_ERROR_014 				255
	#define BS_SCSI_ERROR_015 				256
	#define BS_SCSI_ERROR_016 				257
	#define BS_SCSI_ERROR_017 				258
	#define BS_SCSI_ERROR_MECH_01           259
	#define BS_SCSI_ERROR_MECH_02           260
	#define BS_SCSI_ERROR_RECOVER_01        261
	#define BS_SCSI_ERROR_RECOVER_02        262
	#define BS_SCSI_ERROR_RECOVER_03        263
	#define BS_SCSI_ERROR_RECOVER_04        264
	#define BS_SCSI_ERROR_RECOVER_05        265
	#define BS_SCSI_ERROR_RECOVER_06        266
	#define BS_SCSI_ERROR_RECOVER_07        267
	#define BS_SCSI_ERROR_RECOVER_08        268
	#define BS_SCSI_ERROR_RECOVER_09        269
	#define BS_SCSI_ERROR_RECOVER_10        270
	#define BS_SCSI_ERROR_RECOVER_11        271
	#define BS_SCSI_ERROR_RECOVER_12        272
	#define BS_SCSI_ERROR_RECOVER_13        273
	#define BS_SCSI_ERROR_RECOVER_14        274
	#define BS_SCSI_ERROR_RECOVER_15        275
	#define BS_SCSI_ERROR_RECOVER_16        276
	#define BS_SCSI_ERROR_RECOVER_17        277
	#define BS_SCSI_ERROR_018 				278
	#define BS_SCSI_ERROR_019 				279
	#define BS_SCSI_ERROR_020 				280
	#define BS_SCSI_ERROR_021 				281
	#define BS_SCSI_ERROR_022 				282
	#define BS_SCSI_ERROR_023 				283
	#define BS_SCSI_ERROR_024 				284
	#define BS_SCSI_ERROR_CDB_01            285
	#define BS_SCSI_ERROR_CDB_02            286
	#define BS_SCSI_ERROR_UNIT_16           287
	#define BS_SCSI_ERROR_PARAM_01          288
	#define BS_SCSI_ERROR_PARAM_02          289
	#define BS_SCSI_ERROR_PARAM_03          290
	#define BS_SCSI_ERROR_PARAM_04          291
	#define BS_SCSI_ERROR_025 				292
	#define BS_SCSI_ERROR_026 				293
	#define BS_SCSI_ERROR_SEGM_01           296
	#define BS_SCSI_ERROR_SEGM_02           297
	#define BS_SCSI_ERROR_SEGM_03           298
	#define BS_SCSI_ERROR_027 				299
	#define BS_SCSI_ERROR_028 				300
	#define BS_SCSI_ERROR_SEGM_04           301
	#define BS_SCSI_ERROR_WRITE_06          302
	#define BS_SCSI_ERROR_WRITE_07          303
	#define BS_SCSI_ERROR_WRITE_08          305
	#define BS_SCSI_ERROR_WRITE_09          306
	#define BS_SCSI_ERROR_WRITE_10          307
	#define BS_SCSI_ERROR_WRITE_11          308
	#define BS_SCSI_ERROR_029 				309
	#define BS_SCSI_ERROR_030 				310
	#define BS_SCSI_ERROR_031 				311
	#define BS_SCSI_ERROR_032 				312
	#define BS_SCSI_ERROR_033 				313
	#define BS_SCSI_ERROR_034 				314
	#define BS_SCSI_ERROR_035 				315
	#define BS_SCSI_ERROR_036 				316
	#define BS_SCSI_ERROR_037 				317
	#define BS_SCSI_ERROR_PARAM_05          318
	#define BS_SCSI_ERROR_PARAM_06          319
	#define BS_SCSI_ERROR_PARAM_07          320
	#define BS_SCSI_ERROR_038 				321
	#define BS_SCSI_ERROR_039 				322
	#define BS_SCSI_ERROR_040 				323
	#define BS_SCSI_ERROR_041 				324
	#define BS_SCSI_ERROR_COMMAND_02        325
	#define BS_SCSI_ERROR_042 				326
	#define BS_SCSI_ERROR_043 				327
	#define BS_SCSI_ERROR_044 				328
	#define BS_SCSI_ERROR_045 				329
	#define BS_SCSI_ERROR_COMMAND_03        330
	#define BS_SCSI_ERROR_DISK_01           331
	#define BS_SCSI_ERROR_DISK_02           332
	#define BS_SCSI_ERROR_DISK_03           333
	#define BS_SCSI_ERROR_DISK_04           334
	#define BS_SCSI_ERROR_DISK_05           335
	#define BS_SCSI_ERROR_DISK_06           336
	#define BS_SCSI_ERROR_DISK_07           337
	#define BS_SCSI_ERROR_DISK_08           338
	#define BS_SCSI_ERROR_DISK_09           339
	#define BS_SCSI_ERROR_DISK_10           340
	#define BS_SCSI_ERROR_DISK_11           341
	#define BS_SCSI_ERROR_DISK_12           342
	#define BS_SCSI_ERROR_DISK_13           343
	#define BS_SCSI_ERROR_046               344
	#define BS_SCSI_ERROR_047               345
	#define BS_SCSI_ERROR_048 				346
	#define BS_SCSI_ERROR_049 				347
	#define BS_SCSI_ERROR_050 				348
	#define BS_SCSI_ERROR_051 				349
	#define BS_SCSI_ERROR_052 				350
	#define BS_SCSI_ERROR_PARAM_08          351
	#define BS_SCSI_ERROR_PARAM_09          352
	#define BS_SCSI_ERROR_DISK_14           353
	#define BS_SCSI_ERROR_DISK_15           354
	#define BS_SCSI_ERROR_DISK_16           355
	#define BS_SCSI_ERROR_DISK_17           356
	#define BS_SCSI_ERROR_DISK_18           357
	#define BS_SCSI_ERROR_DISK_19           358
	#define BS_SCSI_ERROR_DISK_20           359
	#define BS_SCSI_ERROR_DISK_21           360
	#define BS_SCSI_ERROR_DISK_22           361
	#define BS_SCSI_ERROR_DISK_23           362
	#define BS_SCSI_ERROR_DISK_24           363
	#define BS_SCSI_ERROR_DISK_25           364
	#define BS_SCSI_ERROR_DISK_26           365
	#define BS_SCSI_ERROR_MECH_03           366
	#define BS_SCSI_ERROR_053 				367
	#define BS_SCSI_ERROR_UNIT_17           368
	#define BS_SCSI_ERROR_UNIT_18           369
	#define BS_SCSI_ERROR_UNIT_19           370
	#define BS_SCSI_ERROR_UNIT_20           371
	#define BS_SCSI_ERROR_UNIT_21           372
	#define BS_SCSI_ERROR_054 				373
	#define BS_SCSI_ERROR_055 				374
	#define BS_SCSI_ERROR_056 				375
    #define BS_SCSI_ERROR_057 				376
	#define BS_SCSI_ERROR_058 				377
	#define BS_SCSI_ERROR_059 				378
	#define BS_SCSI_ERROR_060 				379
	#define BS_SCSI_ERROR_061 				380
	#define BS_SCSI_ERROR_062 				381
	#define BS_SCSI_ERROR_063 				382
	#define BS_SCSI_ERROR_VOL_01            383
	#define BS_SCSI_ERROR_VOL_02            384
	#define BS_SCSI_ERROR_VOL_03            385
	#define BS_SCSI_ERROR_VOL_04            386
	#define BS_SCSI_ERROR_064 				387
	#define BS_SCSI_ERROR_065 				388
	#define BS_SCSI_ERROR_DISK_27           389
	#define BS_SCSI_ERROR_DISK_28           390
	#define BS_SCSI_ERROR_066 				391
	#define BS_SCSI_ERROR_067 				392
	#define BS_SCSI_ERROR_068 				393
	#define BS_SCSI_ERROR_069 				394
	#define BS_SCSI_ERROR_070 				395
	#define BS_SCSI_ERROR_071 				396
	#define BS_SCSI_ERROR_072 				397
	#define BS_SCSI_ERROR_073 				398
	#define BS_SCSI_ERROR_074 				399
	#define BS_SCSI_ERROR_075 				400
	#define BS_SCSI_ERROR_076 				401
	#define BS_SCSI_ERROR_COMMAND_04        402
	#define BS_SCSI_ERROR_077 				403
	#define BS_SCSI_ERROR_UNIT_22           404
	#define BS_SCSI_ERROR_COMMAND_05        405
	#define BS_SCSI_ERROR_078 				410
	#define BS_SCSI_ERROR_079 				411
	#define BS_SCSI_ERROR_080 				412
	#define BS_SCSI_ERROR_081 				413
	#define BS_SCSI_ERROR_082 				414
	#define BS_SCSI_ERROR_083 				415
	#define BS_SCSI_ERROR_084 				416
	#define BS_SCSI_ERROR_085 				417
	#define BS_SCSI_ERROR_LOG_02            418
	#define BS_SCSI_ERROR_086 				419
	#define BS_SCSI_ERROR_LOG_03            420
	#define BS_SCSI_ERROR_LOG_04            421
	#define BS_SCSI_ERROR_087 				422
	#define BS_SCSI_ERROR_UNIT_23           424
	#define BS_SCSI_ERROR_088 				425
	#define BS_SCSI_ERROR_089 				426
	#define BS_SCSI_ERROR_090 				427
	#define BS_SCSI_ERROR_091 				428
	#define BS_SCSI_ERROR_092 				429
	#define BS_SCSI_ERROR_093 				430
	#define BS_SCSI_ERROR_094 				431
	#define BS_SCSI_ERROR_095 				432
	#define BS_SCSI_ERROR_096 				433
	#define BS_SCSI_ERROR_097 				434
	#define BS_SCSI_ERROR_098 				435
	#define BS_SCSI_ERROR_099 				436
	#define BS_SCSI_ERROR_DCSS_01           437
	#define BS_SCSI_ERROR_DCSS_02           438
	#define BS_SCSI_ERROR_DCSS_03           439
	#define BS_SCSI_ERROR_DCSS_04           440
	#define BS_SCSI_ERROR_DCSS_05           441
	#define BS_SCSI_ERROR_DCSS_06           442
	#define BS_SCSI_ERROR_SESSION_01        443
	#define BS_SCSI_ERROR_SESSION_02        444
	#define BS_SCSI_ERROR_SESSION_03        445
	#define BS_SCSI_ERROR_SESSION_04        446
	#define BS_SCSI_ERROR_100               447
	#define BS_SCSI_ERROR_101 				448
	#define BS_SCSI_ERROR_102 				449
	#define BS_SCSI_ERROR_103 				450
	#define BS_SCSI_ERROR_104 				451
	#define BS_SCSI_ERROR_105               452
	#define BS_SCSI_ERROR_ALLOC_01          453
	#define BS_SCSI_ERROR_ALLOC_02 			454
	#define BS_SCSI_ERROR_106 				455
	#define BS_SCSI_ERROR_DISK_34           456
	#define BS_MULTI_ERROR_01				457
	#define BS_MULTI_ERROR_02				458
	#define BS_SCSI_ERROR_DISK_35			459
	#define BS_SCSI_ERROR_108				460
	#define BS_SCSI_ERROR_109				461
	#define BS_SCSI_ERROR_ATT_04			462
	#define BS_SCSI_ERROR_ATT_05			463
	#define BS_SCSI_ERROR_110				464
	#define BS_SCSI_ERROR_111				465
	#define BS_SCSI_ERROR_112				466
	#define BS_SCSI_ERROR_DISK_36			467
	#define BS_SCSI_ERROR_113				468
	#define BS_SCSI_ERROR_114				469
	#define BS_SCSI_ERROR_115				470
	#define BS_SCSI_ERROR_116				471
	#define BS_SCSI_ERROR_117				472
	#define BS_SCSI_ERROR_118				473
	#define BS_SCSI_ERROR_119				474
	#define BS_SCSI_ERROR_DCSS_07           475
	#define BS_SCSI_ERROR_DCSS_08           476
	#define BS_SCSI_ERROR_DISK_37			477
	
	#define BS_SDK_ERROR_INVALID_HANDLE			490
	#define BS_SDK_ERROR_BAD_CAPABILITY_NAME	491

	
	//// error codes


	//// info codes
	#define BS_SDK_MESSAGE_WAIT             500
	#define BS_SDK_MESSAGE_WRITESTART       501
    #define BS_SDK_MESSAGE_EREASESTART      502
    #define BS_SDK_MESSAGE_EXTR_FILE        503
	#define BS_SDK_MESSAGE_SIMULATE         504
	#define BS_SDK_MESSAGE_IMPORT           505
    #define BS_SDK_MESSAGE_FORMAT           506
	#define BS_SDK_MESSAGE_FORMAT_DONE      507
	#define BS_SDK_MESSAGE_IMAGECREATESTART 508
	//// end (info codes)


	//// text codes
	#define BS_SDK_MESSAGE_01						600
	#define BS_SDK_MESSAGE_02						601
	#define BS_SDK_MESSAGE_03						602
	#define BS_SDK_MESSAGE_04						603
	#define BS_SDK_MESSAGE_05						604
	#define BS_SDK_MESSAGE_06						605
	#define BS_SDK_MESSAGE_07						606
	#define BS_SDK_MESSAGE_08						607
	#define BS_SDK_MESSAGE_10						608
	#define BS_SDK_MESSAGE_11						609
	#define BS_SDK_MESSAGE_12						610
	#define BS_SDK_MESSAGE_13						611
	#define BS_SDK_MESSAGE_14						612
	#define BS_SDK_MESSAGE_15						613
	#define BS_SDK_MESSAGE_16						614
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_INIT_COMPLETE					615
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_ERROR_CREATING_PROVIDER		616
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_GETTING_MATCHING_PARAMS		617
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_ERROR_CREATING_MATCHING_PARAM	618
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_GETTING_MATCHING_ENTRIES		619
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_ERROR_GETTING_MATCHING_ENTRIES	620
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_MULTIPLE_MATCHES				621
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_SINGLE_MATCH					622
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_SELECT_ENTRY					623
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_CANNOT_SELECT_ENTRY			624
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_FETCHING_SELECTED_ENTRY		625
	#define BS_SDK_MESSAGE_INTERNETDB_STATUS_ERROR_GETTING_SELECTED_ENTRY	626
	#define BS_GUI_RESOURCE_01						630
	#define BS_GUI_RESOURCE_02						631
	#define BS_GUI_RESOURCE_03						632
	#define BS_GUI_RESOURCE_04						633
	#define BS_GUI_RESOURCE_05						634
	#define BS_GUI_RESOURCE_06						635
	#define BS_GUI_RESOURCE_07						636
	#define BS_GUI_RESOURCE_08						637
	#define BS_GUI_RESOURCE_09						638
	#define BS_GUI_RESOURCE_10						639
	#define BS_GUI_RESOURCE_11						640
	#define BS_GUI_RESOURCE_12						641
	#define BS_GUI_RESOURCE_13						642
	#define BS_GUI_RESOURCE_14						643
	#define BS_GUI_RESOURCE_15						644
	#define BS_GUI_RESOURCE_16						645
	#define BS_GUI_RESOURCE_17						646 //Joliet File System
	#define BS_GUI_RESOURCE_18						647
	#define BS_GUI_RESOURCE_19						648
	#define BS_GUI_RESOURCE_20						649
	#define BS_GUI_RESOURCE_21						650
	#define BS_GUI_RESOURCE_22						651
	#define BS_GUI_RESOURCE_23						652
	#define BS_GUI_RESOURCE_24						653
	#define BS_GUI_RESOURCE_25						654
	#define BS_GUI_RESOURCE_26				        655
	#define BS_GUI_RESOURCE_27		                656
	#define BS_GUI_RESOURCE_28						657
	#define BS_GUI_RESOURCE_29						658
	#define BS_GUI_RESOURCE_30						659
	#define BS_GUI_RESOURCE_31						660
	#define BS_GUI_RESOURCE_32						661
	#define BS_GUI_RESOURCE_33						662
	#define BS_GUI_RESOURCE_34						663
	#define BS_GUI_RESOURCE_35						664		// "Other" tab
	#define BS_GUI_RESOURCE_36						665		// "Compression"
	#define BS_GUI_RESOURCE_37						666		// "Encryption"
	#define BS_GUI_RESOURCE_38						667		// "Password"
	#define BS_GUI_RESOURCE_39						668		// "UDF" tab
	#define BS_GUI_RESOURCE_40						669		// "Version"
	#define BS_GUI_RESOURCE_41						670		// "Partition type"
	#define BS_GUI_RESOURCE_42						671		// "Save Log"
	#define BS_GUI_RESOURCE_43						672		// "Select Multiple Devices"
	#define BS_GUI_RESOURCE_44						673		// "Multiple devices"
	#define BS_GUI_RESOURCE_45						674		// "Write method"
	#define BS_GUI_RESOURCE_46						675		// "Confirm"
	#define BS_GUI_ERROR_01							676		// "Password and confirmation do not match"
	#define BS_GUI_ERROR_02							677		// "Password is empty"
	#define BS_GUI_RESOURCE_47						678		// "Write streams"
	#define BS_GUI_RESOURCE_48						679		// "Rescan"
	#define BS_GUI_RESOURCE_49						680		// "Auto erase"
	#define BS_GUI_RESOURCE_50						681		// "Rock Ridge File System"
	#define BS_GUI_RESOURCE_51						682		// "Pad data tracks"
	#define BS_GUI_RESOURCE_52						683		// "Eject"
	#define BS_GUI_RESOURCE_53						684		// "AVCHD disc"
	#define BS_GUI_RESOURCE_INTERNET_DIALOG_TITLE	685
	#define BS_GUI_RESOURCE_INTERNET_DIALOG_CATEGORY_COLUMN		686
	#define BS_GUI_RESOURCE_INTERNET_DIALOG_ARTIST_COLUMN		687
	#define BS_GUI_RESOURCE_INTERNET_DIALOG_TITLE_COLUMN		688
	#define BS_GUI_RESOURCE_INTERNET_DIALOG_DISCID_COLUMN		689
	#define BS_GUI_RESOURCE_INTERNET_DIALOG_STATUS_LABEL		690

	#define BS_SDK_VERIFY_ERROR_HDDFILEUNREADABLE	700
	#define BS_SDK_VERIFY_ERROR_CDFILEUNREADABLE	701
	#define BS_SDK_VERIFY_ERROR_FILESDIFFERENT		702
	
	//// end (text codes)
//// end (texts)


#	ifdef __cplusplus
}
#	endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif	// __FOXSDK_DEFINITIONS_H__
