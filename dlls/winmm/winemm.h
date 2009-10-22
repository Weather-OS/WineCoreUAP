/*
 * Copyright 1998, Luiz Otavio L. Zorzella
 *           1999, Eric Pouech
 *
 * Purpose:   multimedia declarations (internal to WINMM & MMSYSTEM DLLs)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "mmddk.h"

#define WINE_DEFAULT_WINMM_DRIVER     "alsa,oss,coreaudio,esd"
#define WINE_DEFAULT_WINMM_MAPPER     "msacm32.drv"
#define WINE_DEFAULT_WINMM_MIDI       "midimap.dll"

typedef enum {
    WINMM_MAP_NOMEM, 	/* ko, memory problem */
    WINMM_MAP_MSGERROR, /* ko, unknown message */
    WINMM_MAP_OK, 	/* ok, no memory allocated. to be sent to the proc. */
    WINMM_MAP_OKMEM, 	/* ok, some memory allocated, need to call UnMapMsg. to be sent to the proc. */
} WINMM_MapType;

/* Who said goofy boy ? */
#define	WINE_DI_MAGIC	0x900F1B01

typedef struct tagWINE_DRIVER
{
    DWORD			dwMagic;
    /* as usual LPWINE_DRIVER == hDriver32 */
    DWORD			dwFlags;
    union {
	struct {
	    HMODULE			hModule;
	    DRIVERPROC			lpDrvProc;
	    DWORD_PTR		  	dwDriverID;
	} d32;
	struct {
	    UINT16			hDriver16;
	} d16;
    } d;
    struct tagWINE_DRIVER*	lpPrevItem;
    struct tagWINE_DRIVER*	lpNextItem;
} WINE_DRIVER, *LPWINE_DRIVER;

typedef	DWORD	(CALLBACK *WINEMM_msgFunc16)(UINT16, WORD, DWORD, DWORD, DWORD);
typedef	DWORD	(CALLBACK *WINEMM_msgFunc32)(UINT  , UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

/* for each loaded driver and each known type of driver, this structure contains
 * the information needed to access it
 */
typedef struct tagWINE_MM_DRIVER_PART {
    int				nIDMin;		/* lower bound of global indexes for this type */
    int				nIDMax;		/* hhigher bound of global indexes for this type */
    union {
	WINEMM_msgFunc32	fnMessage32;	/* pointer to function */
	WINEMM_msgFunc16	fnMessage16;
    } u;
} WINE_MM_DRIVER_PART;

#define	MMDRV_AUX		0
#define	MMDRV_MIXER		1
#define	MMDRV_MIDIIN		2
#define	MMDRV_MIDIOUT		3
#define	MMDRV_WAVEIN		4
#define	MMDRV_WAVEOUT		5
#define MMDRV_MAX		6

/* each low-level .drv will be associated with an instance of this structure */
typedef struct tagWINE_MM_DRIVER {
    HDRVR			hDriver;
    LPSTR			drvname;	/* name of the driver */
    unsigned			bIs32 : 1,	/* TRUE if 32 bit driver, FALSE for 16 */
	                        bIsMapper : 1;	/* TRUE if mapper */
    WINE_MM_DRIVER_PART		parts[MMDRV_MAX];/* Information for all known types */
} WINE_MM_DRIVER, *LPWINE_MM_DRIVER;

typedef struct tagWINE_MLD {
/* EPP struct tagWINE_MLD*	lpNext; */		/* not used so far */
       UINT			uDeviceID;
       UINT			type;
       UINT			mmdIndex;		/* index to low-level driver in MMDrvs table */
       DWORD_PTR		dwDriverInstance;	/* this value is driver related, as opposed to
							 * opendesc.dwInstance which is client (callback) related */
       WORD			bFrom32;
       WORD			dwFlags;
       DWORD_PTR		dwCallback;
       DWORD_PTR		dwClientInstance;
} WINE_MLD, *LPWINE_MLD;

typedef struct  {
       WINE_MLD			mld;
} WINE_WAVE, *LPWINE_WAVE;

typedef struct {
       WINE_MLD			mld;
       MIDIOPENDESC		mod;			/* FIXME: should be removed */
} WINE_MIDI, *LPWINE_MIDI;

typedef struct {
       WINE_MLD			mld;
} WINE_MIXER, *LPWINE_MIXER;

#define WINE_MMTHREAD_CREATED	0x4153494C	/* "BSIL" */
#define WINE_MMTHREAD_DELETED	0xDEADDEAD

typedef struct {
       DWORD			dwSignature;		/* 00 "BSIL" when ok, 0xDEADDEAD when being deleted */
       DWORD			dwCounter;		/* 04 > 1 when in mmThread functions */
       HANDLE			hThread;		/* 08 hThread */
       DWORD                    dwThreadID;     	/* 0C */
       DWORD    		fpThread;		/* 10 address of thread proc (segptr or lin depending on dwFlags) */
       DWORD			dwThreadPmt;    	/* 14 parameter to be passed upon thread creation to fpThread */
       LONG                     dwSignalCount;	     	/* 18 counter used for signaling */
       HANDLE                   hEvent;     		/* 1C event */
       HANDLE                   hVxD;		     	/* 20 return from OpenVxDHandle */
       DWORD                    dwStatus;       	/* 24 0x00, 0x10, 0x20, 0x30 */
       DWORD			dwFlags;		/* 28 dwFlags upon creation */
       UINT16			hTask;          	/* 2C handle to created task */
} WINE_MMTHREAD;

typedef struct tagWINE_MCIDRIVER {
        UINT			wDeviceID;
        UINT			wType;
	LPWSTR			lpstrElementName;
        LPWSTR			lpstrDeviceType;
        LPWSTR			lpstrAlias;
        HDRVR			hDriver;
        DWORD_PTR               dwPrivate;
        YIELDPROC		lpfnYieldProc;
        DWORD	                dwYieldData;
        BOOL			bIs32;
        DWORD                   CreatorThread;
        UINT			uTypeCmdTable;
        UINT			uSpecificCmdTable;
        struct tagWINE_MCIDRIVER*lpNext;
} WINE_MCIDRIVER, *LPWINE_MCIDRIVER;

struct IOProcList
{
    struct IOProcList*pNext;       /* Next item in linked list */
    FOURCC            fourCC;      /* four-character code identifying IOProc */
    LPMMIOPROC	      pIOProc;     /* pointer to IProc */
    BOOL              is_unicode;  /* 32A or 32W */
    int		      count;	   /* number of objects linked to it */
};

typedef struct tagWINE_MMIO {
    MMIOINFO			info;
    struct tagWINE_MMIO*	lpNext;
    struct IOProcList*		ioProc;
    unsigned			bTmpIOProc : 1,
                                bBufferLoaded : 1;
    DWORD                       dwFileSize;
} WINE_MMIO, *LPWINE_MMIO;

/* function prototypes */

typedef	WINMM_MapType	        (*MMDRV_MAPFUNC)(UINT wMsg, DWORD_PTR *lpdwUser, DWORD_PTR* lpParam1, DWORD_PTR* lpParam2);
typedef	WINMM_MapType	        (*MMDRV_UNMAPFUNC)(UINT wMsg, DWORD_PTR *lpdwUser, DWORD_PTR* lpParam1, DWORD_PTR* lpParam2, MMRESULT ret);

LPWINE_DRIVER	DRIVER_FindFromHDrvr(HDRVR hDrvr);
BOOL		DRIVER_GetLibName(LPCWSTR keyName, LPCWSTR sectName, LPWSTR buf, int sz);
LPWINE_DRIVER	DRIVER_TryOpenDriver32(LPCWSTR fn, LPARAM lParam2);
void            DRIVER_UnloadAll(void);

BOOL		MMDRV_Init(void);
void            MMDRV_Exit(void);
UINT		MMDRV_GetNum(UINT);
LPWINE_MLD	MMDRV_Alloc(UINT size, UINT type, LPHANDLE hndl, DWORD* dwFlags,
                            DWORD_PTR* dwCallback, DWORD_PTR* dwInstance, BOOL bFrom32);
void		MMDRV_Free(HANDLE hndl, LPWINE_MLD mld);
DWORD		MMDRV_Open(LPWINE_MLD mld, UINT wMsg, DWORD_PTR dwParam1, DWORD dwParam2);
DWORD		MMDRV_Close(LPWINE_MLD mld, UINT wMsg);
LPWINE_MLD	MMDRV_Get(HANDLE hndl, UINT type, BOOL bCanBeID);
LPWINE_MLD	MMDRV_GetRelated(HANDLE hndl, UINT srcType, BOOL bSrcCanBeID, UINT dstTyped);
DWORD           MMDRV_Message(LPWINE_MLD mld, UINT wMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2, BOOL bFrom32);
UINT		MMDRV_PhysicalFeatures(LPWINE_MLD mld, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
BOOL            MMDRV_Is32(unsigned int);
void            MMDRV_InstallMap(unsigned int, MMDRV_MAPFUNC, MMDRV_UNMAPFUNC,
                                 MMDRV_MAPFUNC, MMDRV_UNMAPFUNC, LPDRVCALLBACK);

const char* 	MCI_MessageToString(UINT wMsg);
DWORD           MCI_SendCommand(UINT wDevID, UINT16 wMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
LPWSTR          MCI_strdupAtoW(LPCSTR str);
LPSTR           MCI_strdupWtoA(LPCWSTR str);

BOOL            WINMM_CheckForMMSystem(void);
const char*     WINMM_ErrorToString(MMRESULT error);

UINT            MIDI_OutOpen(HMIDIOUT* lphMidiOut, UINT uDeviceID, DWORD_PTR dwCallback,
                             DWORD_PTR dwInstance, DWORD dwFlags, BOOL bFrom32);
UINT            MIDI_InOpen(HMIDIIN* lphMidiIn, UINT uDeviceID, DWORD_PTR dwCallback,
                            DWORD_PTR dwInstance, DWORD dwFlags, BOOL bFrom32);
MMRESULT        MIDI_StreamOpen(HMIDISTRM* lphMidiStrm, LPUINT lpuDeviceID,
                                DWORD cMidi, DWORD_PTR dwCallback,
                                DWORD_PTR dwInstance, DWORD fdwOpen, BOOL bFrom32);
UINT            WAVE_Open(HANDLE* lphndl, UINT uDeviceID, UINT uType,
                          LPCWAVEFORMATEX lpFormat, DWORD_PTR dwCallback, 
                          DWORD_PTR dwInstance, DWORD dwFlags, BOOL bFrom32);

void		TIME_MMTimeStop(void);

/* Global variables */
extern CRITICAL_SECTION WINMM_cs;
extern HINSTANCE hWinMM32Instance;
extern HANDLE psLastEvent;
extern HANDLE psStopEvent;

/* pointers to 16 bit functions (if sibling MMSYSTEM.DLL is loaded
 * NULL otherwise
 */
extern  WINE_MMTHREAD*  (*pFnGetMMThread16)(UINT16);
extern  LPWINE_DRIVER   (*pFnOpenDriver16)(LPCWSTR,LPCWSTR,LPARAM);
extern  LRESULT         (*pFnCloseDriver16)(UINT16,LPARAM,LPARAM);
extern  LRESULT         (*pFnSendMessage16)(UINT16,UINT,LPARAM,LPARAM);
extern  WINMM_MapType   (*pFnMciMapMsg32WTo16)(WORD,WORD,DWORD,DWORD_PTR*);
extern  WINMM_MapType   (*pFnMciUnMapMsg32WTo16)(WORD,WORD,DWORD,DWORD_PTR);
extern  LRESULT         (*pFnCallMMDrvFunc16)(DWORD /* in fact FARPROC16 */,WORD,WORD,LONG,LONG,LONG);
extern  unsigned        (*pFnLoadMMDrvFunc16)(LPCSTR,LPWINE_DRIVER, LPWINE_MM_DRIVER);
extern  void            (WINAPI *pFnReleaseThunkLock)(DWORD*);
extern  void            (WINAPI *pFnRestoreThunkLock)(DWORD);

/* GetDriverFlags() returned bits is not documented (nor the call itself)
 * Here are Wine only definitions of the bits
 */
#define WINE_GDF_EXIST	        0x80000000
#define WINE_GDF_16BIT	        0x10000000
#define WINE_GDF_EXTERNAL_MASK  0xF0000000
#define WINE_GDF_SESSION        0x00000001
