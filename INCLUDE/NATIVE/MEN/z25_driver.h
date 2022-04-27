/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  z25_driver.h
 *
 *      \author  sv
 *        $Date: 2010/12/08 23:10:00 $
 *    $Revision: 2.6 $
 *
 *        \brief  13Z025 main header file
 *
 *            	  This header file contains the public interface of the
 *                13Z025 driver.
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: z25_driver.h,v $
 * Revision 2.6  2010/12/08 23:10:00  cs
 * R:1. redundant definition of def values for fifo and buffer size in z25_mdis.h
 *   2. lots of defines from driver internal header file where used globally
 * M:1. define default buffer sizes and fifo trigger levels here
 *   2a) remove include of driver internal header file
 *    b) set necessary defines here
 *
 * Revision 2.5  2007/07/13 09:52:12  SVogel
 * added defines Z25_PCI_IRQ_ENABLED and Z25_PCI_IRQ_DISABLED
 *
 * Revision 2.4  2007/07/06 14:53:03  SVogel
 * + cosmetics
 * + default baudrate set to 115200 bd
 * + added z125 support
 *
 * Revision 2.3  2006/05/03 15:24:37  cs
 * removed public function Z25_EnableInterrupt()
 *     interrupts now enabled internally by driver in all cases
 *
 * Revision 2.2  2005/07/06 09:23:14  SVogel
 * added define Z25_MAX_PCI_BUS_SEARCH
 *
 * Revision 2.1  2005/06/23 08:59:54  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifndef	_Z25_DRIVER_H
#define	_Z25_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <MEN/dbg.h>
#include <MEN/mz25_module.h>

/*--------------------------------------+
|   DEFINES                             |
+---------------------------------------*/
#define Z25_DEFAULT_BAUDRATE           (115200)

#define Z25_RX_BUFF_SIZE               (512)	/**< Rx Buffer Size */
#define Z25_TX_BUFF_SIZE               (512)	/**< Tx Buffer Size */
#define Z25_RX_FIFO_TRIG_LEVEL         (4) 		/**< Rx FIFO Trigger Level */
#define Z25_TX_FIFO_TRIG_LEVEL         (30)		/**< Tx FIFO Trigger Level */


#define Z25_MAX_PCI_DEV				    10
										  /**< maximum number of supported pci devices with Z25/Z125 units */
										  /* default: 10 */

#define Z25_MAX_UNITS        			12
										  /**< maximum number of supported units in system */
										  /* default: 12 */

#define Z25_MAX_UARTS_PER_DEV           4 /**< maximal number of uarts per device */
										  /* typically 4 in Z25 Quad UART */

#define Z25_MAX_PCI_BUS_SEARCH          (256)
#define Z25_PCI_IRQ_ENABLED             (1)           /**< PCI irq enable */
#define Z25_PCI_IRQ_DISABLED            (0)           /**< PCI irq enable */

/*--------------------------------------+
|   TYPEDEFS                            |
+---------------------------------------*/
typedef void Z25_HDL;

/*
 * This enumeration contains the Z25 return values.
 */
typedef enum {/* Z25_RETURN */
	Z25_OK = OK,
	Z25_ERROR = ERROR
} Z25_RETURN;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
extern void Z25_DriverId(void);

extern Z25_HDL * Z25_InitDriver(void);

extern Z25_HDL * Z25_CreateDevice(int8 *devName,
									int8 *inString,
									u_int8  usePciIrq,
							        u_int32 irqBase,
							        u_int16 irqOffset,
							        u_int32 uartFreq,
							        FUNCPTR intConnectAddr,
							        FUNCPTR intEnableAddr);

extern STATUS Z25_SetIntFunctions(Z25_HDL *hdlP,
                                 u_int32 irqBase,
							     FUNCPTR intConnectAddr,
							     FUNCPTR intEnableAddr);

extern STATUS Z25_InitDriverAtBoot(Z25_HDL *hdlP,
							   u_int16 unit,
				               u_int16 channel);

extern STATUS Z25_FindUartUnits(Z25_HDL *hdlP,
								 u_int16 pathIndex,
				                 u_int16 *unitP,
				                 u_int16 *maxUnitP);

extern STATUS Z25_SetBaseBaud(Z25_HDL *hdlP,
		                       u_int32 frequency,
					           u_int16 unit);

extern STATUS Z25_InstallTtyInterface(Z25_HDL * hdlP,
					    			   int8 *devNameP,
								       u_int16 unit,
								       u_int16 noOfChan,
								       u_int16 rxBuffSize,
								       u_int16 txBuffSize);

extern STATUS Z25_GetPciPathInfo(Z25_HDL *hdlP,
		    				    int8 *devicePathP,
							    u_int16 *pathIndexP);

extern STATUS Z25_GetIosDriverNumber(Z25_HDL * hdlP,
                                      u_int16 unit);

extern STATUS Z25_FreeHandle(Z25_HDL **hdlP);

extern void Z25_SetDebugLevel(u_int32 level);
extern u_int32 Z25_GetDebugLevel(void);

extern u_int32 G_Z25_ChamPciDomain;
extern VXB_DEVICE_ID G_Z25_vxbDevID;


#ifdef __cplusplus
}
#endif

#endif	/* _Z25_DRIVER_H */

