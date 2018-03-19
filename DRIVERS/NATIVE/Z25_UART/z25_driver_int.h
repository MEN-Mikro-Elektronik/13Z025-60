/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  z25_driver_int.h
 *
 *      \author  sv
 *        $Date: 2015/10/20 13:38:06 $
 *    $Revision: 1.11 $
 *
 *        \brief  13Z025 internal definitions
 *
 *            	  This header file contains the internal interface of the
 *                13Z025 driver. These definitions should not be exported
 *                to other modules !
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: z25_driver_int.h,v $
 * Revision 1.11  2015/10/20 13:38:06  ts
 * R: previous defines for Z25 max. PCI devices were not unique
 * M: defined IZ25_... instead Z25_... defines for max. UARTs and PCI devs.
 *
 * Revision 1.10  2012/08/09 20:36:13  ts
 * R: Rx worked already when devices were instantiated, not when opened
 * M: introduced useCnt: Rx is enabled at 1st open,disabled at last close
 *
 * Revision 1.9  2012/05/14 19:02:42  channoyer
 * R: Standard VxWorks ioctl not supported
 * M:1. Add define for IZ25_ISIG_MASK and IZ25_OSIG_MASK
 *   2. Add options in struct Z25_TY_CO_DEV_TS to store hardware options
 *
 * Revision 1.8  2010/12/08 23:05:19  cs
 * R: some globally used defines were reduntantly defined here
 * M: remove redundant defines, replace with global ones
 *
 * Revision 1.7  2008/02/26 10:57:04  cs
 * cosmetics
 *
 * Revision 1.6  2007/08/30 14:00:49  cs
 * changed:
 *   - IZ25_MODID_* defined to respective CHAMELEON_16Z* defines
 *
 * Revision 1.5  2007/07/19 09:45:24  cs
 * removed IZ25_VENDOR_ID (replaced by common define in chameleon.h)
 *
 * Revision 1.4  2007/07/13 09:52:09  SVogel
 * + added module id support for 16Z057 uarts
 * + added usePciIrq (compiler switch Z25_USE_CHAMELEON_IRQ is no longer used)
 *
 * Revision 1.3  2007/07/06 14:52:57  SVogel
 * + cosmetics
 * + added z125 support
 *
 * Revision 1.2  2005/07/06 09:08:37  SVogel
 * added new struct for PCI bus scan
 *
 * Revision 1.1  2005/06/23 08:53:08  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef	_Z25_DRIVER_INT_H
#define	_Z25_DRIVER_INT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MEN/chameleon.h"
/*--------------------------------------+
|   DEFINES                             |
+---------------------------------------*/
#define IZ25_MIN_UARTS_PER_DEV    		 (1)     /**< minimum number of uarts per device
                                                      (typically: Z125 uart core) */
#define IZ25_MAX_UARTS_PER_DEV    		 (4)     /**< maximal number of uarts per device
                                                      (typically: Z25 uart core) */

#define IZ25_DEVICE_ID_1           (0x4D45)      /**< general FPGA device id */
#define IZ25_DEVICE_ID_2           (0x0002)      /**< device id: 15EM05_F501 */
#define IZ25_DEVICE_ID_3           (0x0003)      /**< device id: 15EM05_F501 */
#define IZ25_DEVICE_ID_4           (0x0004)      /**< device id: 15EM05_AD65 */
#define IZ25_DEVICE_ID_5           (0x0005)      /**< device id: 15EM05_A500 */
#define IZ25_DEVICE_ID_6           (0x0006)      /**< device id: 15EM03 */
#define IZ25_DEVICE_ID_7           (0x0008)      /**< device id: 15EM07-F502 */
#define IZ25_DEVICE_ID_8           (0x0009)      /**< device id: 02F206-01 */
#define IZ25_DEVICE_ID_9           (0x000b)      /**< device id: 02F206-00 */
#define IZ25_DEVICE_ID_10          (0x000d)      /**< device id: 02F206i01 */
#define IZ25_DEVICE_ID_11          (0x000e)      /**< device id: 15EM07-EC01n */
#define IZ25_DEVICE_ID_12          (0x5104)      /**< device id: 15EM04-NAD75 */
#define IZ25_DEVICE_ID_13          (0x0010)      /**< device id: 15EM07-EC01 */
#define IZ25_DEVICE_ID_14          (0x0013)      /**< device id: 02F206i00 */
#define IZ25_DEVICE_ID_15          (0x0001)      /**< device id: 15EM05_EC01 */
#define IZ25_DEVICE_ID_END         (0xffff)      /**< last device id element */

#define IZ25_MODID_1               (CHAMELEON_16Z025_UART)      /**< z25 module code */
#define IZ25_MODID_2               (CHAMELEON_16Z125_UART)      /**< z125 module code */
#define IZ25_MODID_3               (CHAMELEON_16Z057_UART)      /**< z57 module code */
#define IZ25_MODID_END             (0xffff)      /**< last supported modcode */

#define IZ25_MAX_UNITS              (12)          /**< max. 16z025/16z125 units */
#define IZ25_PATH_LENGTH            (16)          /**< max. path length */
#define IZ25_CHANNEL_OFFSET         (0x10)        /* channel address offset */
#define IZ25_MAX_DEV_NAME           (20)          /* device name length,
                                                   e.g tyZ25_0 */
#define IZ25_DOS_COMPATIBILITY      (1843200)     /* dos compatibility mode
												   frequency */
#define IZ25_MAX_PCI_DEV			(10)          /* maximal length of pci path */

/* input and output signals */

#define IZ25_ISIG_MASK	(SIO_MODEM_CTS|SIO_MODEM_DSR|SIO_MODEM_CD)
#define IZ25_OSIG_MASK	(SIO_MODEM_RTS|SIO_MODEM_DTR)

/*--------------------------------------+
|   TYPEDEFS                            |
+---------------------------------------*/
/** This structure describes PCI bus scan interface.
 */
typedef struct {/* Z25_PCI_SCAN_TS */
    u_int16 vendorId;           /**< vendor ID */
    u_int16 deviceId;           /**< device ID */
    u_int8 busNo;               /**< PCI bus number */
    u_int8 deviceNo;            /**< PCI device number */
    u_int8 funcNo;              /**< PCI function number */
    u_int32 *nextP;             /**< address of next PCI device */
} Z25_PCI_SCAN_TS;


/** This structure describes the vxworks sio interface.
 */
typedef struct { /* Z25_SIO_DEVS_TS */
	SIO_DRV_FUNCS	*pDrvFuncs;	        /**< driver functions */

	                                    /**< callbacks */
	STATUS	(*getTxChar)();	            /**< pointer to a xmitr function */
	STATUS	(*putRcvChar)();            /**< pointer to a recvr function */
	void *	getTxArg;                   /**< pointer to transmitt character */
    void *	putRcvArg;                  /**< pointer to received character */
} Z25_SIO_DEV_TS;

/** This union contains the vxworks sio and ty interface.
 */
typedef union {/* Z25_TTY_UNION */
	Z25_SIO_DEV_TS      sioT;
	TY_DEV			    tyDev;
} Z25_TTY_UNION;

/** This structure describes a single channel of a 16Z025 unit.
 */
typedef struct { /* Z25_TY_CO_DEV_TS */
	Z25_TTY_UNION 	u;      /**< vxworks sio and ty interface */
    BOOL 		created;	/**< true if this device has really been created */
					        /**< Registers are Memory mapped */
    u_int32		addr;		/**< uart channel address */
    u_int32		baseAddr;   /**< uart base address */

	u_int16     irq;        /**< Interrupt Request Level */
    u_int16     irqvec;     /**< Interrupt Request vector */

	u_int8		channel;	/**< channel number on UART */
	u_int8 		unit;		/**< UART number in system */
    u_int32		useCnt;     /**< usage count to track multiple open calls*/

	u_int16     uartCore;   /**< z25/z125 uart core flag described by
                                 IZ25_MODCODE_x defines */

	HDL_16Z25   *unitHdlP;  /**< handle of 16Z25 low level driver (TS_16Z25_UNIT) */
	DBG_HANDLE  *dbgHdlP;   /**< debug handle */
	u_int16     options;    /**< hardware options */

} Z25_TY_CO_DEV_TS;

/** This structure describes the pci bus information of the device.
 */
typedef struct {/* TS_CHAM_PCI */
	VXB_DEVICE_ID vxbDevID;         /**< vxBus device ID (for VxBus device ID of PCIe controller) */
    u_int32 bus;                    /**< PCI bus id ( + domain nr. if merged in) */
    u_int16 dev;                    /**< PCI device id */
    u_int16 fct;                    /**< pci function id */
	u_int8	pathLen;				/**< the path length */
	u_int8	path[IZ25_PATH_LENGTH];	/**< the pci path of the unit */
} TS_CHAM_PCI;

/** This structure describes the available quad uart units per pci device.
 */
typedef struct { /* Z25_PATH_TS */
	TS_CHAM_PCI pci;                /**< pci bus information */
	BOOL		installed;			/**< flag if already installed */
	u_int8      start16Z25Unit;     /**< start unit of pci device */
	u_int8      no16Z25Units;       /**< no of units installed in pci device */
} Z25_PATH_TS;

/** This structure describes the interrupt specific functions.
 */
typedef struct {/* Z25_IRQ_TS */
    u_int32 extIrqBase;              /**< external interrupt base */
#ifdef Z25_USE_VXBPCI_FUNCS
    STATUS (*fIntConnectP)(VXB_DEVICE_ID vxbDevId, VOIDFUNCPTR *vector, VOIDFUNCPTR routine, int parameter );
#else			
    STATUS (*fIntConnectP)(VOIDFUNCPTR *vector, VOIDFUNCPTR routine, int parameter );
#endif   	   
    	   /**< function pointer to interrupt connect routine */
    STATUS (*fIntEnableP)(int irq);	 /**< function pointer to interrupt
                                        enable routine */
} Z25_IRQ_TS;

/** This structure describes a 16Z025 quad uart.
 */
typedef struct {/* Z25_DEV_TS */
    Z25_PATH_TS pathInfo[IZ25_MAX_PCI_DEV];    /**< pci path information */

     Z25_TY_CO_DEV_TS
        quadUart[IZ25_MAX_UNITS][IZ25_MAX_UARTS_PER_DEV];  /**< quad uarts units */

    u_int8  noPciPaths;             /**< number of different pci paths */
    u_int8  no16Z25Dev;             /**< number of detected Z25/Z125 units */
    u_int8  noUarts;                /**< number of detected Z25/Z125 uarts */

    int16 driverNumber[IZ25_MAX_UNITS];       /**< vxworks driver number */
    DBG_HANDLE *dbgHdlP;                     /**< debug handle */

     u_int16 irqOffset;              /**< interrupt offset for chameleon
                                          interrupt use */
    u_int8  usePciIrq;              /**< use PCI irq (1=enabled, 0=disabled)*/
    Z25_IRQ_TS irqFct;               /**< interrupt functions */
} Z25_DEV_TS;

#ifdef __cplusplus
}
#endif

#endif	/* _Z25_DRIVER_INT_H */


