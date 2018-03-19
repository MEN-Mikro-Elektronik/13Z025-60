/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysMenZ25.c
 *      Project: MEN Z(1)25 VxWorks DRIVER
 *
 *       Author: ch
 *        $Date: 2011/10/04 20:00:54 $
 *    $Revision: 1.1 $
 *
 *  Description: Serial driver integration glue code
 *
 *     Required:  -
 *     Switches:  see config.h
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysMenZ25_cfg.c,v $
 * Revision 1.1  2011/10/04 20:00:54  channoyer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2011 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#include "vxWorks.h"
#include "vxLib.h"
#include "arch/ppc/vxPpcLib.h"
#include "cacheLib.h"
#include "private/vmLibP.h"
#include "drv/pci/pciIntLib.h"

#include "intLib.h"
#include "miiLib.h"
#include "end.h"

#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif /* defined PRJ_BUILD */

#ifdef Z025_DEBUG_DRIVER
    #ifndef DBG
        #define DBG
    #endif
#else
    #ifdef DBG
        #undef DBG
    #endif
#endif

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>

#define CHAMV2_VARIANT Mem
#include <MEN/chameleon.h>

#ifdef 	INCLUDE_MEN_Z025
#include <DRIVERS/NATIVE/Z25_UART/z25_driver.c>
#include <DRIVERS/NATIVE/Z25_UART/mz25_module.c>

/******************************************************************************
 *     Initialize the serial devices, can be included like this in any BSP
 *
 ******************************************************************************/
STATUS sysZ25Init(void)
{
      u_int16 i;
      u_int16 j, k;
      u_int16 unit = 0;
      u_int16 maxUnit = 0;
      Z25_DEV_TS *z25DevP;
      u_int16 loopIndex = 0;
      int8 l_const = -1;
      u_int32 frequency = 0;

      /* Initialize driver resources */
      if( (z25DevP = Z25_InitDriver()) != NULL )
      {
           /* BSP specific offset for chameleon v2 devices */
           z25DevP->usePciIrq = MEN_Z025_USE_PCI_IRQ;
           z25DevP->irqOffset = MEN_Z025_IRQ_OFFSET;

           /* Set interrupt functions */
           Z25_SetIntFunctions(z25DevP, MEN_Z025_IRQ_BASE, MEN_Z025_INT_CONNECT_ROUTINE, MEN_Z025_INT_ENABLE_ROUTINE);
           /* Find all quad UART units in the system */
           for( i=0; i<z25DevP->noPciPaths; i++ )
           {
               if( Z25_FindUartUnits(z25DevP, i, &unit, &maxUnit) != 0 )
               {
                   i = z25DevP->noPciPaths;
                   maxUnit = 0;
               }
           }
           /* Install all quad UART units */
           for( j=unit; j<(unit+maxUnit); j++ )
           {
        /*------------------------------+
        | driver installation section  |
        +------------------------------*/

        if( z25DevP->quadUart[j][0].uartCore == IZ25_MODID_2 ){
            if( k >= Z25_MAX_UARTS_PER_DEV ){
                k = 0;
            }
            loopIndex = IZ25_MIN_UARTS_PER_DEV;
            ++k;
            if( l_const == -1 ){
                l_const = j;
            }
        }
        else {
            k = 1;
            loopIndex = Z25_MAX_UARTS_PER_DEV;
        }

        if( k == 1 ){
            if( Z25_GetIosDriverNumber((Z25_HDL *)z25DevP, j) != Z25_OK ) {
                goto CLEANUP;
            }
        }

        if( (z25DevP->quadUart[j][0].uartCore == IZ25_MODID_2) &&
            (k <= Z25_MAX_UARTS_PER_DEV) ){
                /* four z125 uarts are sharing one interrupt */
            z25DevP->driverNumber[j] =  z25DevP->driverNumber[l_const];
        }


		/* set base baud for all units in IP core */
	    Z25_SetBaseBaud((Z25_HDL *)z25DevP, frequency, j);

        for(i=0; i<loopIndex; i++ ){
            if( Z25_InstallTtyInterface( (Z25_HDL *)z25DevP,
                                        "/tyCo",
                                        j,
                                        i,
                                        Z25_RX_BUFF_SIZE,
                                        Z25_TX_BUFF_SIZE) != Z25_OK ){
                goto CLEANUP;
            }
        }
           }
      }

    CLEANUP:
    return OK;
}

#endif /* INCLUDE_MEN_Z25 */
