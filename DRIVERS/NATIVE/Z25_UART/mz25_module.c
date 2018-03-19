/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mz25_module.c
 *
 *      \author  sv
 *        $Date: 2012/05/14 18:58:13 $
 *    $Revision: 1.8 $
 *
 *        \brief  16Z025 low-level functions
 *
 *            	  This module provides the low-level functions for the 13Z025
 *                driver. These functions are used to access the 16Z025
 *                registers.
 *
 *     Switches: -
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 *   - MZ25_ModuleId
 *   - MZ25_InitHandle
 *   - MZ25_FreeHandle
 *   - MZ25_EnableInterrupt
 *   - MZ25_DisableInterrupt
 *   - MZ25_SetBaudrate
 *   - MZ25_GetBaudrate
 *   - MZ25_SetSerialParameter
 *   - MZ25_SetDatabits
 *   - MZ25_SetStopbits
 *   - MZ25_SetParity
 *   - MZ25_SetFifoTriggerLevel
 *   - MZ25_SetRts
 *   - MZ25_EnableAutoRtsCts
 *   - MZ25_SetDtr
 *   - MZ25_SetOut1
 *   - MZ25_SetOut2
 *   - MZ25_GetCts
 *   - MZ25_GetDsr
 *   - MZ25_GetDcd
 *   - MZ25_SetSerialMode
 *   - MZ25_GetSerialMode
 *   - MZ25_SetLineStatus
 *   - MZ25_GetLineStatus
 *   - MZ25_SetBaseBaud
 *   - MZ25_ControlModemTxInt
 *   - MZ25_SetModemControl
 *   - MZ25_GetModemControl
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mz25_module.c,v $
 * Revision 1.8  2012/05/14 18:58:13  channoyer
 * R: Not possible to read the UART mode.
 * M: Add the function MZ25_GetSerialMode().
 *
 * Revision 1.7  2010/12/08 23:02:12  cs
 * R: handling of fifo levels was a little confusing
 * M: added clarifying comment
 *
 * Revision 1.6  2009/05/08 18:25:17  cs
 * R: mz25_module functions did not use same debug level as z25_driver
 *    this is very confusing for debugging
 * M: use global G_Z25_DebugLevel for both modules
 *
 * Revision 1.5  2008/06/09 17:15:46  SVogel
 * R:1. each transmitted byte generated an interrupt,
 *      if fifo trigger level was set to 1
 * M:1. splitted fifoTrigger into fifoRxTrigger and fifoTxTrigger
 *   2. changed MZ25_SetFifoTriggerLevel interface (added 'rxTx' parameter)
 *
 * Revision 1.4  2008/02/26 11:39:31  cs
 * R:1. Baudrate was not read and computed correctly
 *   2. Even when DLAB set IP-Core Z125 did not access divisor latch register
 *   3. Compiler warning about redefinition of DBG when BSP compiled for debugs
 * M:1. Change all variables holding the baud rate to u_int32
 *   2. Add bugfix (additional LCR read) in MZ25_SetBaudrate/GetBaudrate
 *   3. Remove definition of DBG
 *
 * Revision 1.3  2007/07/06 14:52:49  SVogel
 * + cosmetics
 * + added automatic rts/cts handshake
 * + adaption of setting trigger level for z125 uarts
 *
 * Revision 1.2  2005/07/06 08:58:26  SVogel
 * removed bug in baudrate functions
 *
 * Revision 1.1  2005/06/23 08:53:05  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
static char *G_rcsId="$Id: mz25_module.c,v 1.8 2012/05/14 18:58:13 channoyer Exp $ Exp $ build " __DATE__" "__TIME__ ;

/* VxWorks specific includes */
#include "vxWorks.h"
#include "sysLib.h"

/* MEN specific includes */
#include <MEN/men_typs.h>
#include <MEN/oss.h>

/* Module related includes */
#include <MEN/mz25_module.h>

/* Standard ANSI C includes */
#include <stdlib.h>
#include <stdio.h>

/*--------------------------------------*/
/*    DEFINES                           */
/*--------------------------------------*/
#ifdef DBG
#undef DBG_MYLEVEL
#define DBG_MYLEVEL			(G_Z25_DebugLevel) /* DBG_ALL */
extern u_int32 G_Z25_DebugLevel;
#endif /* DBG */

/**
 * \defgroup _MZ25_GLOB_FUNC Z25 low-level functions
 *  @{
 */
/**********************************************************************/
 /** Routine to get the module identification
  *
  *  This routine prints the build date, time, module name and author
  *  to the standard output (console).
  *
  *  \return no return value
 */
void MZ25_ModuleId(void){
    printf("%s\n", G_rcsId );
}/* MZ25_ModuleId */

/**********************************************************************/
 /** Routine to initialize module handle
  *
  *  This routine allocates the memory for the 16Z025 module handle.
  *  All elements of the 16Z025 module structure are set to a
  *  defined state. If debugging is enabled, the debug library will
  *  be initialized.
  *
  *	 \param hdlP			16Z025 handle
  *	 \param address			UART channel address
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_InitHandle(HDL_16Z25 **hdlP, u_int32 address){
	TS_16Z25_UNIT *mZ25P;   /* 16Z025 resources */
	u_int32 gotSize = 0;      /* size of memory block */

    if( (mZ25P = (TS_16Z25_UNIT *)
        OSS_MemGet(NULL, sizeof(TS_16Z25_UNIT), &gotSize)) == NULL ){
		return MZ25_ERROR;
	}

	mZ25P->address = address;
 	mZ25P->baudrate = 0;
	mZ25P->stopbits = 0;
	mZ25P->databits = 0;
	mZ25P->parity = 0;
	mZ25P->fifoRxTrigger = 0;
	mZ25P->fifoTxTrigger = 0;
	mZ25P->lineStatus = 0;

	mZ25P->dlabSet = FALSE;

        mZ25P->modemControl = FALSE;
	mZ25P->rts = FALSE;
	mZ25P->dtr = FALSE;
	mZ25P->out1 = FALSE;
	mZ25P->out2 = FALSE;
	mZ25P->cts = FALSE;
	mZ25P->dsr = FALSE;
	mZ25P->dcd = FALSE;
	mZ25P->rtsCtsHs = FALSE;
	mZ25P->uartFreq = 1843200;
	mZ25P->divisorConst = 16;

	DBGINIT(("16Z025Unit", (DBG_HANDLE **)&mZ25P->dbgHdlP));

    DBGWRT_1((mZ25P->dbgHdlP, "MZ25_InitHandle \n"));

	*hdlP = mZ25P;

	return MZ25_OK;
}/* MZ25_InitHandle */

/**********************************************************************/
 /** Routine to free module handle
  *
  *  This routine frees the memory, which was allocated by
  *  MZ25InitHandle. If debugging is enbabled, the debug resources
  *  will be freed, too.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_FreeHandle(HDL_16Z25 **hdlP){
	TS_16Z25_UNIT *mZ25P;   /* 16Z025 resources */

	mZ25P = *hdlP;

	if( mZ25P == NULL ){
		return MZ25_ERROR;
	}

	DBGWRT_1((mZ25P->dbgHdlP,
		"MZ25_FreeHandle\n"));

	DBGEXIT((&mZ25P->dbgHdlP));

	OSS_MemFree(NULL, (void *)mZ25P, sizeof(TS_16Z25_UNIT));
	mZ25P = NULL;

	*hdlP = mZ25P;

	return MZ25_OK;
}/* MZ25_FreeHandle */

/**********************************************************************/
 /** Routine to set the baudrate
  *
  *  This routine sets the baudrate for each 16Z025 unit channel.
  *	 The baudrate is set as a decimal value, e.g. 115200bd=115200,
  *  57600bd=57600, ... .
  *  Baudrate values up to 115200 baud are supported. If the specified
  *  value is not supported the baudrate is set to 115200 baud.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			baudrate value
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetBaudrate(HDL_16Z25 *hdlP,
					     int32 value){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
	u_int8 lowByte = 0;         /* low byte */
	u_int8 highByte = 0;        /* high byte */
	u_int16 regBaudDiv = 0;     /* baudrate divisor */
	u_int8 intStored = 0;       /* active interrupt */
	u_int8 regVal = 0;

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }
    DBGWRT_1((tmphdlP->dbgHdlP, "MZ25_SetBaudrate %d\n", value));

    if( ((value > 0) && (value > 3000000)) ||
    	(value < 0) )
    {
        DBGWRT_ERR((tmphdlP->dbgHdlP, "*** Unsupported baudrate (%d)- baudrate is set to 115200 baud\n",value));
		value = 115200;
    }

	tmphdlP->baudrate = (u_int32)value;

    regBaudDiv = (u_int16)((tmphdlP->uartFreq/(value*tmphdlP->divisorConst)&0x0000FFFF));

    lowByte = (u_int8)(regBaudDiv & 0x00ff);
    highByte = (u_int8)((regBaudDiv & 0xff00) >> 8);

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	/* store LCR register */
	regVal = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET,(MIZ25_DLAB|regVal));

	tmphdlP->dlabSet = TRUE;

	/* TBD: bugfix?
	 * without the following read the access to the
	 * divisor latch registers are not valid! */
    MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);

	/* write baudrate value to register */
	MZ25_REG_WRITE(tmphdlP->address, MIZ25_DLL_OFFSET, lowByte);
	MZ25_REG_WRITE(tmphdlP->address, MIZ25_DLH_OFFSET, highByte);

    DBGWRT_3( (tmphdlP->dbgHdlP, "MZ25_SetBaudrate(0x%x): DLH = 0x%x.\n",
	        tmphdlP->address+MIZ25_DLH_OFFSET,
	        MZ25_REG_READ(tmphdlP->address,
	        MIZ25_DLH_OFFSET)) );
	DBGWRT_3( (tmphdlP->dbgHdlP, "MZ25_SetBaudrate(0x%x): DLL = 0x%x.\n",
	        tmphdlP->address+MIZ25_DLL_OFFSET,
	        MZ25_REG_READ(tmphdlP->address, MIZ25_DLL_OFFSET)) );

	/* restore LCR register */
	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, regVal);

	MZ25_EnableInterrupt(hdlP, intStored);

	DBGWRT_3( (tmphdlP->dbgHdlP, "MZ25_SetBaudrate(0x%x): DLAB = 0x%x.\n",
	    tmphdlP->address,
	    MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET)&MIZ25_DLAB) );

	tmphdlP->dlabSet = FALSE;

	return MZ25_OK;
}/* MZ25_SetBaudrate */

/**********************************************************************/
 /** Routine to get the current baudrate
  *
  *  This routine reads out the baudrate registers and returns the
  *  current baudrate. The baudrate is set as a decimal value,
  *  e.g. 115200bd=115200, 57600bd=57600, ... .
  *
  *	 \param hdlP			16Z025 module handle
  *	 \param valueP          baudrate value
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_GetBaudrate(HDL_16Z25 *hdlP,
					    int32 *valueP)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 lowByte = 0;         /* low byte */
	u_int8 highByte = 0;        /* high byte */
	u_int8 regVal = 0;          /* register value */
	u_int8 intStored;           /* active interrupts */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP, "MZ25_GetBaudrate\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	/* store LCR register */
	regVal = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, (MIZ25_DLAB | regVal));
	tmphdlP->dlabSet = TRUE;

	/* TBD: bugfix?
	 * without the following read the access to the
	 * divisor latch registers are not valid! */
    MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);

	/* write baudrate value to register */
	lowByte = MZ25_REG_READ(tmphdlP->address, MIZ25_DLL_OFFSET);
	highByte = MZ25_REG_READ(tmphdlP->address, MIZ25_DLH_OFFSET);

    DBGWRT_2( (tmphdlP->dbgHdlP, "MZ25_GetBaudrate(0x%x): DLH = 0x%x.\n",
	    tmphdlP->address+MIZ25_DLH_OFFSET, highByte) );
	DBGWRT_2( (tmphdlP->dbgHdlP, "MZ25_GetBaudrate(0x%x): DLL = 0x%x.\n",
	    tmphdlP->address+MIZ25_DLL_OFFSET, lowByte) );

	*valueP = (highByte << 8) + lowByte;

    *valueP = (u_int32)(tmphdlP->uartFreq/(*valueP*tmphdlP->divisorConst));
    DBGWRT_3( (tmphdlP->dbgHdlP, "MZ25_GetBaudrate(0x%x): baudrate=%d.\n",
	    tmphdlP->address, *valueP) );

	/* restore LCR register */
	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, regVal);

    MZ25_EnableInterrupt(hdlP, intStored);

	tmphdlP->dlabSet = FALSE;

    tmphdlP->baudrate = *valueP;

    return MZ25_OK;
}/* MZ25_GetBaudrate */

/**********************************************************************/
 /** Routine to set the serial port parameters
  *
  *  This routine sets the serial transmission parameter (stopbits,
  *  databits and parity).
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			serial parameter value
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetSerialParameter(HDL_16Z25 *hdlP,
							  u_int8 value){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 intStored = 0;       /* active interrupt */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetSerialParameter\n"));

	tmphdlP->stopbits = (value & (MIZ25_NOSTP));
	tmphdlP->databits = (value & (MIZ25_WL0+MIZ25_WL1));
	tmphdlP->parity = (value & (MIZ25_PEN+MIZ25_PTYPE));

    intStored = MZ25_DisableInterrupt(hdlP, 0);
	if( tmphdlP->dlabSet == TRUE ){
	    /* DLAB set - it must be unset */
		u_int8 tmp;

		tmp = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
		tmp &= ~MIZ25_DLAB;

		MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, tmp);

		tmphdlP->dlabSet = FALSE;
	}
    /* write serial parameter */
	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, value);

	MZ25_EnableInterrupt(hdlP, intStored);

	DBGWRT_2( (tmphdlP->dbgHdlP,
	    "MZ25_SetSerialParameter(0x%x): LCR = 0x%x.\n",
	    tmphdlP->address+MIZ25_LCR_OFFSET, value) );

	return MZ25_OK;
}/* MZ25_SetSerialParameter */

/**********************************************************************/
 /** Routine to set the number of data bits
  *
  *  This routine sets the databits used for serial transmission.
  *  Use value=5..8 for setting the databits
  *
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			number of databits
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetDatabits(HDL_16Z25 *hdlP,
					   int32 value)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 lcrReg = 0;          /* lcr register */
    u_int8 intStored = 0;       /* active interrupts */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetDatabits\n"));

	switch(value){
		case 5:
			tmphdlP->databits = MZ25_DATABITS_5;
			break;
	    case 6:
	    	tmphdlP->databits = MZ25_DATABITS_6;
	    	break;
	    case 7:
	    	tmphdlP->databits = MZ25_DATABITS_7;
	    	break;
	    case 8:
	    default:
	    	tmphdlP->databits = MZ25_DATABITS_8;
	    	break;
	}

	DBGWRT_2( (tmphdlP->dbgHdlP, "MZ25_SetDatabits(0x%x): value=%d.\n",
	    tmphdlP->address, value) );

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	if( tmphdlP->dlabSet == TRUE ){
	    /* DLAB set - it must be unset */
		u_int8 tmp;

		tmp = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
		tmp &= ~MIZ25_DLAB;

		MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, tmp);

		tmphdlP->dlabSet = FALSE;
	}

	lcrReg = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
	lcrReg &= ~(MIZ25_WL0 | MIZ25_WL1);
	lcrReg |= tmphdlP->databits;

	DBGWRT_3( (tmphdlP->dbgHdlP, "MZ25_SetDatabits(0x%x): LCR=0x%x.\n",
	    tmphdlP->address+MIZ25_LCR_OFFSET, lcrReg) );

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, lcrReg);

    MZ25_EnableInterrupt(hdlP, intStored);

    return MZ25_OK;
}/* MZ25_SetDatabits */

/**********************************************************************/
 /** Routine to set the number of stop bits
  *
  *  This routine sets the stopbits used for serial transmission.
  *  Use value=1-one stopbits, 2-two stopbits.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			number of stopbits
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetStopbits(HDL_16Z25 *hdlP,
					   int32 value)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 lcrReg = 0;          /* 16Z025 resources */
    u_int8 intStored = 0;       /* active interrupts */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetStopbits\n"));

    switch(value){
		case 1:
		default:
			tmphdlP->stopbits = MZ25_STOPBITS_1;
			break;
	    case 2:
	    	tmphdlP->stopbits = MZ25_STOPBITS_2;
	    	break;
	}

    intStored = MZ25_DisableInterrupt(hdlP, 0);
	if( tmphdlP->dlabSet == TRUE ){
	    /* DLAB set - it must be unset */
		u_int8 tmp;

		tmp = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
		tmp &= ~MIZ25_DLAB;

		MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, tmp);

		tmphdlP->dlabSet = FALSE;
	}

	lcrReg = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
	lcrReg &= ~MIZ25_NOSTP;
	lcrReg |= tmphdlP->stopbits;

	DBGWRT_2( (tmphdlP->dbgHdlP, "MZ25_SetStopbits(0x%x): LCR=0x%x.\n",
	    tmphdlP->address+MIZ25_LCR_OFFSET, lcrReg) );

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, lcrReg);

    MZ25_EnableInterrupt(hdlP, intStored);

    return MZ25_OK;
}/* MZ25_SetStopbits */

/**********************************************************************/
 /** Routine to set the parity
  *
  *  This routine sets the parity for serial transmission.
  *  Use value=0-no parity, 1-even parity and 2-odd parity.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			parity
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetParity(HDL_16Z25 *hdlP,
					 int32 value){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 lcrReg = 0;          /* lcr register */
    u_int8 intStored = 0;       /* active interrupt */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetParity\n"));

    switch(value){
		case 0:
		default:
			tmphdlP->parity = MZ25_NO_PARITY;
			break;
		case 1:
			tmphdlP->parity = MZ25_EVEN_PARITY;
			break;
		case 2:
			tmphdlP->parity = MZ25_ODD_PARITY;
			break;
	}

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	if( tmphdlP->dlabSet == TRUE ){
	    /* DLAB set - it must be unset */
		u_int8 tmp;

		tmp = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
		tmp &= ~MIZ25_DLAB;

		MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, tmp);

		tmphdlP->dlabSet = FALSE;
	}

	lcrReg = MZ25_REG_READ(tmphdlP->address, MIZ25_LCR_OFFSET);
	lcrReg &= ~(MIZ25_PEN | MIZ25_PTYPE);
	lcrReg |= tmphdlP->parity;

	DBGWRT_2( (tmphdlP->dbgHdlP, "MZ25_SetParity(0x%x): LCR=0x%x.\n",
	    tmphdlP->address+MIZ25_LCR_OFFSET, lcrReg) );

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_LCR_OFFSET, lcrReg);

    MZ25_EnableInterrupt(hdlP, intStored);

    return MZ25_OK;
}/* MZ25_SetParity */

/**********************************************************************/
 /** Routine to set the FIFO trigger level
  *
  *  This routine sets the FIFO trigger lever. Accepted values are 1,
  *  4, 30 or 58 bytes. Use define FIFO_x_BYTE for the parameter value
  *  to set the trigger level.
  *
  *	 \param hdlP			16Z025 module handle
  *	 \param rxTx                    0 = set rx trigger level
  *                                     > 0 set tx trigger level
  *  \param value			FIFO buffer size (1, 4, 30 or 58 bytes)
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetFifoTriggerLevel(HDL_16Z25 *hdlP,
                                u_int8 rxTx,
							   u_int8 value)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 mask = 0;            /* mask */
    u_int8 intStored = 0;       /* active interrupts */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
	"MZ25_SetFifoTriggerLevel: rxTx=%d, value=%d\n",
	rxTx, value));

    if( rxTx == 0 ){
        tmphdlP->fifoRxTrigger = value;
    }
    else {
        tmphdlP->fifoTxTrigger = value;
        return MZ25_OK;
    }/* end if */

	/* only Rx FIFO level is handled in HW and has to be programmed here */
    switch(value){
        case 1:/* Z025/Z125 uart */
            value =  MZ25_FIFO_1_BYTE;
            break;

        case 4 :/* Z025 uart */
        case 8 :/* Z125 uart */
            value =  MZ25_FIFO_4_BYTE;
            break;

        case 30 :/* Z025 uart */
        case 60 :/* Z125 uart */
            value =  MZ25_FIFO_30_BYTE;
            break;

        case 58 :
        case 116:/* Z125 uart */
            value =  MZ25_FIFO_58_BYTE;
            break;
        default:
            value =  MZ25_FIFO_1_BYTE;
            tmphdlP->fifoRxTrigger = 1;
            break;
    }

    DBGWRT_2( (tmphdlP->dbgHdlP,
        "MZ25_SetFifoTriggerLevel(0x%x): fifoRxTrigger=%d.\n",
        tmphdlP->address, tmphdlP->fifoRxTrigger) );

	mask = (value | MIZ25_FIFOEN);

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_FCR_OFFSET, mask);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetFifoTriggerLevel(0x%x): FCR=0x%x written.\n",
	    tmphdlP->address, mask) );

    MZ25_EnableInterrupt(hdlP, intStored);

    return MZ25_OK;
}/* MZ25_SetFifoTriggerLevel */

/**********************************************************************/
 /** Routine to enable the UART interrupts
  *
  *  This routine enables the UART interrupts. Use the defines RDAIEN,
  *  THREIEN, RLSIEN or MSIEN. To set two or more interrupts use e.g.
  *  value=(RDAIEN|MSIEN). Already activated interrupts will not be
  *  overwritten !
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			interrupt mask
  *
  *  \return active interrupts, before enable
 */
u_int8 MZ25_EnableInterrupt(HDL_16Z25 *hdlP,
							 u_int8 value)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 storedVal = 0;       /* stored value */
    u_int8 retVal = 0;          /* return value */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_EnableInterrupt\n"));

    storedVal = retVal =
        MZ25_REG_READ(tmphdlP->address, MIZ25_IER_OFFSET);

    storedVal |= value;

    DBGWRT_4( (tmphdlP->dbgHdlP,
    	"MZ25_EnableInterrupt(0x%x): IER=0x%x read.\n",
	    tmphdlP->address, storedVal) );

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_IER_OFFSET, storedVal);

	DBGWRT_4( (tmphdlP->dbgHdlP,
		"MZ25_EnableInterrupt(0x%x): IER=0x%x written.\n",
	    tmphdlP->address, storedVal) );

	return retVal;
} /* MZ25_EnableInterrupt */

/**********************************************************************/
 /** Routine to disable the interrupts
  *
  *  This routine disables activated interrupts. If the value is 0, all
  *  interrupts are masked and will be disabled. Otherwise use the defines
  *  RDAIEN, THREIEN, RLSIEN or MSIEN to disable a specific interrupt or
  *  an interrupt group, e.g. value=(RDAIEN|MSIEN).
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value			interrupt mask
  *
  *  \return active interrupts, before disable
 */
u_int8 MZ25_DisableInterrupt(HDL_16Z25 *hdlP,
							  u_int8 value)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 regVal = 0;          /* register value */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_3((tmphdlP->dbgHdlP,
		"MZ25_DisableInterrupt\n"));

    regVal = MZ25_REG_READ(tmphdlP->address, MIZ25_IER_OFFSET);

    if( value == 0 ){
        MZ25_REG_WRITE(tmphdlP->address, MIZ25_IER_OFFSET, value);
    }
    else{
        regVal &= ~value;
        MZ25_REG_WRITE(tmphdlP->address, MIZ25_IER_OFFSET, regVal);
    }

    DBGWRT_4( (tmphdlP->dbgHdlP,
    	"MZ25_DisableInterrupt(0x%x): IER=0x%x.\n",
	    tmphdlP->address, regVal) );

	return regVal;
} /* MZ25_DisableInterrupt */

/**********************************************************************/
 /** Routine to set the RTS line
  *
  *  This routine sets the RTS line to logical '1' or logical '0'.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param status			true=RTS high, false=RTS low
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetRts(HDL_16Z25 *hdlP,
					BOOL status)
{
	TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
	u_int8 intStored = 0;       /* active interrupts */
	u_int8 mask = 0;            /* mask */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetRts\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	mask = MZ25_REG_READ(tmphdlP->address, MIZ25_MCR_OFFSET);

    DBGWRT_3( (tmphdlP->dbgHdlP,
    	"MZ25_SetRts(0x%x): MCR=0x%x.\n",
	    tmphdlP->address, mask) );

	if( status == TRUE ){
		/* set RTS */
		tmphdlP->rts = TRUE;
		mask |= MIZ25_RTS;
	}
	else{
		/* reset RTS */
		tmphdlP->rts = FALSE;
		mask &= ~MIZ25_RTS;
	}

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_MCR_OFFSET, mask);

	MZ25_EnableInterrupt(hdlP, intStored);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetRts(0x%x): MCR=0x%x.\n",
	    tmphdlP->address, mask) );

	return MZ25_OK;
}/* MZ25_SetRts */

/**********************************************************************/
 /** Routine to enable/disable the automatic RTS/CTS handshake
  *
  *  This routine enables or disables the automatic RTS/CTS handshake.
  *  The feature is only supported by Z125 core designs.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param status			TRUE  = RTS/CTS handshake on
  *                         FALSE = RTS/CTS handshake off
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_EnableAutoRtsCts(HDL_16Z25 *hdlP,
						     BOOL status)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
	u_int8 intStored = 0;       /* active interrupts */
	u_int8 mask = 0;            /* mask */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_EnableAutoRtsCts\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	mask = MZ25_REG_READ(tmphdlP->address, MIZ25_MCR_OFFSET);

    DBGWRT_3( (tmphdlP->dbgHdlP,
    	"MZ25_EnableAutoRtsCts(0x%x): MCR=0x%x.\n",
	    tmphdlP->address, mask) );

	if( status == TRUE ){
		/* set RTS */
		tmphdlP->rtsCtsHs = TRUE;
		mask |= MIZ25_RCFC;
	}
	else{
		/* reset RTS */
		tmphdlP->rtsCtsHs = FALSE;
		mask &= ~MIZ25_RCFC;
	}

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_MCR_OFFSET, mask);

	MZ25_EnableInterrupt(hdlP, intStored);

	mask = 0;
	mask = MZ25_REG_READ(tmphdlP->address, MIZ25_MCR_OFFSET);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_EnableAutoRtsCts(0x%x): MCR=0x%x.\n",
	    tmphdlP->address, mask) );

	if( !(mask&MIZ25_RCFC) && (status == TRUE) ){
	    printf("*** error: Automatic RTS/CTS handshake is not implemented in FPGA uart IP core !\n");

	    DBGWRT_ERR( (tmphdlP->dbgHdlP,
		"MZ25_EnableAutoRtsCts(0x%x): Feature is not supported by device !\n\n",
	    tmphdlP->address) );

	    return MZ25_ERROR;
	}

    return MZ25_OK;
}/* MZ25_EnableAutoRtsCts */
/**********************************************************************/
 /** Routine to set the DTR line
  *
  *  This routine sets the DTR line to logical '1' or logical '0'.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param status			true=DTR high, false=DTR low
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetDtr(HDL_16Z25 *hdlP,
				  BOOL status){
	TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
	u_int8 intStored = 0;       /* active interrupts */
	u_int8 mask = 0;            /* mask */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetDtr\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	mask = MZ25_REG_READ(tmphdlP->address, MIZ25_MCR_OFFSET);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetDtr(0x%x): MCR=0x%x.\n",
	    tmphdlP->address, mask) );

	if( status == TRUE ){
		/* set DTR */
		tmphdlP->dtr = TRUE;
		mask |= MIZ25_DTR;
	}
	else{
		/* reset DTR */
		tmphdlP->dtr = FALSE;
		mask &= ~MIZ25_DTR;
	}

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_MCR_OFFSET, mask);

    MZ25_EnableInterrupt(hdlP, intStored);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetDtr(0x%x): MCR=0x%x.\n",
	    tmphdlP->address, mask) );

	return MZ25_OK;
}/* MZ25_SetDtr */

/**********************************************************************/
 /** Routine to set the OUT1 line
  *
  *  This routine sets the OUT1 line to logical '1' or logical '0'.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param status			true=OUT1 high, false=OUT1 low
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetOut1(HDL_16Z25 *hdlP,
				   BOOL status){
	TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
	u_int8 intStored = 0;       /* active interrupts */
	u_int8 mask = 0;            /* mask */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetOut1\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	mask = MZ25_REG_READ(tmphdlP->address, MIZ25_MCR_OFFSET);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetOut1(0x%x): MCR=0x%x.\n",
		tmphdlP->address, mask) );

	if( status == TRUE ){
		/* set OUT1 */
		tmphdlP->out1 = TRUE;
		mask |= MIZ25_OUT1;
	}
	else{
		/* reset OUT1 */
		tmphdlP->out1 = FALSE;
		mask &= ~MIZ25_OUT1;
	}

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_MCR_OFFSET, mask);

    MZ25_EnableInterrupt(hdlP, intStored);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetOut1(0x%x): MCR=0x%x.\n",
		tmphdlP->address, mask) );

    return MZ25_OK;
}/* MZ25_SetOut1 */

/**********************************************************************/
 /** Routine to set the OUT2 line
  *
  *  This routine sets the OUT2 line to logical '1' or logical '0'.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param status			true=OUT2 high, false=OUT2 low
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetOut2(HDL_16Z25 *hdlP,
				     BOOL status){
	TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
	u_int8 intStored = 0;       /* active interrupts */
	u_int8 mask = 0;            /* mask */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetOut2\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

	mask = MZ25_REG_READ(tmphdlP->address, MIZ25_MCR_OFFSET);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetOut2(0x%x): MCR=0x%x.\n",
		tmphdlP->address, mask) );

	if( status == TRUE ){/* set OUT2 */
		tmphdlP->out2 = TRUE;
		mask |= MIZ25_OUT2;
	}
	else{
		/* reset OUT2 */
		tmphdlP->out2 = FALSE;
		mask &= MIZ25_OUT2;
	}

	MZ25_REG_WRITE(tmphdlP->address, MIZ25_MCR_OFFSET, mask);

    MZ25_EnableInterrupt(hdlP, intStored);

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_SetOut2(0x%x): MCR=0x%x.\n",
		tmphdlP->address, mask) );

    return MZ25_OK;
} /* MZ25_SetOut2 */

/**********************************************************************/
 /** Routine to get the status of the CTS line
  *
  *  This routine returns the status of the CTS line.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return 1=CTS high, 0=CTS low
 */
int32 MZ25_GetCts(HDL_16Z25 *hdlP)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z25 resources */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_GetCts\n"));

	/* get CTS status */
	tmphdlP->cts =
		(MZ25_REG_READ(tmphdlP->address,
		 MIZ25_MSR_OFFSET) & MIZ25_CTS) >> 4;

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_GetCts(0x%x): CTS='%d'.\n",
		tmphdlP->address, tmphdlP->cts) );

	return tmphdlP->cts;
}/* MZ25_GetCts */

/**********************************************************************/
 /** Routine to get the status of the DSR line
  *
  *  This routine returns the status of the DSR line.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return 1=DSR high, 0=DSR low
 */
int32 MZ25_GetDsr(HDL_16Z25 *hdlP)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_GetDsr\n"));

	/* get DSR status */
	tmphdlP->dsr =
		(MZ25_REG_READ(tmphdlP->address,
		 MIZ25_MSR_OFFSET) & MIZ25_DSR) >> 4;

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_GetDsr(0x%x): DSR='%d'.\n",
		tmphdlP->address, tmphdlP->dsr) );

	return tmphdlP->dsr;
}/* MZ25_GetDsr */

/**********************************************************************/
 /** Routine to get the status of the DCD line
  *
  *  This routine returns the status of the DCD line.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return 1=DCD high, 0=DCD low
 */
int32 MZ25_GetDcd(HDL_16Z25 *hdlP)
{
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */

    if( hdlP == NULL ){
         return MZ25_ERROR;;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_GetDcd\n"));

	/* get DCD status */
	tmphdlP->dsr =
		(MZ25_REG_READ(tmphdlP->address,
		 MIZ25_MSR_OFFSET) & MIZ25_DCD) >> 4;

	DBGWRT_3( (tmphdlP->dbgHdlP,
		"MZ25_GetDcd(0x%x): DCD='%d'.\n",
		tmphdlP->address, tmphdlP->dcd) );

	return tmphdlP->dsr;
}/* MZ25_GetDcd */

/**********************************************************************/
 /** Routine to set the UART mode
  *
  *  This routine sets the UART mode. Either RS232 or RS485 (full/half
  *  duplex) is possible. Use the defines MODE_RS232, MODE_RS485_HD or
  *  MODE_RS485_FD to set the mode.
  *
  *	 \param hdlP			16Z025 module handle
  *  \param value   		UART operation mode
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetSerialMode(HDL_16Z25 *hdlP,
						 int32 value){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 intStored = 0;       /* active interrupts */
    u_int8 mask = 0;            /* mask */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetSerialMode\n"));

    mask = value & 0x00ff;

    intStored = MZ25_DisableInterrupt(hdlP, 0);

    MZ25_REG_WRITE(tmphdlP->address, MIZ25_ACR_OFFSET, mask);

    MZ25_EnableInterrupt(hdlP, intStored);

    DBGWRT_2( (tmphdlP->dbgHdlP,
    	"MZ25_SetSerialMode(0x%x): ACR=0x%x written.\n",
	    tmphdlP->address, mask) );

	return MZ25_OK;
}/* MZ25_SetSerialMode */

/**********************************************************************/
 /** Routine to get the UART mode
  *
  *  This routine gets the UART mode. Possible values are MZ25_MODE_RS232,
  *  MZ25_MODE_RS485_HD or MZ25_MODE_RS485_FD.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return value UART operation mode
 */
STATUS MZ25_GetSerialMode(HDL_16Z25 *hdlP){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 intStored = 0;      /* active interrupts */
    u_int8 value = 0;          /* value */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_GetSerialMode\n"));

    intStored = MZ25_DisableInterrupt(hdlP, 0);

    value = MZ25_REG_READ(tmphdlP->address, MIZ25_ACR_OFFSET);

    MZ25_EnableInterrupt(hdlP, intStored);

    DBGWRT_2( (tmphdlP->dbgHdlP,
    	"MZ25_GetSerialMode(0x%x): ACR=0x%x.\n",
	    tmphdlP->address, value) );

	return value;
}/* MZ25_GetSerialMode */

/**********************************************************************/
 /** Routine to set the 16Z025 handle value for line status
  *
  *  This routine sets the value for the 16Z025 handle variable lineStatus.
  *  It is used to return the current line status with the help of
  *  function MZ25GetLineStatus.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetLineStatus(HDL_16Z25 *hdlP){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetLineStatus\n"));

    tmphdlP->lineStatus = MZ25_LINE_STATUS(tmphdlP->address);

    DBGWRT_4( (tmphdlP->dbgHdlP,
    	"MZ25_SetLineStatus(0x%x): LSR=0x%x read.\n",
	    tmphdlP->address, tmphdlP->lineStatus) );

	return MZ25_OK;
}/* MZ25_SetLineStatus */

/**********************************************************************/
 /** Routine to get the line status
  *
  *  This routine routine returns the line status.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return value line status
 */
int32 MZ25_GetLineStatus(HDL_16Z25 *hdlP){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */

    if( hdlP == NULL ){
        return 0;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_GetLineStatus\n"));

	DBGWRT_4( (tmphdlP->dbgHdlP,
	    "MZ25_GetLineStatus(0x%x): LSR=0x%x register value.\n",
	    tmphdlP->address, tmphdlP->lineStatus) );

    return (int32)tmphdlP->lineStatus;
}/* MZ25_GetLineStatus */


/**********************************************************************/
 /** Routine to set UART frequnecy
  *
  *  This routine sets the frequency to calculate the right value for
  *  the baudrate.\n
  *  Formula:\n
  * \verbatim
                    frequency
     divisor = -------------------------
               baudrate * 16 (or: 16 * 2) \endverbatim
  *
  *  dos compatibility mode (default): frequency = 1,8432MHz\n
  *  others                          : frequency = 33MHz/66MHz ...\n
  *
  *	 \param hdlP			16Z025 module handle
  *  \param frequency       UART clock frequency
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetBaseBaud(HDL_16Z25 *hdlP,
					   u_int32 frequency){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    DBGWRT_1((tmphdlP->dbgHdlP,
		"MZ25_SetBaseBaud\n"));

    tmphdlP->uartFreq = frequency;

    if( frequency == 1843200 ){
        tmphdlP->divisorConst = 16;
    }
    else{
        tmphdlP->divisorConst = 32;
    }
    return MZ25_OK;
}/* MZ25_SetBaseBaud */

/**********************************************************************/
 /** Routine to enable Tx interrupt in modem mode
  *
  *  This routine enables the Tx interrupt in dependence of the
  *  modem status register of the UART.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_ControlModemTxInt(HDL_16Z25 *hdlP){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    u_int8 modemStatus = 0;		/* modem register status */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    modemStatus = MZ25_MODEM_STATUS(tmphdlP->address);

	/* if CTS is asserted by modem, enable Tx interrupt */
	if( (modemStatus & MIZ25_CTS) &&
	    (modemStatus & MIZ25_DCTS) ){
	    MZ25_EnableInterrupt(tmphdlP, MZ25_THREIEN);
	}
    else{
	    MZ25_DisableInterrupt(tmphdlP, MZ25_THREIEN);
    }

    return MZ25_OK;
}/* MZ25_ControlModemInt */

/**********************************************************************/
 /** Routine to set the modem mode
  *
  *  This routine enables or disables the modem mode.
  *
  *	 \param hdlP			16Z025 module handle
  *	 \param onOff           TRUE=modem enabled, FALSE=modem disabled
  *
  *  \return MZ25_OK or MZ25_ERROR
 */
STATUS MZ25_SetModemControl(HDL_16Z25 *hdlP, BOOL onOff){
    TS_16Z25_UNIT *tmphdlP;    /* 16Z025 resources */
    STATUS status = MZ25_OK;   /* return value */

    if( hdlP == NULL ){
        return MZ25_ERROR;
    }
    else{
        tmphdlP = hdlP;
    }

    tmphdlP->modemControl = onOff;

    if( onOff == TRUE ){
        status = MZ25_EnableInterrupt(tmphdlP,
        	            (MZ25_MSIEN|MZ25_RLSIEN));
    }
    else{
        status = MZ25_DisableInterrupt(tmphdlP,
        	            (MZ25_MSIEN|MZ25_RLSIEN));
    }

    return status;
}/* MZ25_SetModemControl */

/**********************************************************************/
 /** Routine returns if modem mode is enabled or disabled
  *
  *  This routine returns the modem mode.
  *
  *	 \param hdlP			16Z025 module handle
  *
  *  \return TRUE=modem enabled, FALSE=modem disabled
 */
BOOL MZ25_GetModemControl(HDL_16Z25 *hdlP){
    TS_16Z25_UNIT *tmphdlP;  /* 16Z025 resources */

    if( hdlP == NULL ){
        return FALSE;
    }
    else{
        tmphdlP = hdlP;
    }

    return tmphdlP->modemControl;
} /* MZ25_GetModemControl */
/*! @} */





