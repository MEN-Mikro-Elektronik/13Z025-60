/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *      \file    z25_mdis.c
 *
 *      \author  sv
 *        $Date: 2010/12/08 23:19:16 $
 *    $Revision: 1.7 $
 *
 *      \brief   13Z025 MDIS interface
 *
 *               This module consists of the 13Z025 MDIS interface. No further
 *               function calls are necessary to run the driver.
 *
 *     Switches: -
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 * - Z25_MdisDriver
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: z25_mdis.c,v $
 * Revision 1.7  2010/12/08 23:19:16  cs
 * R:1. device did not work when initialized using Z25_MdisDriver()
 *      obviously at transmit data was corrupted
 *   2. redundant definition of def values for descriptor keys
 *      in z25_driver.h and z25_mdis.h
 * M:1. set base baud before initializing first channel of an unit
 *   2. use defines from z25_driver.h
 *
 * Revision 1.6  2010/11/30 18:00:45  cs
 * R:1. MDIS integration interface not yet adapted to be able to set Rx and Tx FIFO
 *      trigger level separately (MZ25_SetFifoTriggerLevel prototype changed)
 *   2. used wrong default values for descriptor keys (not identical to ones in .xml file)
 * M:1.a) read Tx FIFO trigger level from descriptor
 *     b) use correct MZ25_SetFifoTriggerLevel call
 *   2. use identical default values for descriptor keys as defined in .xml file
 *
 * Revision 1.5  2008/06/09 17:15:54  SVogel
 * R:1. no possibiltiy to switch between chameleon and PCI interrupt
 * M:1. added compiler switch USE_PCI_IRQ
 *
 * Revision 1.4  2007/07/19 09:54:19  cs
 * added:
 *   + support for MEN PCI vendor ID
 * replaced IZ25_VENDOR_ID with define CHAMELEON_PCI_VENID_* from chameleon.h
 *
 * Revision 1.3  2007/07/11 14:43:59  SVogel
 * removed IZ25_MAX_SUPPORTED_TYPES
 *
 * Revision 1.2  2007/07/06 14:52:59  SVogel
 * cosmetics
 *
 * Revision 1.1  2005/07/06 10:14:56  SVogel
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include "vxWorks.h"            /* always first, CPU type, family , big/litte endian, etc. */
#include "sysLib.h"
#include "tyLib.h"

/* Standard ANSI libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MEN specific libraries */
#include <MEN/men_typs.h>
#include <MEN/chameleon.h>
#include <MEN/oss.h>

/* Used for MDIS integration */
#include <MEN/desc.h>
#include <MEN/os2m.h>
#include <MEN/mk.h>
#include <MEN/mdis_err.h>
#include <MEN/maccess.h>
#include <MEN/ll_defs.h>

#include <MEN/mz25_module.h>
#include <MEN/z25_driver.h>
#include <MEN/z25_mdis.h>
#include "z25_driver_int.h"

/*--------------------------------------*/
/*    DEFINES                           */
/*--------------------------------------*/
#ifdef DBG
#define DBG_MYLEVEL         (G_MdisLevel)
#endif /* DBG */

/*--------------------------------------+
|   CONSTANTS                           |
+---------------------------------------*/
/* supported vendor and device id */
LOCAL const u_int16 G_deviceIdent[][2]={
                    { CHAMELEON_PCI_VENID_MEN,    IZ25_DEVICE_ID_1 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_1 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_2 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_3 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_4 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_5 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_6 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_7 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_8 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_9 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_10 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_11 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_12 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_13 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_14 },
                    { CHAMELEON_PCI_VENID_ALTERA, IZ25_DEVICE_ID_15 }
};


static u_int32 G_MdisLevel = OSS_DBG_DEFAULT;

/*--------------------------------------*/
/*    LOCAL PROTOTYPES                  */
/*--------------------------------------*/
LOCAL int32 LocMdisPciPath( DESC_SPEC *boardDescP,
                            Z25_HDL *hdlP,
                            char *pciPathP);

LOCAL int32 LocGetComponentInfo(DESC_SPEC *deviceDescP,
                                Z25_HDL *hdlP,
                                u_int16 *vendorIdP,
                                u_int16 *deviceIdP,
                                u_int32*debugLevelP,
                                u_int16 *idCheckP);

LOCAL int32 LocGetChannelParameter(DESC_SPEC *deviceDescP,
                                   Z25_HDL *hdlP,
                                   u_int8 channel,
                                   u_int32 *fioBaudrateP,
                                   u_int16 *rxFifoLevelP,
                                   u_int16 *txFifoLevelP,
                                   u_int16 *physModeP,
                                   u_int16 *rxBuffSizeP,
                                   u_int16 *txBuffSizeP);

LOCAL int32 LocGetUartStartUnit(DESC_SPEC *deviceDescP,
                                Z25_HDL *hdlP,
                                u_int16 *startUnitP);

LOCAL STATUS LocPerformIdCheck(DESC_SPEC *deviceDescP,
                               Z25_HDL *hdlP);

/**********************************************************************/
/** Routine to get the PCI bus path
 *
 *  This routine reads the PCI bus path out of the board descriptor.
 *
 *  \param boardDescP       board descriptor, e.g. f206_1
 *  \param hdlP             Z25 handle returned by Z25_Init
 *  \param pciPathP         PCI bus path
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL int32 LocMdisPciPath( DESC_SPEC *boardDescP,
                            Z25_HDL *hdlP,
                            char *pciPathP)
{
    u_int32 length = 0;
    u_int32 slotNo = 0;
    int32 deviceNo = 0;
    u_int32 defaultValue = 0;
    Z25_DEV_TS *z25DevP = NULL;
    DESC_HANDLE *boardHdl = NULL;

    /* check input parameter */
    if( boardDescP == NULL )
    {
        return Z25_ERROR;
    }

    if( hdlP == NULL )
    {
        return Z25_ERROR;
    }
    else
    {
        z25DevP = hdlP;
    }

    /* get board descriptor handle */
    if( DESC_Init(boardDescP, NULL, &boardHdl) )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error initializing board descriptor !\n"));
        goto CLEANUP;
    }

    /* get pci bus path - mandatory */
    length = IZ25_PATH_LENGTH; /* initial value */
    if( DESC_GetBinary( boardHdl,
                        (u_int8 *)pciPathP,
                        length,
			(u_int8 *)pciPathP,
                        &length,
                        "PCI_BUS_PATH") )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error reading descriptor value PCI_BUS_PATH !\n"));
        goto CLEANUP;
    }

    /* get PCI device number - mandatory */
    defaultValue = 0;
    if( DESC_GetUInt32( boardHdl,
                        defaultValue,
			(u_int32*)&deviceNo,
                        "PCI_DEVICE_NUMBER" ) )
    {
        if( DESC_GetUInt32( boardHdl,
                            defaultValue,
                            &slotNo,
                            "PCI_BUS_SLOT" ) )
        {
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25 - Error reading descriptor value PCI_BUS_PATH !\n"));
            goto CLEANUP;
        }

        OSS_PciSlotToPciDevice(NULL, *(pciPathP+length-1), slotNo, &deviceNo);/**/
    }

    *(pciPathP+length) = (u_int8)(deviceNo & 0x000000ff);

    return Z25_OK;

    CLEANUP:
        if( boardHdl != NULL )
        {
            DESC_Exit( &boardHdl );
        }

        return Z25_ERROR;
}/*LocMdisPciPath*/

/**********************************************************************/
/** Routine to read the Z25 component information
 *
 *  This routine reads Z25 component information out of the device
 *  descriptor.
 *
 *  \param deviceDescP          device descriptor
 *  \param hdlP                 Z25 handle returned by Z25_Init
 *  \param vendorIdP            vendor ID
 *  \param deviceIdP            device ID
 *  \param debugLevelP          debug level information
 *  \param idCheckP             ID-check status
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL int32 LocGetComponentInfo(DESC_SPEC *deviceDescP,
                                Z25_HDL *hdlP,
                                u_int16 *vendorIdP,
                                u_int16 *deviceIdP,
                                u_int32*debugLevelP,
                                u_int16 *idCheckP)
{
    Z25_DEV_TS *z25DevP = NULL;   /* Z25 resources */
    DESC_HANDLE *deviceHdl = NULL;  /* device descriptor handle */
    u_int32 defaultValue = 0;       /* default descriptor value */
    u_int32 vendorId = 0;           /* vendor id */
    u_int32 deviceId = 0;           /* device id */
    u_int32 debugLevel = 0;         /* debug level */
    u_int32 idCheck = 0;            /* id check */

    /* check input parameter */
    if( deviceDescP == NULL )
    {
        return Z25_ERROR;
    }

    if( hdlP == NULL )
    {
        return Z25_ERROR;
    }
    else
    {
        z25DevP = hdlP;
    }

    /* get device descriptor handle */
    if( DESC_Init(deviceDescP, NULL, &deviceHdl) )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error initializing device descriptor !\n"));
        goto CLEANUP;
    }

    /* get vendor id */
    defaultValue = 0x0000;
    if( DESC_GetUInt32(deviceHdl,
                       defaultValue,
                       &vendorId,
                       "PCI_VENDOR_ID") )
    {
        DBGWRT_1((z25DevP->dbgHdlP,
            "Z25 - Warning key PCI_VENDOR_ID is disabled !\n"));
    }
    else
    {
        *vendorIdP = (u_int16)(vendorId & 0x0000ffff);
    }

    /* get device ID */
    defaultValue = 0x0000;
    if( DESC_GetUInt32(deviceHdl,
                       defaultValue,
                       &deviceId,
                       "PCI_DEVICE_ID") )
    {
        DBGWRT_1((z25DevP->dbgHdlP,
            "Z25 - Warning key PCI_DEVICE_ID is disabled !\n"));
    }
    else
    {
        *deviceIdP = (u_int16)(deviceId & 0x0000ffff);
    }

    /* get debug level */
    defaultValue = OSS_DBG_DEFAULT;
    if( DESC_GetUInt32( deviceHdl,
                        defaultValue,
                        &debugLevel,
                        "DEBUG_LEVEL") )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "Z25 - Error reading descriptor value DEBUG_LEVEL !\n"));
        goto CLEANUP;
    }
    else
    {
        *debugLevelP = debugLevel;
    }


    /* module ID check */
    defaultValue = 0;
    if( DESC_GetUInt32( deviceHdl,
                        defaultValue,
                        &idCheck,
                        "ID_CHECK" ) )
    {
        DBGWRT_1((z25DevP->dbgHdlP,
            "Z25 - Warning key ID_CHECK is disabled !\n"));
    }
    else
    {
        *idCheckP = (u_int16)(idCheck & 0x0000ffff);
    }

    /* everything was ok */
    return Z25_OK;

    CLEANUP:
        /* return error - parameter might be disabled */
        if( deviceHdl != NULL )
        {
            DESC_Exit( &deviceHdl );
        }

        return Z25_ERROR;
}/*LocGetComponentInfo*/

/**********************************************************************/
/** Routine to read out the channel parameter
 *
 *  This routine reads the channel parameters from the device descriptor.
 *
 *  \param deviceDescP              device descriptor
 *  \param hdlP                     Z25 handle returned by Z25_Init
 *  \param channel                  channel number
 *  \param fioBaudrateP             channel baudrate
 *  \param rxfifoLevelP             Receiver FIFO trigger level
 *  \param txfifoLevelP             Transmitter FIFO trigger level
 *  \param physModeP                physical mode (RS232, RS485, ...)
 *  \param rxBuffSizeP              Rx buffer size
 *  \param txBuffSizeP              Tx buffer size
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL int32 LocGetChannelParameter(DESC_SPEC *deviceDescP,
                                   Z25_HDL *hdlP,
                                   u_int8 channel,
                                   u_int32 *fioBaudrateP,
                                   u_int16 *rxFifoLevelP,
                                   u_int16 *txFifoLevelP,
                                   u_int16 *physModeP,
                                   u_int16 *rxBuffSizeP,
                                   u_int16 *txBuffSizeP)
{
    Z25_DEV_TS *z25DevP = NULL;   /* Z25 resources */
    DESC_HANDLE *deviceHdl = NULL;  /* device descriptor handle */
    u_int32 defaultValue = 0;       /* default descriptor value */
    u_int32 fioBaudrate = 0;        /* channel baudrate */
    u_int32 fifoLevel = 0;          /* fifo level */
    u_int32 physMode = 0;           /* physical mode */
    u_int32 rxBuffSize = 0;         /* rx buffer size */
    u_int32 txBuffSize = 0;         /* tx buffer size */

    /* check input parameter */
    if( deviceDescP == NULL )
    {
        return Z25_ERROR;
    }

    if( hdlP == NULL )
    {
        return Z25_ERROR;
    }
    else
    {
        z25DevP = hdlP;
    }

    /* get device descriptor handle */
    if( DESC_Init(deviceDescP, NULL, &deviceHdl) )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error initializing device descriptor !\n"));
        goto CLEANUP;
    }

    defaultValue = 0;
    if( DESC_GetUInt32( deviceHdl,
                    Z25_MDIS_DEFAULT_BAUD,
                    &fioBaudrate,
                    "CHANNEL_%d/FIOBAUDRATE", channel ) )
    {
        DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - Default value for FIOBAUDRATE is used (channel=%d)!\n",
            channel));
    }
    *fioBaudrateP = fioBaudrate;

    if( DESC_GetUInt32( deviceHdl,
                    Z25_RX_BUFF_SIZE,
                    &rxBuffSize,
                    "CHANNEL_%d/RX_BUFF_SIZE", channel ) )
    {
        DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - Default value for RX_BUFF_SIZE is used (channel=%d)!\n",
            channel));
    }
    *rxBuffSizeP = (u_int16)(rxBuffSize & 0x0000ffff);

    if( DESC_GetUInt32( deviceHdl,
                    Z25_TX_BUFF_SIZE,
                    &txBuffSize,
                    "CHANNEL_%d/TX_BUFF_SIZE", channel) )
    {
        DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - Default value for TX_BUFF_SIZE is used (channel=%d)!\n",
            channel));
    }
    *txBuffSizeP = (u_int16)(txBuffSize & 0x0000ffff);

    if( DESC_GetUInt32( deviceHdl,
                    Z25_RX_FIFO_TRIG_LEVEL,
                    &fifoLevel,
                    "CHANNEL_%d/FIFO_LEVEL", channel ) )
    {
        DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - Default value for FIFO_LEVEL is used (channel=%d)!\n",
            channel));
    }
    *rxFifoLevelP = (u_int16)(fifoLevel & 0x0000ffff);

    if( DESC_GetUInt32( deviceHdl,
                    Z25_TX_FIFO_TRIG_LEVEL,
                    &fifoLevel,
                    "CHANNEL_%d/TX_FIFO_LEVEL", channel ) )
    {
        DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - Default value for FIFO_LEVEL is used (channel=%d)!\n",
            channel));
    }
    *txFifoLevelP = (u_int16)(fifoLevel & 0x0000ffff);

     if( DESC_GetUInt32( deviceHdl,
                    Z25_MDIS_DEFAULT_PHYS_INT,
                    &physMode,
                    "CHANNEL_%d/PHYS_INT", channel ) )
    {
        DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - Default value for PHYS_INT is used (channel=%d)!\n",
            channel));
    }
    *physModeP = (u_int16)(physMode & 0x0000ffff);


    /* everything was ok */
    return Z25_OK;

    CLEANUP:
        if( deviceHdl != NULL )
        {
            DESC_Exit( &deviceHdl );
        }

        return Z25_ERROR;
}/*LocGetChannelParameter*/

/**********************************************************************/
/** Routine to get the UART start unit
 *
 *  This routine returns the first quad UART unit.
 *
 *  \param deviceDescP              device descriptor
 *  \param hdlP                     Z25 handle returned by Z25_Init
 *  \param startUnitP               first quad UART unit
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL int32 LocGetUartStartUnit(DESC_SPEC *deviceDescP,
                            Z25_HDL *hdlP,
                            u_int16 *startUnitP)
{
    Z25_DEV_TS *z25DevP = NULL;   /* Z25 resources */
    DESC_HANDLE *deviceHdl = NULL;  /* device descriptor handle */
    u_int32 startUnit = 0;          /* first unit to install */
    u_int32 defaultValue = 0;       /* default descriptor value */

    if( deviceDescP == NULL )
    {
        return Z25_ERROR;
    }

    if( hdlP == NULL )
    {
        return Z25_ERROR;
    }
    else
    {
        z25DevP = hdlP;
    }

    /* get device descriptor handle */
    if( DESC_Init(deviceDescP, NULL, &deviceHdl) )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error initializing device descriptor !\n"));
        goto CLEANUP;
    }

    defaultValue = 0;
    if( DESC_GetUInt32( deviceHdl,
                    defaultValue,
                    &startUnit,
                    "DEVICE_SLOT" ) )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error reading descriptor value DEVICE_SLOT !\n"));
        goto CLEANUP;
    }

    *startUnitP = (u_int16)(startUnit & 0x0000ffff);

    /* everything was ok */
    return Z25_OK;

    CLEANUP:
        if( deviceHdl != NULL )
        {
            DESC_Exit( &deviceHdl );
        }

        return Z25_ERROR;
}/*LocGetUartStartUnit*/

/**********************************************************************/
/** Routine to check the component ID
 *
 *  This routine checks the vendor and device ID on validity.
 *
 *  \param deviceDescP              device descriptor
 *  \param hdlP                     Z25 handle returned by Z25_Init
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocPerformIdCheck(DESC_SPEC *deviceDescP,
                         Z25_HDL *hdlP)
{
    Z25_DEV_TS *z25DevP = NULL;
    BOOL status = FALSE;
    u_int16 vendorId = 0;
    u_int16 deviceId = 0;
    u_int32 debugLevel = 0;
    u_int16 idCheck = 0;
    u_int8 i;

    /* check input parameter */
    if( deviceDescP == NULL )
    {
        return Z25_ERROR;
    }

    if( hdlP == NULL )
    {
        return Z25_ERROR;
    }
    else
    {
        z25DevP = hdlP;
    }

    if( LocGetComponentInfo(deviceDescP,
                            z25DevP,
                            &vendorId,
                            &deviceId,
                            &debugLevel,
                            &idCheck) != Z25_OK )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error getting descriptor values for id-check !\n"));
    }
    else
    {/* check if vendor and device id combination is valid */
        if( idCheck == 0 )
        {
            DBGWRT_1((z25DevP->dbgHdlP,
            "Z25 - Id-check not activated !\n"));
            status = TRUE;
        }

        for( i=0; i<15; i++)
        {
            if( (G_deviceIdent[i][0] == vendorId) &&
                (G_deviceIdent[i][1] == deviceId) )
            {
                status = TRUE;
                break;
            }
        }
    }

    Z25_SetDebugLevel(debugLevel);

    /* return true, if ID check was successful */
    if( status == TRUE )
    {
        return Z25_OK;
    }
    else
    {
        return Z25_ERROR;
    }
}/*LocPerformIdCheck*/

/*--------------------------------------*/
/*    PUBLIC PROTOTYPES                 */
/*--------------------------------------*/
/**
 * \defgroup _Z25_MDIS_GLOB_FUNC Z25 MDIS interface
 *  @{
 */
/**********************************************************************/
/** Routine to initialize the Z25 driver
 *
 *  This routine provides the MDIS initialization function for the
 *  the Z25 driver.
 *
 *  \param boardDescP                   board descriptor
 *  \param deviceDescP                  device descriptor
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_MdisDriver(DESC_SPEC *boardDescP, DESC_SPEC *deviceDescP)
{
    Z25_DEV_TS *z25DevP = NULL;         /* z25 resources */
    FUNCPTR intConnectAddr = NULL;      /* irq connect function address */
    FUNCPTR intEnableAddr = NULL;       /* irq enable function address */
    u_int8 i;                           /* loop index */
    char devName[64];                 /* device name buffer */
    char pciPath[16];                 /* pci path of device */
    u_int16 noOfUnits = 0;              /* number of uart units */
    u_int16 startUnit = 0;              /* first unit */
    u_int16 pathIndex = 0;              /* index of pci infos */
    u_int16 rxBuffSize = 0;             /* rx buffer size */
    u_int16 txBuffSize = 0;             /* tx buffer size */
    u_int16 rxFifoLevel = 0;            /* Rx fifo trigger level */
    u_int16 txFifoLevel = 0;            /* Tx fifo trigger level */
    u_int16 physMode = 0;               /* physical mode rs232, ... */
    u_int32 fioBaudrate = 0;            /* channel baudrate */

    bzero(pciPath, sizeof(pciPath));
    bzero(devName, sizeof(devName));

    /*------------------------------+
     | check input parameter        |
     +------------------------------*/
    if ( (boardDescP == NULL) || (deviceDescP == NULL))
    {
        return Z25_ERROR;
    }

    /*------------------------------+
     | initialization section       |
     +------------------------------*/
    /* create Z25 handle */
    if( (z25DevP=Z25_InitDriver()) == NULL )
    {
        printf("*** Error initializing driver !\n");
        goto CLEANUP;
    }

    G_MdisLevel = Z25_GetDebugLevel();

#ifdef USE_PCI_IRQ
    z25DevP->usePciIrq = 1;
#endif /* USE_PCI_IRQ */

    /* get pci path */
    if( LocMdisPciPath( boardDescP,
                        z25DevP,
                        pciPath) != Z25_OK )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error reading board descriptor values !\n"));
        goto CLEANUP;
    }

    /* check if device is supported by driver */
    if( LocPerformIdCheck(deviceDescP,
                          z25DevP) != Z25_OK )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error performing id-check !\n"));
    }

    /* get the start unit */
    if( LocGetUartStartUnit(deviceDescP,
                        z25DevP,
                        &startUnit) != Z25_OK )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Error getting start unit!\n"));
        goto CLEANUP;
    }

    /* get MDIS interrupt routines */
    intConnectAddr = MK_GetIntConnectRtn();
    intEnableAddr = MK_GetIntEnableRtn();

    /* set interrupt functions */
    /* irqBase, depends on processor architecture */
    Z25_SetIntFunctions(z25DevP, OSS_GetIrqNum0(), intConnectAddr, intEnableAddr);

    /*-----------------------------+
     | find units                  |
     +-----------------------------*/

    if( Z25_GetPciPathInfo((Z25_HDL *)z25DevP,
		    	    (int8 *)pciPath,
                           &pathIndex) != Z25_OK )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Unknown pci bus path !\n"));
        goto CLEANUP;
    }

    if( z25DevP->pathInfo[pathIndex].installed == FALSE )
    {
        u_int16 tmpStartUnit = 0;
        if( Z25_FindUartUnits( z25DevP,
                                pathIndex,
                                &tmpStartUnit,
                                &noOfUnits) != Z25_OK )
        {
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25 - Error initializing chameleon units.\n"));
            goto CLEANUP;
        }
    }

    /* check if unit was found in FPGA */
    if( startUnit == z25DevP->pathInfo[pathIndex].no16Z25Units )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Unknown uart unit number %d !\n", startUnit));
        goto CLEANUP;
    }

    /*------------------------------+
     | driver installation section  |
     +------------------------------*/
    if( Z25_GetIosDriverNumber((Z25_HDL *)z25DevP, startUnit) != Z25_OK )
    {
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - !!! Z25_GetIosDriverNumber !!!!\n"));
        goto CLEANUP;
    }

    z25DevP->pathInfo[pathIndex].installed = TRUE;

    /* read channel 0..3 initialization values if property enabled */
    for( i=0; i<Z25_MAX_UARTS_PER_DEV; i++ )
    {
        HDL_16Z25 *unitHdlP;
        unitHdlP = z25DevP->quadUart[startUnit][i].unitHdlP;

        if( z25DevP->quadUart[startUnit][i].addr == 0 )
        {/* invalid address - continue loop */
            continue;
        }

        if( LocGetChannelParameter(deviceDescP,
                                   z25DevP,
                                   i,
                                   &fioBaudrate,
                                   &rxFifoLevel,
                                   &txFifoLevel,
                                   &physMode,
                                   &rxBuffSize,
                                   &txBuffSize) != Z25_OK )
        {
            DBGWRT_2((z25DevP->dbgHdlP,
            "Z25 - No descriptor values for channel %d!\n", i));
        }

		/* set base baud for this unit, only once, on channel 0 */
		if( 0 == i )
	    	Z25_SetBaseBaud((Z25_HDL *)z25DevP, 1843200, startUnit);


        sprintf(devName, "/tyZ25_%d", pathIndex);
        if( Z25_InstallTtyInterface( (Z25_HDL *)z25DevP,
        			    (int8 *)devName,
                                    startUnit,
                                    i,
                                    rxBuffSize,
                                    txBuffSize) != Z25_OK )
        {
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25 - !!! Z25_InstallTtyInterface on channel %d!!!!\n",
                i));
        }

        /* set MDIS initial parameter */
        if( fioBaudrate > 0 )
        {
            MZ25_SetBaudrate(unitHdlP, fioBaudrate);
        }

        MZ25_SetFifoTriggerLevel(unitHdlP, 0, rxFifoLevel);
        MZ25_SetFifoTriggerLevel(unitHdlP, 1, txFifoLevel);

        if( physMode > 0 )
        {
            MZ25_SetSerialMode(unitHdlP, physMode);
        }

        bzero(devName, sizeof(devName));
    }

    DBGWRT_2((z25DevP->dbgHdlP,
        "Z25 - Driver initialization finished.\n"));

    DBGWRT_2((z25DevP->dbgHdlP,
        "Z25 - Z25_CreateDevice\n"));

    return Z25_OK;

    CLEANUP:
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25 - Driver installation aborted !\n"));
        if( z25DevP != NULL )
        {
            Z25_FreeHandle((Z25_HDL *)&z25DevP);
        }

        return Z25_ERROR;
}/*Z25_MdisDriver*/

/**********************************************************************/
/** Routine to provide the entry function for the MDIS kernel
 *
 *  This routine provides the entry function for the MDIS kernel. It is
 *  the first function that is called by the MDIS kernel.
 *
 *  \param drvP         driver functions (not used !)
 *
 *  \return no return value
 */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    extern void LL_GetEntry( LL_ENTRY* drvP )
#else

#if (defined (Z25_SW))
    extern void Z25_SW_GetEntry( LL_ENTRY* drvP )
#else
    extern void Z25_GetEntry( LL_ENTRY* drvP )
#endif /* ! (defined ((Z25_SW)))) */
#endif /* ! _ONE_NAMESPACE_PER_DRIVER_ */
{
    /* nothing to be done */
}/* LL_GetEntry/Z25_GetEntry/Z25_SW_GetEntry */
/*! @} */




