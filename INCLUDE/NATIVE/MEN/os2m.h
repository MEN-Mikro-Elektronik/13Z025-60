/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: os2m.h
 *
 *      $Author: UFranke $
 *        $Date: 2010/09/02 16:33:38 $
 *    $Revision: 2.4 $
 *
 *  Description: OS to MDIS interface
 *
 *     Switches: OS2M_MAX_DEVS default 50
 *               INCLUDE_MIPIOS_VX_TARGET for MIPIOS targets 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: os2m.h,v $
 * Revision 2.4  2010/09/02 16:33:38  UFranke
 * R: OS2M_IdentString removed
 * M: OS2M_Ident() added
 *
 * Revision 2.3  2009/02/19 10:04:57  ufranke
 * prepared
 *  + for MIPIOS
 *
 * Revision 2.2  2008/01/25 14:11:32  ufranke
 * changed
 *  - OS2M_MAX_DEVS configurable by mdis Makefile now
 *    i.e. ADDED_CFLAGS = -DOS2M_MAX_DEVS=...
 *
 * Revision 2.1  2005/04/05 16:45:00  ufranke
 * added
 *  + OS2M_Show()
 *
 * Revision 2.0  2000/03/17 15:07:39  kp
 * adapted to MK 2.0
 *
 * Revision 1.3  1999/08/31 12:14:32  Franke
 * MDIS 4.3
 * changed OS2M_DevRemove parameter p (OS2M_DevHandle) to device name
 *
 * Revision 1.2  1998/06/24 10:46:33  Franke
 * added  lockMode (prepared for future releases - channel/device locking)
 *
 * Revision 1.1  1998/03/10 12:21:43  franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef INC_OS2M
#define INC_OS2M

#ifdef __cplusplus
   extern "C" {
#endif


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

typedef void*  OS2M_MK_HANDLE;

typedef struct
{
     /*--------------------------+
     |       driver control      |
     +--------------------------*/
     char			*devName;		 /* name of the device e.g. "/m055/0" */
     u_int32		 devNameSize;	 /* size of memory for devName*/
     int32			mipiosFlag;		 /* 0 local, 1- MIPIOS */
}OS2M_DEV_HDR;



/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define OS2M_ERROR      -2          /* say to mdis_api.c that error comes from MDIS */

#define OS2M_SCOD_ADD   0x2000      /* offset to do setstat */
#define OS2M_NOPATH     -1


#ifndef OS2M_MAX_DEVS
	#define OS2M_MAX_DEVS   50
#endif /* OS2M_MAX_DEVS */

#define OS2M_MIPIOS_FILE_HANDLE_START		100000 /* MIPIOS file handle starting at */

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
extern OS2M_DEV_HDR   *OS2M_DevTbl[OS2M_MAX_DEVS];

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

extern STATUS OS2M_DrvInstall( void );
extern STATUS OS2M_DrvRemove(  void );
extern STATUS OS2M_DevCreate(  char      *devName,
                               DESC_SPEC *llDescSpec,
                               DESC_SPEC *bbDescSpec
                            );
extern STATUS OS2M_DevRemove( char *device );
extern int32 OS2M_DevTblLock(   void );
extern int32 OS2M_DevTblUnLock( void );

extern void OS2M_ErrnoSet( int errorNbr );


extern int32  OS2M_Open(  char *device );
extern STATUS OS2M_Close( int32 path   );

extern void OS2M_DbgLevelSet(       u_int32 dbgLevel );
extern void OS2M_DbgLevelGet(       u_int32 *dbgLevelP );

extern void OS2M_Show( void );

extern char *OS2M_Ident( void );

extern STATUS OS2M_MipiosDevCreate( char *devName, OS2M_DEV_HDR *devHdl ); /* for MIPIOS internal use only */
	#ifdef INCLUDE_MIPIOS_VX_TARGET
		extern int32  OS2M_SetLlDescForMipios( char	*devName, DESC_SPEC	llDescSpec );
	#endif /*INCLUDE_MIPIOS_VX_TARGET*/

#ifdef __cplusplus
   }
#endif
#endif /*INC_OS2M*/



