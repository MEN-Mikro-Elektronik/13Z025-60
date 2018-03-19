/* 50menZ25.cdf - Component configuration file */

/*
 * Copyright (c) 2005-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
*/

Folder FOLDER_MEN_Z025_CONFIG  {
    NAME			MEN Z025 configuration options
    SYNOPSIS		MEN Z025 configuration options configuration
    CHILDREN		INCLUDE_MEN_Z025
    _CHILDREN		FOLDER_DRIVERS
    _DEFAULTS		+= FOLDER_DRIVERS
}

Component	INCLUDE_MEN_Z025 {
	NAME			MEN Z025 Serial Driver
	SYNOPSIS		MEN Z025 Serial Driver
    CONFIGLETTES	MEN/sysMenZ25_cfg.c
    REQUIRES        INCLUDE_VXBUS \
                    INCLUDE_PLB_BUS
	_CHILDREN		FOLDER_MEN_Z025_CONFIG
 	_INIT_ORDER		usrIosCoreInit
	INIT_RTN		sysZ25Init();
	PROTOTYPE		STATUS sysZ25Init (void);
	INIT_AFTER		INCLUDE_TTY_DEV
	INIT_BEFORE		INCLUDE_SIO
    CFG_PARAMS		MEN_Z025_IRQ_BASE \
					MEN_Z025_INT_CONNECT_ROUTINE \
					MEN_Z025_INT_ENABLE_ROUTINE \
					MEN_Z025_USE_PCI_IRQ \
					MEN_Z025_IRQ_OFFSET
}


Component Z025_DEBUG_DRIVER {
	NAME			Z025 serial driver debug version
	SYNOPSIS		Z025 serial driver debug version
	_CHILDREN		FOLDER_MEN_Z025_CONFIG
	REQUIRES		DBG
}

Parameter MEN_Z025_IRQ_BASE {
	NAME		System specific external interrupt base offset for Z025 driver
	SYNOPSIS	This is the function pointer the driver uses to connect it's ISR
	TYPE		uint
	DEFAULT		(0x20)
}

Parameter MEN_Z025_INT_CONNECT_ROUTINE {
	NAME		System specific interrupt connect routine for Z025 driver
	SYNOPSIS	This is the function pointer the driver uses to connect it's ISR
	TYPE		FUNCPTR
	DEFAULT		intConnect
}

Parameter MEN_Z025_INT_ENABLE_ROUTINE {
	NAME		System specific interrupt enable routine for Z025 driver
	SYNOPSIS	This is the function pointer the driver uses to enable the interrupt in the system
	TYPE		FUNCPTR
	DEFAULT		intEnable
}

Parameter MEN_Z025_USE_PCI_IRQ {
	NAME			Set flag for using PCI or chameleon interrupt number for Z(1)25 unit
	SYNOPSIS    	0=use chameleon interrupt, 1=use PCI interrupt number
	TYPE			uint
	DEFAULT			(1)
}
Parameter MEN_Z025_IRQ_OFFSET {
	NAME			Set interrupt number offset for Z(1)25 unit
	SYNOPSIS    	e.g. on x86 systems, PCI device interrupts are to be connected with an offset of 0x20
	TYPE			uint
	DEFAULT			(0x20)
}
