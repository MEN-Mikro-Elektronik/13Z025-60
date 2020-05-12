/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  z25_driver.c
 *
 *      \author  sv
 *        $Date: 2012/08/09 20:33:37 $
 *    $Revision: 1.24 $
 *
 *        \brief  13Z025 serial driver interface
 *
 *                This module provides the exported 13Z025 functions for the
 *                different initialization possibilities.
 *
 *     Switches: -
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 * - Z25_DriverId
 * - Z25_InitDriver
 * - Z25_CreateDevice
 * - Z25_SetIntFunctions
 * - Z25_InitDriverAtBoot
 * - Z25_FindUartUnits
 * - Z25_SetBaseBaud
 * - Z25_InstallTtyInterface
 * - Z25_GetPciPathInfo
 * - Z25_GetIosDriverNumber
 * - Z25_FreeHandle
 * - Z25_SetDebugLevel
 * - Z25_GetDebugLevel
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: z25_driver.c,v $
 * Revision 1.24  2012/08/09 20:33:37  ts
 * R: 1. closing an open file handle failed with -1 (errno: -23, UNAVAIL)
 *    2. Char reception worked already when devices were instantiated, not opened
 * M: 1. implemented LocClose and pass it to iosDrvInstall.
 *    2a. splitted LocOpen to LocCreate and LocOpen, pass both to iosDrvInstall.
 *    2b. implemented useCnt: Rx is enabled at 1st open,disabled at last close
 *
 * Revision 1.23  2012/05/14 19:26:47  channoyer
 * R:1. Standard VxWorks ioctl not supported
 *   2. Not possible to read the UART mode
 * M:1. Add ioctl for SIO_BAUD_SET, FIOBAUDRATE, SIO_BAUD_GET,
 *      SIO_HW_OPTS_SET, SIO_HW_OPTS_GET, SIO_MSTAT_GET,
 *      SIO_MCTRL_BITS_SET, SIO_MCTRL_BITS_CLR,
 *      SIO_MCTRL_OSIG_MASK, SIO_MCTRL_ISIG_MASK
 *   2. Add ioctl MEN_UART_IOCTL_MODE_GET
 *
 * Revision 1.22  2011/12/19 19:42:36  ts
 * R: compile on VxWorks 6.8 showed numerous warnings about sign difference
 * M: added casts where necessary
 *
 * Revision 1.21  2010/12/08 23:12:53  cs
 * R: globally used defines where additionally defined in driver internal header
 * M: replaced some defines from driver internal header with global ones
 *
 * Revision 1.20  2010/11/30 17:55:33  cs
 * R: Z25 only supports 30 bytes fifoTriggerLevel
 * M: set default Tx fifoTriggerLevel to 30 instead of 60
 *    this is cosmetical only because register bit set is identical
 *    (Z125 extended FIFO will have 60 byte)
 *
 * Revision 1.19  2009/05/08 18:24:26  cs
 * R:1. Z25_CreateDevice(): base baud only set for first unit of QUART
 *                          left uninitialized for units 1-3!
 * 2. mz25_module functions did not use same debug level as z25_driver
 *      this is very confusing for debugging
 * M:1. call Z25_SetBaseBaud() for every available UART unit in QUART
 * 2. use global G_Z25_DebugLevel for both modules
 *
 * Revision 1.18  2008/12/22 17:09:22  AWanka
 * R: If the interrupt was not from the z25, the z25 ISR handles the interrupt
 *    nevertheless as a modem status interrupt
 * M: Now the ISR checks the MIZ25_IRQN Bit to decide if the interrupt was from
 *    z25 core or not.
 *
 * Revision 1.17  2008/06/09 17:15:51  SVogel
 * R:1. each transmitted byte generated an interrupt,
 *      if fifo trigger level was set to 1
 * M:1. fifoTrigger splittet into fifoRxTrigger and fifoTxTrigger
 *   2. added additional i/o control to set fifo tx trigger level
 *   3. added additional debug message in transmitt interrupt routine
 *
 * Revision 1.16  2008/05/13 18:48:56  cs
 * R:1. FIFO trigger level was set to 1 Byte during init (FIFOs disabled)
 * M:1. fix call to MZ25_SetFifoTriggerLevel() in LocInitChannels()
 *
 * Revision 1.15  2008/02/26 11:48:09  cs
 * R:1. interrupt identifcation was malicious (too many interrupts detected)
 *   2. Z25_SetBaseBaud() set base clock for all subsequent Z25 units
 *   3. Compiler warning about redefinition of DBG_MYLEVEL
 * M:1. fix identification of interrupt cause in LocInterruptRoutine()
 *   2. only set own base clock in Z25_SetBaseBaud()
 *   3. Undef DBG_MYLEVEL befor defining it
 *
 * Revision 1.14  2007/08/30 13:53:57  cs
 * fixed:
 *   - DBG_MYLEVEL points to G_debugLevel now
 * cosmetics
 *
 * Revision 1.13  2007/07/19 09:52:34  cs
 * added:
 *   + support for MEN PCI vendor ID
 *
 * Revision 1.12  2007/07/13 09:52:06  SVogel
 * + cosmetics
 * + added 16Z057 support
 * + added usePciIrq to struct Z25_DEV_TS and Z25_CreateDevice
 *   (compiler switch Z25_USE_CHAMELEON_IRQ is no longer used)
 *
 * Revision 1.11  2007/07/11 14:43:57  SVogel
 * changed chameleon interface to chameleon V2 - V0/V1 is no longer supported
 *
 * Revision 1.10  2007/07/06 14:52:54  SVogel
 * + cosmetics
 * + added z125 support
 * + documentation update
 *
 * Revision 1.9  2006/10/14 02:38:57  cs
 * removed needless fix from last checkin
 * cosmetics
 *
 * Revision 1.8  2006/09/14 11:53:20  cs
 * added some debugs
 * fixed error handling
 *
 * Revision 1.7  2006/05/03 15:44:21  cs
 * function Z25_EnableInterrupt() now local and renamed to LocEnableInterrupt()
 * fixed: - When characters received during initialization system would hang.
 *          Now Rx interrupt is enabled after intConnect has been called in
 *          LocEnableInterrupt()
 *        - only initialize available channels in Z25_InitDriverAtBoot()
 *
 * Revision 1.6  2006/02/16 09:46:16  cs
 * cosmetics
 *
 * Revision 1.5  2005/08/18 14:57:52  SVogel
 * Added Z25_USE_CHAMELEON_IRQ to use the chameleon table interrupt for the uart.
 *
 * Revision 1.4  2005/07/26 10:39:30  SVogel
 * Added function pointer check in LocSioRecieveInt.
 *
 * Revision 1.3  2005/07/22 10:15:59  CSchuster
 * LocFindChameleonDevices(): fixed indexing problem
 * index ran away when FPGAs without Z025_UART modules found
 *
 * Revision 1.2  2005/07/06 09:08:08  SVogel
 * PCI bus search algorithm changed
 *
 * Revision 1.1  2005/06/23 08:53:07  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
 /*! \mainpage
*  \n
*  The Z25 VxWorks driver is a generic uart driver for the MEN FPGA uart cores
*  16Z025/16Z125 and 16Z057. The driver scans the PCI bus and searches for
*  16Z025/16Z125 and 16Z057 chameleon devices.\n
*
*  The driver can start automatically at system boot or can be started manually
*  after the boot procedure. If initialization at system boot is desired, the
*  driver initialization functions must be called in <em>sysSerialHwInit2
*  (sysSerial.c)</em>.\n
*  \n
*  The 13Z025-60.zip file contains all files and scripts to install the driver. \n
*  \n
*  \section CHAP_00 Hardware Requirements
*  The driver requires the following hardware:\n
*  - x86 or PowerPC architecture, e.g. D3 or EM7\n
*  - F206 with <em>F206-00IC001B2.rbf</em> FPGA filling\n
*  - SA-Adapter, e.g.:\n 08SA01-00: rs232\n
*                     08SA02-00: RS485 half duplex\n
*                     08SA02-01: RS485 full duplex\n
*  \n
*  \section CHAP_01 Software Requirements
*  The software requires:\n
*  -  Tornado2.2 / VxWorks 5.5\n
*  -  Wind River Workbench 2.x / VxWorkts 6.x\n
*  \n
*  \section CHAP_02 Supported Devices
*  The following devices are supportedt by the 13Z025-60 driver:\n
*
*  - 15EM01
*  - 15EM03
*  - 15EM04 (define Z25_USE_CHAMELEON_IRQ in Makefile)
*  - 15EM05
*  - 15EM07
*  - 02F206
*  - 02F206i
*  - 02F206n01
*  - 02D006-00
*  \n
*  \section CHAP_1 1. Feature List
*  This section describes the basic features of the Z25 driver.\n
*  - Z025: Baudrates from 300 upto 115200 baud\n
*    Z125: Baudrates from 110 upto 30000000 baud\n
*  - Stop bits 1 or 2\n
*  - Data bits 5 ... 8\n
*  - Parity even, odd or no\n
*  - Automatic Xon/Xoff\n
*  - Manual hardware handshake\n
*  - Automatic RTS/CTS handshake (Z125 only!)\n
*  - VxWorks TTY conformity\n
*  - Can be integrated into any board support packages (BSP)\n
*  - Can be integrated in MDIS environment\n
*  - Driver channels can be used as serial console at boot\n
*  - Each PCI device can be installed separately via its PCI bus path\n
*  - User-definable interrupt functions\n
*  - Possibility to activate debug output\n
*  \n
*  \section CHAP_2 2. Short Description
*  The 13Z025 driver consists of a high-level and a low-level interface. The
*  low-level interface is provided by the module <em>mz25_modules.c</em>. This
*  module defines all 16Z025/16Z125 and 16Z057 specific functions and controls.
*  It is used by the main module <em>z25_driver.c</em>. \n
*  A third module <em>z25_mdis.c</em> is provided for integrating the native
*  driver into MDIS.\n
*  \n
*
*  Header file <em>Z25_driver.h</em> includes various defines which allow the
*  user to adapt the driver to the system.\n
*  \n
*  - <b>Z25_MAX_PCI_DEV</b>:\n
*  Limit maximum number of supported PCI devices\n
*  <em><b>Default value:</b> 10</em>\n
*  \n
*  - <b>Z25_MAX_UNITS</b>: \n
*  Maximum number of supported UART units per Chameleon device
*  if initialization in BSP it represents the total number of
*  UART units in the system\n
*  <em><b>Default value:</b> 10</em>\n
*  \n
*  - <b>Z25_RX_BUFF_SIZE / Z25_TX_BUFF_SIZE</b>: \n
*  Receive/send buffer size for each UART channel\n
*  <em><b>Default value:</b> 512</em>\n
*  \n
*  \subsection CHAP_21 Supported I/O Controls
*  All supported I/O controls return OK on success or ERROR if the
*  operation failed. The following gives a list of all driver controls.\n
*  <b>13Z025 specific controls:</b>\n
*  - MEN_UART_IOCTL_DATABITS\n
*  Set data bits, <em>value=5..8</em>\n
*  - MEN_UART_IOCTL_PARITY\n
*  Set parity, <em>value=0(no),1(even) or 2(odd)</em>\n
*  - MEN_UART_IOCTL_STOPBITS\n
*  Set stop bits, <em>value=1 or 2</em>\n
*  - MEN_UART_IOCTL_SET_RTS\n
*  Set RTS line, <em>value=0(RTS low) or 1(RTS high)</em>\n
*  - MEN_UART_IOCTL_SET_DTR\n
*  Set DTR line, <em>value=0(DTR low) or 1(DTR high)</em>\n
*  - MEN_UART_IOCTL_SET_OUT1\n
*  Set OUT1 line, <em>value=0(OUT1 low) or 1(OUT1 high)</em>\n
*  - MEN_UART_IOCTL_SET_OUT2\n
*  Set OUT2 line, <em>value=0(OUT2 low) or 1(OUT2 high)</em>\n
*  - MEN_UART_IOCTL_GET_CTS\n
*  Get CTS status, <em>return value is 1 if CTS is set, otherwise 0 if not
*  set</em>\n
*  - MEN_UART_IOCTL_GET_DSR\n
*  Get DSR status, <em>return value is 1 if DSR is set, otherwise 0 if not
*  </em> set\n
*  - MEN_UART_IOCTL_GET_DCD\n
*  Get DCD status, <em>return value is 1 if DCD is set, otherwise 0 if not
*  </em> set\n
*  - MEN_UART_IOCTL_MODE_SELECT\n
*  Set physical mode, <em>value=0(RS232), 1(RS485 half duplex), 2(RS485 full
*  duplex)</em>\n
*  - MEN_UART_IOCTL_AUTO_RS485\n
*  Set automatic RS485 2-wire mode, <em>value=ignored</em>\n
*  - MEN_UART_IOCTL_MODE_GET\n
*  Get physical mode, <em>return value=0(RS232), 1(RS485 half duplex), 2(RS485 full
*  duplex)</em>\n
*  - MEN_UART_IOCTL_MODEM\n
*  Set modem modes, <em>value=1(activate) or 0(deactivate)</em>\n
*  - MEN_UART_IOCTL_SET_FIFO_BYTES\n
*  Set Rx FIFO trigger level of uart.
*  Z025: 1/4/30/58  bytes, <em>value=1/4/30/58</em>\n
*  Z125: 1/8/30/116 bytes, <em>value=1/8/60/116</em>\n
*  - MEN_UART_IOCTL_SET_TX_FIFO_BYTES\n
*  Set Tx FIFO trigger level of uart.
*  Z025: up to 60 bytes, <em>value=1..60</em>\n
*  Z125: up to 116 bytes, <em>value=1..116</em>\n
*  <b>Standard VxWorks controls:</b>\n
*  - SIO_BAUD_SET, <em>value=decimal baud-rate value, e.g. 9600</em>\n
*  - FIOBAUDRATE, <em>value=decimal baud-rate value, e.g. 9600</em>\n
*  - SIO_BAUD_GET, <em>return is decimal baud-rate value, e.g. 9600</em>\n
*  - SIO_HW_OPTS_SET, <em>set the hardware options</em>\n
*  - SIO_HW_OPTS_GET, <em>get the hardware options</em>\n
*  - SIO_MSTAT_GET, <em>returns status of all input and output modem signals</em>\n
*  - SIO_MCTRL_BITS_SET, <em>sets modem signal(s) specified in argument</em>\n
*  - SIO_MCTRL_BITS_CLR, <em>clears modem signal(s) specified in argument</em>\n
*  - SIO_MCTRL_OSIG_MASK, <em>returns mask of all input modem signals</em>\n
*  - SIO_MCTRL_ISIG_MASK, <em>returns mask of all output(writable) modem signals</em>\n
* \n
*  \subsection CHAP_22 Supported System Calls
*  - <em><b>open(name, mode, 0):</b></em> open a device (create a handle to the device)\n
*  e.g.: open("/tyZ25_00/0", 2, 0)\n
*  - <em><b>read(fd, ,data, bytes):</b></em> read data from the device\n
*  e.g.: read( fd , &buffer, size )
*  - <em><b>write(fd, data, bytes):</b></em> write data to the device\n
*  e.g.: write(fd, &buffer, size)\n
*  - <em><b>ioctl(fd, request, value):</b></em> configure the device\n
*  see <em>Supported I/O Controls</em>\n
*  e.g.: ioctl (fd , FIOBAUDRATE, 19200)\n
*  - <em><b>close (fd):</b></em> close the device (delete the handle to the device)\n
*  e.g.: close (fd)\n
*
*  fd: file descriptor returned by <em>open</em> function
*
*  \n
*  \section CHAP_3 3. Driver Make
*  This section describes how the driver can be built.\n
*  \n
*  \subsection CHAP_31 3.1 Related Files
*  This section gives an overview about the driver files.
*  \n
*  \subsubsection CHAP_311  3.1.1 High-Level Files
*    - <em>z25_driver.c</em>\n
*      Z25 driver source file\n
*    - <em>z25_driver.h</em>\n
*      Z25 driver header file\n
*    - <em>z25_driver_int.h</em>\n
*      Z25 driver internal header file
*  \n
*  \subsubsection CHAP_312 3.1.2 Low-Level Files
*    - <em>mz25_module.c</em>\n
*      13Z025 low level source file\n
*    - <em>mz25_module.h</em>\n
*      13Z025 low level header file\n
*    - <em>mz25_module_int.h</em>\n
*      13Z025 low level internal header file
*  \n
*  \subsubsection CHAP_313 3.1.3 MDIS Files
*    - <em>z25_mdis.c</em>\n
*      13Z025 mdis source file\n
*    - <em>z25_mdis.h</em>\n
*      13Z025 mdis header file\n
*  \n
*  \subsubsection CHAP_314 3.1.4 Scripts
*    - <em>driver.mak</em> \n
*      MDIS makefile, links driver functions to <em>mdis_xxx.o</em>\n
*    - <em>driver_sw.mak</em> \n
*      MDIS makefile with byteswap, links driver functions to
*      <em>mdis_xxx.o</em>\n
*    - <em>makef.mak</em> \n
*      Module makefile, generates objects <em>mz25_module.o</em>,
*      <em>z25_driver.o</em> and <em>z25.o</em>\n
*    - <em>mk.bat</em> \n
*      Batch file to make objects with makefile <em>makef.mak</em>\n
*  \n
*  \subsection  CHAP_32 3.2 MDIS Integration
*  To integrate the driver into MDIS refer to document <em>21m000-14.pdf</em>
*  (MDIS4 under VxWorks) chapters A2, A3 and A4.\n
*  \n
*  \subsection  CHAP_33 3.3 Compile
*  To compile the driver it is neccessary to set the environment variables of
*  the host to the Tornado path, similar to compilation of a VxWorks BSP or
*  other drivers.\n
*  If the compile procedure succeeded the objects are placed in the following
*  directory for a PPC603 architecture:\n
*  \verbatim
          /VXWORKS/LIB/MEN/objppc603gnu
                        or:
          /VXWORKS/LIB/MEN/objppc603>gnutest \endverbatim
*
*  \subsubsection CHAP_331 3.3.1 MDIS
*  To integrate the 13Z025 driver into MDIS, perform the following steps:\n
*  - Start the mdiswizard.\n
*  - Create a new MDIS project.\n
*  - Select the desired CPU board.\n
*  - Install <em>13Z025-60.zip</em> driver package.\n
*  - Add an F206 CompactPCI 3U FPGA carrier board.\n
*  - Add the Z25 driver units to FPGA internal slot 0..15.\n
*  - Save and build the projects.\n
*
*  You can find a detailed description of how to use the MDIS Wizard in the
*  MDIS user manual, see Chapter 3.2 MDIS Integration.\n
*  \n
*  \subsubsection CHAP_332 3.3.2 None-MDIS
*  In order to use the Z25 driver on different platforms the <em>mk.bat</em>
*  must be modfied to fit the desired CPU type. Only one CPU type can be
*  activated, the others must remain deactivated. If you wish to compile the
*  driver for debug purposes comment the respective lines in <em>mk.bat</em>.\n
*  \section CHAP_4 4. Driver Usage
*  The driver can be installed in two ways. First it can be installed using
*  <em>Z25_MdisDriver</em> and second via <em>Z25_CreateDevice</em>. The first
*  function initializes the device via the MDIS board and device descriptors,
*  where all necessary information is stored. The second possibility needs a few
*  more parameters as input (see 4.3 VxWorks Shell).\n
*  If debug output is enabled, the debug message can be displayed through <em>
*  DBG_Show()</em>.\n
*  \note For some processor boards it is neccessary to change the UART clock
*        frequency. In this casy you can use the function <em>Z25_SetUartBase
*        </em> to change the frequency. For detailed information about this
*        frequency read the user manual of the processor board.
*  \n
*  \subsection CHAP_41 4.1 PCI Bus Path
*  The PCI device path is an enumeration of PCI device numbers of the PCI
*  bridges which will be used on the way to the desired module.\n
*  <b>Example:</b>\n
*  The system consists of a D3 CPU board with an F206. The D3 is placed in slot 1
*  of the CompactPCI backplane. The first F206 is placed in slot 2 of the
*  CompactPCI backplane. The PCI device path for this F206 would be "0x1E 0x0F",
*  because:\n
*  - 0x1E is the device number of the PCI bridge for the CompactPCI backplane on
*  the D3.\n
*  - 0x0F is the device number of the F206 in slot 2 of the CompactPCI
*    backplane.\n
*  \n
*  \subsection CHAP_42 4.2 MDIS
*  After loading the MDIS object the following function must be called to
*  initialize the driver properly.\n
*  \n
*  OS2M_DrvInstall  \n
*  OSS_SetIrqNum0 0 \n
*  Z25_MdisDriver(&f206_1, &z25_1)\n
*  - f206_1: board descriptor of first F206\n
*  - z25_1: device descriptor of first 16Z025 quad UART unit\n
*  - OSS_SetIrqNum0: 0 for power pc boards and 0x20 for intel boards
*
*  \n
*  \note Only one 16Z025 quad UART unit of the FPGA will be installed !
*  \n
*
*  The device list with your 16Z025/16Z125 and 16Z057 UART channels should appear,
*  if you enter the command <em>devs</em> in the VxWorks shell:\n
*  \verbatim
        -> devs
        drv name
          0 /null
          1 /tyCo/0
          1 /tyCo/1
          1 /tyCo/2
          1 /tyCo/3
          6 host:
         10 /vio
         11 /tgtsvr
         12 /tyZ25_00/0
         12 /tyZ25_00/1
         12 /tyZ25_00/2
         12 /tyZ25_00/3
\endverbatim
*  \n
*  \subsection CHAP_43 4.3 VxWorks Shell
*  To initialize the driver in the VxWorks shell after boot the two object files
*  <em>z25.o</em> must be loaded. After that the driver can be installed using
*  the following function call:\n
*  \n
*  Z25_CreateDevice("/tyZ25_0", "0x1E 0x0F", 1, 0x00, 0x80, 1843200, intConnect, intEnable)\n
*
*  - "/tyZ25_0"   : device name\n
*  - "0x1E 0x0F"  : PCI bus path\n
*  - 0x00         : interrupt vector base (intel => 0x20)\n
*  - 0x80         : chameleon V2 interrupt offset (BSP specific)
*  - 1843200      : uart base freqency, if 0 DOS compatibility mode (1.8432MHz)
*  - intConnect: interrupt connect function, if zero <em>pciIntConnect</em>
*   is used as default\n
*  - intEnable    : interrupt enable function, if zero <em>intEnable</em>
*  is used as default\n
*
*  \n
*  The PCI bus path can also contain a direct PCI location contained in the
*  following syntax:
*          "PCIn:bbb.dd.ff"
*  Where:
*         n = PCI domain. For CPUs with single PCI controller:                     n=0
*
*                         For CPUs with multiple PCI controllers (e.g. A21):       n=<PCI domain>
*                         n must be the PCI(e) controller # on which the chameleon FPGA is located.
*                         Attention: domain count starts with 1, because the value is used as
*                         the PCI controller driver instance passed to vxbInstByNameFind().
*         bbb = PCI bus number,  0..255     [in decimal!]
*         dd = PCI device number, 0..31     [in decimal!]
*         f = PCI function number, for MEN Chameleon FPGAs this is always 0.
*
*  If the PCI bus path string does not start with the 3 letters 'PCI' its assumed that the standard
*  behavior is desired.
*  If it starts with 'PCI' its assumed that the domain number is the 3rd char of the string, bus nr. is 5th char,
*  and so on (The string is expected exactly in the format "PCIn:b.d.f")
*  To find out on which PCI controller the FPGA is located, run either sysChameleonInit and sysChameleonShow,
*  or check the VXB_DEVICE_ID handlers of each instance of the PCI controller driver. Use these as argument for
*  vxbPciConfigTopoShow. Example:
*      -> vxBusShow
*      [...]
*            m85xxPci unit 0 on PLB_Bus @ 0x00497aa8 with busInfo 0x00000000
*            m85xxPci unit 1 on PLB_Bus @ 0x0049bca8 with busInfo 0x004963e8
*            m85xxPci unit 2 on PLB_Bus @ 0x0049bea8 with busInfo 0x00496728
*      [...]
*      Here the VXB_DEVICE_ID handlers are 0x00497aa8 for PCI controller #1,
*                                          0x0049bca8 for PCI controller #2,
*                                          0x0049bea8 for PCI controller #3.
*
*      Now the controllers can be checked using vxbPciHeaderShow:
*      -> vxbPciConfigTopoShow  0x0049bca8
*    [0,0,0] type=PROCESSOR
*            status=0x0010 ( CAP DEVSEL=0 )
*            command=0x0006 ( MEM_ENABLE MASTER_ENABLE )
*            bar0 in 32-bit mem space @ 0xc0000000
*    [1,0,0] type=P2P BRIDGE to [2,0,0]
*            base/limit:
*              mem=   0xa0000000/0xa40fffff
*              preMem=0x0000000010000000/0x00000000000fffff
*              I/O=   0x00001000/0x00000fff
*            status=0x0010 ( CAP DEVSEL=0 )
*            command=0x0007 ( IO_ENABLE MEM_ENABLE MASTER_ENABLE )
*    [2,3,0] type=UNKNOWN (0x80) BRIDGE
*            status=0x0200 ( DEVSEL=1 )
*            command=0x0007 ( IO_ENABLE MEM_ENABLE MASTER_ENABLE )
*            bar0 in 32-bit mem space @ 0xa0000000
*            bar1 in prefetchable 32-bit mem space @ 0xa2000000
*            bar2 in 32-bit mem space @ 0xa4000000
*            bar3 in 32-bit mem space @ 0xa4001000
*    value = 0 = 0x0
*    -> vxbPciHeaderShow 0x0049bca8,2,3,0
*        vendor ID =                   0x1a88
*        device ID =                   0x4d45
*        command register =            0x0007
*        status register =             0x0200
*        revision ID =                 0x05
*        class code =                  0x06
*        sub class code =              0x80
*        programming interface =       0x00
*        cache line =                  0x00
*        latency time =                0x80
*        header type =                 0x00
*        BIST =                        0x00
*        base address 0 =              0xa0000000
*        base address 1 =              0xa2000008
*        base address 2 =              0xa4000000
*        base address 3 =              0xa4001000
*        base address 4 =              0x00000000
*        base address 5 =              0x00000000
*        cardBus CIS pointer =         0x00000000
*        sub system vendor ID =        0x00a3
*        sub system ID =               0x5a14
*        expansion ROM base address =  0x00000000
*        interrupt line =              0x07
*        interrupt pin =               0x01
*        min Grant =                   0x01
*        max Latency =                 0xff
*        value = 0 = 0x0
*        ->
*
*  \n
*  \note All 16Z025 quad UART units of the FPGA will be installed !
*        As default the uarts are configured for use in DOS compatibility mode.
*        If it is required to change the uart frequency, please use the function
*        'Z25_SetBaseBaud'.
*  \n
*
*  The device list with your 16Z025/16Z125 and 16Z057 UART channels should appear,
*  if you enter the command <em>devs</em> in the VxWorks shell:\n
*  \verbatim
        -> devs
        drv name
          0 /null
          1 /tyCo/0
          1 /tyCo/1
          1 /tyCo/2
          1 /tyCo/3
          6 host:
         10 /vio
         11 /tgtsvr
         12 /tyZ25_00/0
         12 /tyZ25_00/1
         12 /tyZ25_00/2
         12 /tyZ25_00/3
         13 /tyZ25_01/0
         13 /tyZ25_01/1
         13 /tyZ25_01/2
         13 /tyZ25_01/3
\endverbatim
*  \n
*  \subsection CHAP_44 4.4 Board Support Package (BSP)
*  To integrate the driver in a BSP the driver objects must be linked to the
*  VxWorks image file. Only the function <em>sysSerialHwInit2</em> must be
*  adapted for the integration.
*  The following code section shows a typical driver initialization at boot
*  time.\n
*\code
*  #include "MEN/z25_driver.h"
*
*  void sysSerialHwInit2 (void)
*  {
*      u_int16 i;
*      u_int16 j, k;
*      u_int16 unit = 0;
*      u_int16 maxUnit = 0;
*      Z25_DEV_TS *z25DevP;
*
*      -- Initialize driver resources
*      if( (z25DevP = Z25_InitDriver()) != NULL )
*      {
*           -- BSP specific offset for chameleon v2 devices
*           z25DevP->usePciIrq = 0; -- if PCI interrupt is connected, set usePciIrq = 1
*           z25DevP->irqOffset = BSP_CHAMELEON_V2_IRQ_OFFSET;
*
*           -- Set interrupt functions
*           Z25_SetIntFunctions(z25DevP, 0, intConnect, intEnable);
*           -- Find all quad UART units in the system
*           for( i=0; i<z25DevP->noPciPaths; i++ )
*           {
*               if( Z25_FindUartUnits(z25DevP, i, &unit, &maxUnit) != 0 )
*               {
*                   i = z25DevP->noPciPaths;
*                   maxUnit = 0;
*               }
*           }
*           -- Install all quad UART units
*           for( j=0; j<z25DevP->no16Z25Dev; j++ )
*           {
*               for( k=0; k<Z25_MAX_UARTS_PER_DEV; k++ )
*               {
*                    Z25_InitDriverAtBoot(z25DevP, j, k);
*
*                   addSioChanEntry( tyCoNum++,
*                     (SIO_CHAN *)&z25DevP->quadUart[j][k].u.sioT.pDrvFuncs );
*               }
*
*               -- set the UART frequency for the 15EM01-00
*               -- please read the user manual for detailed information
*               Z25_SetBaseBaud(z25DevP, 132571429, j);
*           }
*      }
*  }\endcode
*  \n
*
 */

 /*! \page dummy
  \menimages
*/

 static char *G_z25RcsId="$Id: z25_driver.c,v 1.24 2012/08/09 20:33:37 ts Exp $ Exp $ build " __DATE__" "__TIME__ ;
/*--------------------------------------*/
/*    INCLUDES                          */
/*--------------------------------------*/
/* VxWorks standard libraries */
#include "vxWorks.h"            /* always first, CPU type, family , big/litte endian, etc. */
#include "iv.h"                 /* for INUM_TO_IVEC, then config.h is not needed */
#include "vmLib.h"
#include "sysLib.h"
#include "intLib.h"
#include "tickLib.h"
#include "taskLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "usrLib.h"
#include "errnoLib.h"
#include "tyLib.h"

#include <vxBusLib.h>
#include <tickLib.h>
#include <hwif/vxbus/vxBus.h>
/* Standard VxWorks driver libraries */
#include "drv/pci/pciConfigLib.h"   /* for PCI const */
#include "drv/pci/pciIntLib.h"      /* for PCI interrupt */

/* Standard ANSI libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MEN specific libraries */
#include <MEN/men_typs.h>
#include <MEN/chameleon.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>

/* #include "MEN/vxttyioctlh." */
#include <MEN/vxttyioctl.h>

/* Module related header files */
#include <MEN/z25_driver.h>
#include "z25_driver_int.h"

/* error handling for direct PCI dev. specification */
#define CHK_NUMBER(X)  if (((X) < '0') && ((X) > '9')) { \
                            errval = ERROR; \
                            goto errout; \
                         }


/* for debug verbosity in case of DBG build */
#define VERBOSITY_1        0x1
#define VERBOSITY_2        0x3
#define VERBOSITY_3        0x7

#define NR_MAX_DOMAINS    3           /* currently on QorIQ P10/20xx */

#define LZ25_CREATE_TYPE_AT_BOOT       (1)
#define LZ25_CREATE_TYPE_LATE          (2)
#define LZ25_MAX_PCI_DEV_NO            (32)
#define LZ25_MAX_PCI_FUNC_NO           (8)

/* tyDevInit API differs between 6.8 and 6.9 */
#if (_WRS_VXWORKS_MINOR == 9)
# define TY_LOC_CAST   TY_DEVSTART_PTR
#elif (_WRS_VXWORKS_MINOR == 8)
# define TY_LOC_CAST   FUNCPTR
#else
# error "*** z25_driver.c: check API of tyDevInit of your version! (7.0?)"
#endif

/* on x86 the PCI IRQ from config space cannot be used directly, the relating vector must be read from sysInumTbl[irq].
 * Instead of MDIS package defines use the more generic ones from target/h/types/vxCpu.h
 **/
#if ( _VX_CPU_FAMILY==_VX_I80X86 )
IMPORT UINT8 *sysInumTbl;
# define INT_NUM_GET(irq)    (sysInumTbl[(int)irq])
#else
# define INT_NUM_GET(irq)    (irq)
#endif

/*--------------------------------------*/
/*    LOCAL PROTOTYPES                  */
/*--------------------------------------*/
/* Z25 resource handling */
LOCAL STATUS LocInitHandle(Z25_HDL **hdlP);

/* PCI bus related functions */
LOCAL STATUS LocScanPciDevices(Z25_PCI_SCAN_TS **pciDevicesP,
                               u_int16 *noPciDevicesP,
                               u_int8 pciDomains);

LOCAL STATUS LocFindPciBridge(int32 pciDomain,
                              int32 bus,
                              u_int8 *busP,
                              int32 *devP);

LOCAL STATUS LocGetDevicePath(Z25_HDL *hdlP,
                              u_int8 *devicePathP,
                              u_int8 *pathCountP,
                              u_int16 devIndex);

/* Chameleon related functions */
LOCAL STATUS LocFindChameleonDevices(Z25_HDL *hdlP, u_int8 pciDomain);

LOCAL STATUS LocInitUartUnit(Z25_HDL *hdlP,
                             u_int32 address,
                             u_int8 irq,
                             u_int16 uartCore);

LOCAL STATUS LocInitChannels(Z25_HDL *hdlP,
                           u_int16 unit,
                           u_int16 channel,
                           u_int8 createType);

LOCAL int LocCreate(Z25_TY_CO_DEV_TS *chanP,
                  int8 *name,
                  int mode);

LOCAL int LocClose( Z25_TY_CO_DEV_TS *chanP );

LOCAL int LocIoctl(Z25_TY_CO_DEV_TS *chanP,
                   int request,
                   void * arg);

LOCAL void LocStartup(Z25_TY_CO_DEV_TS *chanP);

LOCAL void LocInterrupt(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocInterruptRoutine(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocLineInt(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocSioReceiveInt(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocTyReceiveInt(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocSioTransmitInt(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocTyTransmitInt(Z25_TY_CO_DEV_TS *chanP);
LOCAL void LocModemInt(Z25_TY_CO_DEV_TS *chanP);

LOCAL void LocBuildPciPath(int8 *pathString,
                           int8 *devicePath);

LOCAL int LocCallbackInstall(
                    SIO_CHAN *  sioChanP,
                    int callbackType,
                    STATUS (*callback)(void *, ...),
                    void *callbackArg );

LOCAL int LocOptsSet(Z25_TY_CO_DEV_TS *chanP, u_int16 options);

LOCAL int LocMstatGet (Z25_TY_CO_DEV_TS *);
LOCAL int LocMstatSetClr (Z25_TY_CO_DEV_TS *, UINT bits, BOOL setFlag);

/*--------------------------------------+
|   CONSTANTS                           |
+---------------------------------------*/
/* Supported vendor and device ID */
LOCAL const u_int16 G_deviceIdent[]={
                        IZ25_DEVICE_ID_1 ,
                        IZ25_DEVICE_ID_2 ,
                        IZ25_DEVICE_ID_3 ,
                        IZ25_DEVICE_ID_4 ,
                        IZ25_DEVICE_ID_5 ,
                        IZ25_DEVICE_ID_6 ,
                        IZ25_DEVICE_ID_7 ,
                        IZ25_DEVICE_ID_8 ,
                        IZ25_DEVICE_ID_9 ,
                        IZ25_DEVICE_ID_10,
                        IZ25_DEVICE_ID_11,
                        IZ25_DEVICE_ID_12,
                        IZ25_DEVICE_ID_13,
                        IZ25_DEVICE_ID_14,
                        IZ25_DEVICE_ID_15,
                        IZ25_DEVICE_ID_END
};/* G_deviceIdent */

LOCAL const u_int16 G_modId[] = {
                        IZ25_MODID_1,
                        IZ25_MODID_2,
                        IZ25_MODID_3,
                        IZ25_MODID_END
};/* G_modId */

/* SIO driver functions */
LOCAL const SIO_DRV_FUNCS G_sioDrvFuncs ={
    (void*)LocIoctl,                    /* ioctl            */
    (void*)LocStartup,                  /* txStartup        */
    (void*)LocCallbackInstall           /* callbackInstall  */
}; /* G_sioDrvFuncs */

/*--------------------------------------+
|   GLOBALS                             |
+---------------------------------------*/
#ifdef DBG
u_int32 G_Z25_DebugLevel = (DBG_NORM_INTR | DBG_LEVERR | VERBOSITY_3);
#endif
/* Instance (n-th instance of Chameleon device on 'this' PCI domain), e.g. 1st and 2nd
 * PMC on A21C. If -1 select all found Devices */
int32  G_Z25_ChamFpgaInstance = -1;

IMPORT VXB_DEVICE_ID sysGetPciCtrlID(int instance);

/* PCI domain to seek chameleon device (if driver used on multiple PCI domain CPU) */
VXB_DEVICE_ID G_Z25_vxbDevID;
u_int32 G_Z25_ChamPciDomain;

/*--------------------------------------*/
/*    DEFINES                           */
/*--------------------------------------*/
#ifdef DBG
#undef DBG_MYLEVEL
#define DBG_MYLEVEL         (G_Z25_DebugLevel)
#endif /* DBG */

#ifdef MAC_IO_MAPPED
    #define CHAM_INIT            (CHAM_InitIo)
#else
    #define CHAM_INIT            (CHAM_InitMem)
#endif /* MAC_IO_MAPPED */

/**********************************************************************/
/** Routine to set hardware options
 *
 *  This routine sets up the hardware according to the specified option
 *  argument.  If the hardware cannot support a particular option value, then
 *  it should ignore that portion of the request.
 *
 *  \param chanP            pointer to channel
 *  \param options          new hardware options
 *
 *  \return OK on success, EIO if error
 */
LOCAL int LocOptsSet( Z25_TY_CO_DEV_TS *chanP,
                      u_int16 options ){
    int32 status = 0,
          value  = 0,
          retVal = OK;

    if (chanP == NULL || options & 0xffffff00) {
        retVal = EIO;
        goto CLEANUP;
    }

    /* do nothing if options already set */
    if (chanP->options == options) {
        goto CLEANUP;
    }

    if( (chanP->options & CSIZE) != (options & CSIZE) ) {
        switch (options & CSIZE)
        {
        case CS5:
            value = 5; break;
        case CS6:
            value = 6; break;
        case CS7:
            value = 7; break;
        case CS8:
        default:
            value = 8; break;
        }
        status = MZ25_SetDatabits(chanP->unitHdlP, value);
        if( status != OK  ) {
            retVal = EIO;
            goto CLEANUP;
        }
    }

    if( (chanP->options & STOPB) != (options & STOPB) ) {
        if( (options & STOPB) == STOPB ) {
            value = 2;
        } else {
            value = 1;
        }
        status = MZ25_SetStopbits(chanP->unitHdlP, value);
        if( status != OK  ) {
            retVal = ERROR;
            goto CLEANUP;
        }
    }

    if( (chanP->options & (PARENB|PARODD)) != (options & (PARENB|PARODD)) ) {
        switch (options & (PARENB | PARODD))
        {
        case PARENB|PARODD:
            value = 2; break;
        case PARENB:
            value = 1; break;
        default:
        case 0:
            value = 0; break;
        }
        status = MZ25_SetParity(chanP->unitHdlP, value);
        if( status != OK  ) {
            retVal = ERROR;
            goto CLEANUP;
        }
    }

    if( (chanP->options & CLOCAL) != (options & CLOCAL) ) {
        if (!(options & CLOCAL))
        {
        /* !clocal enables hardware flow control(DTR/DSR) */

            MZ25_SetModemControl(chanP->unitHdlP, TRUE);
            MZ25_SetRts(chanP->unitHdlP, TRUE);
            MZ25_SetDtr(chanP->unitHdlP, TRUE);
        }
        else
        {
        /* disable modem status interrupt */

            MZ25_SetModemControl(chanP->unitHdlP, FALSE);
            MZ25_SetRts(chanP->unitHdlP, FALSE);
            MZ25_SetDtr(chanP->unitHdlP, FALSE);
        }
    }

    if( (chanP->options & CREAD) != (options & CREAD) ) {
        if (options & CREAD) {
            /* enable receive interrupt of channel */
            MZ25_EnableInterrupt( chanP->unitHdlP, MZ25_RDAIEN );
        } else {
            /* disable receive interrupt of channel */
            MZ25_DisableInterrupt( chanP->unitHdlP, MZ25_RDAIEN );
        }
    }

    chanP->options = options;

 CLEANUP:
    return retVal;
}/* LocOptsSet */

/*******************************************************************************
*
* LocMstatGet - read device modem control line status
*
* Read the device modem control lines and map them to the standard
* modem signal bits.
*
* RETURNS:
* Returns the modem control line status bits.
*/

LOCAL int LocMstatGet
    (
    Z25_TY_CO_DEV_TS *chanP
    )
    {
    UINT8 rawMsrStatus,
          rawMcrStatus;
    int result = 0;

    rawMsrStatus = MZ25_REG_READ(chanP->addr, MIZ25_MSR_OFFSET);
    rawMcrStatus = MZ25_REG_READ(chanP->addr, MIZ25_MCR_OFFSET);

    /* Now map device status bits, to standard status bits */

    if (rawMsrStatus & MIZ25_CTS) {
        result |= SIO_MODEM_CTS;
    }

    if (rawMsrStatus & MIZ25_DSR) {
        result |= SIO_MODEM_DSR;
    }

    if (rawMsrStatus & MIZ25_RI) {
        result |= SIO_MODEM_RI;
    }

    if (rawMsrStatus & MIZ25_DCD) {
        result |= SIO_MODEM_CD;
    }


    if (rawMcrStatus & MIZ25_RTS) {
        result |= SIO_MODEM_RTS;
    }

    if (rawMcrStatus & MIZ25_DTR) {
        result |= SIO_MODEM_DTR;
    }

    return result;
    }

/*******************************************************************************
*
* LocMstatSetClear - set/clear modem control lines
*
* This routine allows the user to set or clear individual modem control
* lines.  Of course, only the output lines can actually be changed.
*
* RETURNS:
* OK, or EIO upon detecting a hardware fault.
*/

LOCAL int LocMstatSetClr
    (
    Z25_TY_CO_DEV_TS *chanP,
    UINT bits,      /* bits to change */
    BOOL setFlag    /* TRUE = set, FALSE = clear */
    )
    {

    if (bits & SIO_MODEM_DTR) {
        MZ25_SetDtr(chanP->unitHdlP, setFlag);
    }

    if (bits & SIO_MODEM_RTS) {
        MZ25_SetRts(chanP->unitHdlP, setFlag);
    }

    return OK;
    }

/**********************************************************************/
/** Routine to install the SIO callback handler
 *
 *  This routine provides the function pointers for send and receive
 *  operation via the VxWorks SIO interface.
 *
 *  \param sioChanP         SIO channel resources
 *  \param callbackType     callback type (transmit or receive)
 *  \param callback         callback function pointer
 *  \param callbackArg      callback argument
 *
 *  \return OK on success, ENOSYS if error
 */
LOCAL int LocCallbackInstall(  SIO_CHAN *  sioChanP,
                               int         callbackType,
                               STATUS      (*callback)(void *, ...),
                               void *      callbackArg ){
    Z25_TY_CO_DEV_TS *chanP;
    int32 retVal = OK;

    chanP = (Z25_TY_CO_DEV_TS *)sioChanP;
    switch (callbackType){
        case SIO_CALLBACK_GET_TX_CHAR:
            chanP->u.sioT.getTxChar    = (void*)callback;
            chanP->u.sioT.getTxArg     = callbackArg;
            break;
        case SIO_CALLBACK_PUT_RCV_CHAR:
            chanP->u.sioT.putRcvChar   = (void*)callback;
            chanP->u.sioT.putRcvArg    = callbackArg;
            break;
        default:
            retVal = ENOSYS;
            break;
    }

    return retVal;
}/* LocCallbackInstall */

/**********************************************************************/
/** Routine to find PCI bridges
 *
 *  This routine searches for PCI bridges and their buses.
 *
 *  \param pciDomain    number of PCI domain
 *  \param bus          number of buses to scan
 *  \param busP         found bus number
 *  \param devP         found device number
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS  LocFindPciBridge(int32 pciDomain,
                               int32 bus,
                               u_int8 *busP,
                               int32 *devP)
{
    int32       pciFunc = 0;        /* PCI function index */
    int32       pciBus = 0;         /* PCI bus index */
    int32       pciDevice = 0;      /* PCI device index */
    int32  secondaryBus = 0;   /* secondary bus */
    int32  primaryBus = 0;     /* primary bus */
    int32  header = 0;         /* configuration header type */
    int32  vendor = 0;        /* vendor ID */

    /*------------------------------------------+
    | search for PCI bridges and their buses   |
    +------------------------------------------*/
    for (pciBus = 0; pciBus < bus; pciBus++){
        for (pciDevice = 0; pciDevice < LZ25_MAX_PCI_DEV_NO; pciDevice++){
            for (pciFunc = 0; pciFunc < LZ25_MAX_PCI_FUNC_NO; pciFunc++){
                /* read out vendor id */
                OSS_PciGetConfig(NULL, OSS_MERGE_BUS_DOMAIN(pciBus, pciDomain),
                            pciDevice, pciFunc, OSS_PCI_VENDOR_ID, &vendor);

                /*---------------------------------------------------+
                | only look at vendor ID field for existence check   |
                | this field must exsist for every PCI device        |
                | if 0xFFFF is returned, go to next device           |
                +----------------------------------------------------*/
                if (((vendor & 0x0000ffff) != 0x0000FFFF)){
                    /*------------------------------+
                    | read out header type          |
                    | skip if not PCI_PCI bridge    |
                    +-------------------------------*/
                    OSS_PciGetConfig(NULL, OSS_MERGE_BUS_DOMAIN(pciBus, pciDomain),
                                pciDevice, pciFunc, OSS_PCI_HEADER_TYPE, &header);

                    /* PCI to PCI bridge header found */
                    if ((header & PCI_HEADER_TYPE_MASK) == PCI_HEADER_PCI_PCI){
                        /* read out secondary bus number */
                        OSS_PciGetConfig(NULL, OSS_MERGE_BUS_DOMAIN(pciBus, pciDomain),
                                        pciDevice, pciFunc, OSS_PCI_SECONDARY_BUS, &secondaryBus);

                        /* secondary bus available */
                        if(secondaryBus == bus){
                            OSS_PciGetConfig( NULL,
                                              OSS_MERGE_BUS_DOMAIN( pciBus,
                                                                    pciDomain ),
                                              pciDevice,
                                              pciFunc,
                                              OSS_PCI_SECONDARY_BUS,
                                              &primaryBus );

                            /* bus number of PCI to PCI bridge */
                            *busP = primaryBus;
                            *devP = pciDevice;
                            return Z25_OK;
                        }
                    }

                    if((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC){
                        /* no multi function device, so we can stop this loop */
                        pciFunc = LZ25_MAX_PCI_FUNC_NO;
                        break;
                    }
                }
            }
        }
    }
    return Z25_ERROR;
} /* LocFindPciBridge */

/**********************************************************************/
/** Routine to scan the PCI bus for devices
 *
 *  This routine searches the PCI bus for devices with the given
 *  vendor id at startup.
 *
 *  \param pciDevicesP         PCI bus information
 *  \param noPciDevicesP       number of found PCI devices
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocScanPciDevices(Z25_PCI_SCAN_TS **pciDevicesP,
        u_int16 *noPciDevicesP,
        u_int8 pciDomain)
{
    int32 busNo     = 0; /* bus number index */
    int32 deviceNo  = 0; /* device number index */
    int32 funcNo    = 0; /* function number index */
    int32 device    = 0; /* device ID                  */
    int32 vendor    = 0; /* vendor ID                  */
    u_int16 devCnt  = 0;
    Z25_PCI_SCAN_TS *pciDevicesTmpP = NULL;

    for (busNo=0; busNo < Z25_MAX_PCI_BUS_SEARCH; busNo++)
    {
        for (deviceNo=0; deviceNo < LZ25_MAX_PCI_DEV_NO; deviceNo++)
        {
            OSS_PciGetConfig(NULL, OSS_MERGE_BUS_DOMAIN(busNo, pciDomain), deviceNo, funcNo, OSS_PCI_VENDOR_ID, &vendor);
            OSS_PciGetConfig(NULL, OSS_MERGE_BUS_DOMAIN(busNo, pciDomain), deviceNo, funcNo, OSS_PCI_DEVICE_ID, &device);

                /* check if valid vendor ID */
            if( vendor == CHAMELEON_PCI_VENID_MEN || vendor == CHAMELEON_PCI_VENID_ALTERA )
            {
                if( devCnt == 0 ) {
                        pciDevicesTmpP = malloc(sizeof(Z25_PCI_SCAN_TS));
                    if ( pciDevicesTmpP == NULL ) { /* KlocWork Finding */
                        *pciDevicesP = NULL;
                        return Z25_ERROR;
                    }
                        *pciDevicesP = pciDevicesTmpP;
                } else {
                    pciDevicesTmpP->nextP = malloc(sizeof(Z25_PCI_SCAN_TS));
                    if ( pciDevicesTmpP->nextP == NULL ) { /* KlocWork Finding */
                        pciDevicesTmpP = NULL;
                        return Z25_ERROR;
                    }
                        pciDevicesTmpP = (Z25_PCI_SCAN_TS *)pciDevicesTmpP->nextP;
                    }

                        bzero((char *)pciDevicesTmpP, sizeof(Z25_PCI_SCAN_TS));
                        devCnt ++;
                        pciDevicesTmpP->vendorId = vendor;
                        pciDevicesTmpP->deviceId = device;
                        pciDevicesTmpP->busNo = (u_int8)(busNo & 0x000000ff);
                        pciDevicesTmpP->deviceNo = (u_int8)(deviceNo & 0x000000ff);
                        pciDevicesTmpP->funcNo = (u_int8)(funcNo & 0x000000ff);
                        pciDevicesTmpP->nextP = NULL;
                    }
                }
    }


    if( devCnt == 0 ){
        return Z25_ERROR;
    }
    else{
        *noPciDevicesP = devCnt;
        return Z25_OK;
    }
}/* LocScanPciDevices */

/**********************************************************************/
/** Routine to get device path of the found PCI devices
 *
 *  This routine gets the device path of the PCI devices. The device path
 *  is an enumeration of PCI device numbers of the PCI bridges which will
 *  be used to access the desired FPGA module
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param devicePathP  device path of PCI device
 *  \param pathCountP   path length of PCI device
 *  \param devIndex     index of PCI device
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocGetDevicePath(Z25_HDL *hdlP,
                              u_int8 *devicePathP,
                              u_int8 *pathCountP,
                              u_int16 devIndex){
    int32 inBus = 0;        /* PCI bus to start scan*/
    int32 tmpDev = 0;       /* PCI device */
    u_int8 tmpBus = 0;      /* bus number of PCI bridge */
    u_int8 j;               /* index */
    u_int8 tmpPciPath[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  /* local PCI bus variable */
    Z25_DEV_TS *z25HdlP;  /* Z25 resources */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25HdlP = hdlP;
    }

    inBus = OSS_BUS_NBR(z25HdlP->pathInfo[devIndex].pci.bus);
    /* find all bridges to the way of the desired Chameleon device */
    do{
        if( LocFindPciBridge(G_Z25_ChamPciDomain, inBus, &tmpBus, &tmpDev) != Z25_OK ){
            break;
        }

        ++(*pathCountP);
        tmpPciPath[((*pathCountP-1) & 0xf)] = tmpDev; /* KlocWork Finding: limit index to 15 */

        inBus = tmpBus;
    }while( (tmpBus != 0) | (*pathCountP > 16) );

    j=*pathCountP;

    while( j > 0 ){
        *(devicePathP++) = tmpPciPath[j-1];
        --j;
    }

    /* add device number of Chameleon PCI device to PCI path */
    *(devicePathP++) = z25HdlP->pathInfo[devIndex].pci.dev;
    ++(*pathCountP);

    DBGWRT_2( (z25HdlP->dbgHdlP, "Z25/Z125 - PCI device path for device %d:", (devIndex+1) ) );

    /* show device path */
    DBGWRT_2( (z25HdlP->dbgHdlP, "LocGetDevicePath show device path: devIndex = %d\n", devIndex));
    for(j=0; j<z25HdlP->pathInfo[devIndex].pci.pathLen; j++)
    {
        DBGWRT_2( (z25HdlP->dbgHdlP, " 0x%02x", z25HdlP->pathInfo[devIndex].pci.path[j]) );
    }
    DBGWRT_2( (z25HdlP->dbgHdlP, ".\n") );

    DBGWRT_2( (z25HdlP->dbgHdlP, "<--- LocGetDevicePath\n"));
    return Z25_OK;
 }/* LocGetDevicePath */


/**********************************************************************/
/** Routine to find Chameleon devices
 *
 *  This routine calls LocFindPciDevice and LocGetDevicePath to find
 *  all Chameleon devices and to register their PCI path. Each supported
 *  vendor and device ID will be used to find valid devices.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param pciDomain    optional: PCI ctrl. nr if FPGA resides on
 *                                multiple PCIe ctrl CPU. 0 by default
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocFindChameleonDevices(Z25_HDL *hdlP, u_int8 pciDomain){
    int32  index = 0;           /* index */
    Z25_DEV_TS *z25HdlP;        /* Z25 resources */
    u_int32 i;                  /* for loop index */
    u_int16 k = 0;              /* while loop index */
    Z25_PCI_SCAN_TS *pciDevicesP = NULL;
    u_int32 *addrFreeP = NULL;
    u_int16 noPciDevices = 0;

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25HdlP = hdlP;
    }

    if (pciDomain > NR_MAX_DOMAINS)
        return Z25_ERROR;

    DBGWRT_1( (z25HdlP->dbgHdlP, "Z25/Z125 - LocFindChameleonDevices.\n" ) );
    if( LocScanPciDevices(&pciDevicesP, &noPciDevices, pciDomain) == Z25_OK ) {

        DBGWRT_2( (z25HdlP->dbgHdlP,
            "Z25/Z125 - found %d considerable PCI devices.\n",noPciDevices ) );

        for( i=0; i<noPciDevices; i++ ){
            while( G_deviceIdent[k] != IZ25_DEVICE_ID_END ){
                if( pciDevicesP->deviceId == G_deviceIdent[k] ){

                    z25HdlP->pathInfo[index].pci.bus = pciDevicesP->busNo;
                    z25HdlP->pathInfo[index].pci.dev = pciDevicesP->deviceNo;
                    z25HdlP->pathInfo[index].pci.fct = pciDevicesP->funcNo;

                    ++index;

                    DBGWRT_2( (z25HdlP->dbgHdlP,
                        "Z25/Z125 - PCI device %d with vendor-id=0x%x and "
                        "device-id=0x%x found at %d/%d/%d (bus/dev/func).\n",
                        index, pciDevicesP->vendorId, G_deviceIdent[k],
                        pciDevicesP->busNo,
                        pciDevicesP->deviceNo,
                        pciDevicesP->funcNo ) );
                }
                k++;
            }
            /* store current address of item */
            addrFreeP = (u_int32 *)pciDevicesP;
            pciDevicesP = (Z25_PCI_SCAN_TS *)pciDevicesP->nextP;
            /* free list item */
            free(addrFreeP);
            k = 0;
        }
    } else {
        DBGWRT_ERR( (z25HdlP->dbgHdlP, "*** Z25/Z125 - No considerable PCI devices found\n") );
        /* KlocWork finding: free already malloc'ed elements, like above
         * (This scenario can occur if e.g. some list handles could be allocated,
         * then during search for Z25 devices we run out of memory).
         * In that case, LocScanPciDevices returned a nr. of PCI devices but not all.
         * We iterate trough the list from start element to that number (or as long as there
         * are ->nextP elements) and free them. */
        for( i=0; (i < noPciDevices) && (pciDevicesP != NULL); i++ ) {
            addrFreeP = (u_int32 *)pciDevicesP;
            pciDevicesP = (Z25_PCI_SCAN_TS *)pciDevicesP->nextP;
            /* free list item */
            free(addrFreeP);
            addrFreeP = NULL;
        }
        return Z25_ERROR;
    }

    z25HdlP->noPciPaths = index;
    /* KlocWork finding: assure that index isn't larger than IZ25_MAX_PCI_DEV */
    if ( index >= IZ25_MAX_PCI_DEV ) {
            DBGWRT_ERR( (z25HdlP->dbgHdlP, "*** Z25/Z125 - internal error (too many devices ?)\n") );
            return Z25_ERROR;
    }

    /* store found devices in Z25 handle */
    for( i=0; i<index; i++){
        if( LocGetDevicePath(hdlP, z25HdlP->pathInfo[i].pci.path,
                &z25HdlP->pathInfo[i].pci.pathLen, i) != Z25_OK ){
            DBGWRT_ERR( (z25HdlP->dbgHdlP,
                "*** Z25/Z125 - No device path found for device %d.\n", i) );
            return Z25_ERROR;
        }
    }

    return Z25_OK;
} /* LocFindChameleonDevices*/

/**********************************************************************/
/** Routine to initialize the found UART units
 *
 *  This routine performs the basic UART initializations. It stores the
 *  address and enumerates the channels automatically.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param address      unit address
 *  \param irq          unit interrupt number
 *  \param uartCore     uart core identifier
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocInitUartUnit(Z25_HDL *hdlP,
                             u_int32 address,
                             u_int8 irq,
                             u_int16 uartCore){
    u_int8 i;               /* loop index */
    u_int8 uartsExist = 0;  /* existing UART channels */
    u_int16 loopIndex = 0;
    Z25_DEV_TS *z25HdlP;  /* Z25 resources */
    Z25_TY_CO_DEV_TS *tmpQuadUartP;    /* UART resources */
    HDL_16Z25 *tmpHdlP;    /* 16Z025 handle */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else {
        z25HdlP = hdlP;
    }

    /* Z125 uart detected */
    if( uartCore == IZ25_MODID_2 ){
        loopIndex = IZ25_MIN_UARTS_PER_DEV;
         DBGWRT_1((z25HdlP->dbgHdlP,
        "Z25/Z125 - module id 0x%x / Z125 uart core detected !\n",
            uartCore));
    }
    else {
        loopIndex = Z25_MAX_UARTS_PER_DEV;
        DBGWRT_1((z25HdlP->dbgHdlP,
        "Z25/Z125 - module id 0x%x / Z25 uart core detected !\n",
            uartCore));

        uartsExist = MZ25_UARTS_EXISTING((u_int32)(address));

        if( uartsExist == 0 ){
            DBGWRT_ERR((z25HdlP->dbgHdlP,
            "*** Z25/Z125 - No UART channel detected ! \n"));
            return Z25_ERROR;
        }
    }/* end if */

    if( loopIndex == 0 ){
        DBGWRT_ERR((z25HdlP->dbgHdlP,
        "*** Z25/Z125 - No UART channel detected ! \n"));
        return Z25_ERROR;
    }/* end if */

    for( i=0; i<loopIndex; i++ ){
        u_int8 uartMask = 0;

        /* set pointer to UART channel */
        tmpQuadUartP =
            &z25HdlP->quadUart[z25HdlP->no16Z25Dev][i];

        tmpQuadUartP->baseAddr = address;

        uartMask = 1 << i;

        /* check if UART exists */
        if( ((uartsExist&uartMask) != uartMask) &&
            (uartCore != IZ25_MODID_2) ){
            tmpQuadUartP->addr = 0;
        }
        else {/* fill channel structure */
            if( uartCore == IZ25_MODID_2 ){
                tmpQuadUartP->addr = (u_int32)address;
            }
            else {
                tmpQuadUartP->addr = (u_int32)(address + i*IZ25_CHANNEL_OFFSET);
            } /* end if */
            tmpQuadUartP->channel = z25HdlP->noUarts++;
            tmpQuadUartP->dbgHdlP = z25HdlP->dbgHdlP;

            tmpHdlP = NULL;
            /* yes, create MZ25 handle */
            if( MZ25_InitHandle
                ((HDL_16Z25 **)&tmpHdlP,
                tmpQuadUartP->addr ) == 0 ){   /* store 16Z25 handle to channel structure */
                z25HdlP->quadUart[z25HdlP->no16Z25Dev][i].unitHdlP =
                    tmpHdlP;
            }
            else {
                DBGWRT_ERR((z25HdlP->dbgHdlP,
                "*** Z25/Z125 - Could not allocate memory for 16Z25 handle\n"));
                return Z25_ERROR;
            }
        }
        /* register interrupt */
        tmpQuadUartP->irq = irq;
        tmpQuadUartP->irqvec = irq + z25HdlP->irqFct.extIrqBase;
        tmpQuadUartP->uartCore = uartCore;
    }

    /* increment unit number */
    ++z25HdlP->no16Z25Dev;

    return Z25_OK;
}/* LocInitUartUnit */

/**********************************************************************/
/** Routine to Create a channel
 *
 *  \param chanP            channel structure
 *  \param name             device name (used by tyLib)
 *  \param mode             device mode (used by tyLib)
 *
 *  \return address to channel structure
 */
LOCAL int LocCreate( Z25_TY_CO_DEV_TS *chanP,
                   int8 *name,
                   int mode )
{
    return (int)chanP;
}/* LocCreate */

/**********************************************************************/
/** Routine to open a channel
 *
 *  This routine is used by the tyLib to open a UART channel using the
 *  VxWorks function 'open(...)'.
 *
 *  \param chanP            channel structure
 *  \param name             device name (used by tyLib)
 *  \param mode             device mode (used by tyLib)
 *
 *  \return address to channel structure
 */
LOCAL int LocOpen( Z25_TY_CO_DEV_TS *chanP )
{
    if( chanP != NULL )
    {
        chanP->useCnt++;
        if( chanP->useCnt == 1 )
        {
            /* first open: Set UART to RS232 mode as default (enables the Receiver) */
            MZ25_SetSerialMode( chanP->unitHdlP,
                                MZ25_MODE_RS232 );
        }
        return (int)chanP;
    }
    else return ( Z25_ERROR );
}/* LocCreate */


/**********************************************************************/
/** Routine to close a channel
 *
 * This routine is used by the tyLib to close a UART channel using the
 * VxWorks function 'close()'.
 *
 *  \param fh     file handle
 *
 *  \return OK
 */
LOCAL int LocClose( Z25_TY_CO_DEV_TS *chanP )
{
    switch( chanP->useCnt )
    {
        case 0:
            return ( Z25_ERROR );
            break;
        case 1: /* close last handle? -> disable Rx */
            MZ25_REG_WRITE( chanP->addr,
                            MIZ25_ACR_OFFSET,
                            MZ25_REG_READ( chanP->addr,
                                           MIZ25_ACR_OFFSET ) &= ~MIZ25_RXEN );
            chanP->useCnt--;
            return ( OK );
        default:
            chanP->useCnt--;
            return ( OK );
    }
}/* LocClose */


/**********************************************************************/
/** Routine to handle device control requests
 *
 *  This routine provides the 16Z025/16Z125 and 16Z057 specific control
 *  request, to control the UART channels via the VxWorks function 'ioctl(...)'.
 *
 *  \param chanP            channel structure
 *  \param request          control request
 *  \param arg              control request argument
 *
 *  \return OK or ENOSYS
 */
LOCAL int LocIoctl(Z25_TY_CO_DEV_TS *chanP,
                   int request,
                   void * pArg ){
    int status = OK;    /* tty status */
    int arg = (int)pArg;

    DBGWRT_3((chanP->dbgHdlP," Z25/Z125 - LocIoctl request=%d, arg=0x%08x\n",request, arg));
    switch (request){
        case SIO_BAUD_SET:
        case FIOBAUDRATE:
            if( (chanP->uartCore == IZ25_MODID_1) &&
                (arg > 115200) ){
                arg = 115200;
                printf("*** Unsupported baudrate for Z25 uart core - baudrate limited to %d baud\n", arg);
            }/* end if */

            status = MZ25_SetBaudrate(chanP->unitHdlP, arg);
            break;

        case SIO_BAUD_GET:
            *(int *)pArg = ((TS_16Z25_UNIT *)chanP->unitHdlP)->baudrate;
            break;

        case SIO_HW_OPTS_SET:
            status = LocOptsSet(chanP, (u_int16)arg);
            break;

        case SIO_HW_OPTS_GET:
            *(int *)arg = chanP->options;
            break;

        case SIO_MSTAT_GET:
            return LocMstatGet(chanP);

        case SIO_MCTRL_BITS_SET:
            return LocMstatSetClr (chanP, arg, TRUE);

        case SIO_MCTRL_BITS_CLR:
            return LocMstatSetClr (chanP, arg, FALSE);

        case SIO_MCTRL_OSIG_MASK:
            *(int *)arg = IZ25_OSIG_MASK;
            break;

        case SIO_MCTRL_ISIG_MASK:
            *(int *)arg = IZ25_ISIG_MASK;
            break;

        case MEN_UART_IOCTL_DATABITS:
            status = MZ25_SetDatabits(chanP->unitHdlP, arg);
            if(status == OK) {
                chanP->options &= ~CSIZE;
                switch(arg){
                    case 5:
                        chanP->options |= CS5;
                        break;
                    case 6:
                        chanP->options |= CS6;
                        break;
                    case 7:
                        chanP->options |= CS7;
                        break;
                    case 8:
                    default:
                        chanP->options |= CS8;
                        break;
                }
            }
            break;

        case MEN_UART_IOCTL_PARITY:
            status = MZ25_SetParity(chanP->unitHdlP, arg);
            if(status == OK) {
                chanP->options &= ~(PARENB | PARODD);
                switch (arg){
                    case 2:
                        chanP->options |= (PARENB|PARODD);
                        break;
                    case 1:
                        chanP->options |= PARENB;
                        break;
                    default:
                    case 0:
                        break;
                }
            }
            break;

        case MEN_UART_IOCTL_STOPBITS:
            status = MZ25_SetStopbits(chanP->unitHdlP, arg);
            if(status == OK) {
                switch(arg){
                    case 1:
                    default:
                        chanP->options &= ~STOPB;
                        break;
                    case 2:
                        chanP->options |= STOPB;
                        break;
                }
            }
            break;

        case MEN_UART_IOCTL_SET_RTS:
            if( arg == 1){
                status = MZ25_SetRts(chanP->unitHdlP, TRUE);
            }
            else{
                status = MZ25_SetRts(chanP->unitHdlP, FALSE);
            }
            break;

        case MEN_UART_IOCTL_SET_DTR:
            if( arg == 1){
                status = MZ25_SetDtr(chanP->unitHdlP, TRUE);
            }
            else{
                status = MZ25_SetDtr(chanP->unitHdlP, FALSE);
            }
            break;

        case MEN_UART_IOCTL_SET_OUT1:
            if( arg == 1){
                status = MZ25_SetOut1(chanP->unitHdlP, TRUE);
            }
            else{
                status = MZ25_SetOut1(chanP->unitHdlP, FALSE);
            }
            break;

        case MEN_UART_IOCTL_SET_OUT2:
            if( arg == 1){
                status = MZ25_SetOut2(chanP->unitHdlP, TRUE);
            }
            else{
                status = MZ25_SetOut2(chanP->unitHdlP, FALSE);
            }
            break;

        case MEN_UART_IOCTL_GET_CTS:
            status = MZ25_GetCts(chanP->unitHdlP);
            break;

        case MEN_UART_IOCTL_GET_DSR:
            status = MZ25_GetDsr(chanP->unitHdlP);
            break;

        case MEN_UART_IOCTL_GET_DCD:
            status = MZ25_GetDcd(chanP->unitHdlP);
            break;

        case MEN_UART_IOCTL_MODE_SELECT:
            switch(arg){
                case 0: /* RS232 */
                default:
                    status = MZ25_SetSerialMode(chanP->unitHdlP, MZ25_MODE_RS232);
                    break;
                case 1: /* RS485 - half duplex */
                    status = MZ25_SetSerialMode(chanP->unitHdlP, MZ25_MODE_RS485_HD);
                    break;
                case 2: /* RS485 - full duplex */
                    status = MZ25_SetSerialMode(chanP->unitHdlP, MZ25_MODE_RS485_FD);
                    break;
            }
            break;

        case MEN_UART_IOCTL_AUTO_RS485:
            status = MZ25_SetSerialMode(chanP->unitHdlP, MZ25_MODE_RS485_HD);
            break;

        case MEN_UART_IOCTL_MODE_GET:
            switch(MZ25_GetSerialMode(chanP->unitHdlP)){
                case MZ25_MODE_RS232: /* RS232 */
                default:
                    status = 0;
                    break;
                case MZ25_MODE_RS485_HD: /* RS485 - half duplex */
                    status = 1;
                    break;
                case MZ25_MODE_RS485_FD: /* RS485 - full duplex */
                    status = 2;
                    break;
            }
            break;

        case MEN_UART_IOCTL_MODEM:
            if( arg == 1 ){
                status = MZ25_SetModemControl(chanP->unitHdlP, TRUE);
            }
            else{
                status = MZ25_SetModemControl(chanP->unitHdlP, FALSE);
            }
            break;

        case MEN_UART_IOCTL_SET_FIFO_BYTES:
            status = MZ25_SetFifoTriggerLevel(chanP->unitHdlP, 0, arg);
            break;

        case MEN_UART_IOCTL_SET_TX_FIFO_BYTES:
            status = MZ25_SetFifoTriggerLevel(chanP->unitHdlP, 1, arg);
            break;

        case MEN_UART_IOCTL_LINE_STATUS:
            status = MZ25_GetLineStatus(chanP->unitHdlP);
            break;

        case MEN_UART_IOCTL_RTS_CTS:
            if( arg == 1 ){
                status = MZ25_EnableAutoRtsCts(chanP->unitHdlP, TRUE);
            }
            else {
                status = MZ25_EnableAutoRtsCts(chanP->unitHdlP, FALSE);
            }
            break;

        case MEN_UART_IOCTL_HANDSHAKE_OFF:
            status = MZ25_EnableAutoRtsCts(chanP->unitHdlP, FALSE);
            break;

        default:
            /* standard io control */
            if( chanP->created == LZ25_CREATE_TYPE_LATE ){
                status = tyIoctl (&chanP->u.tyDev, request, arg);
            }
            else{
                status = ENOSYS;
            }
            break;
    }/*switch*/

    return (status);
}/* LocIoctl */

/**********************************************************************/
/** Routine to enable the Tx interrupt
 *
 *  This routine is used by the VxWorks tyLib to enable the Tx
 *  interrupt. It is called once for each send cycle. If transmission
 *  finished the transmit interrupt must be disabled by the transmit
 *  interrupt routine.
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return not return value
 */
LOCAL void LocStartup( Z25_TY_CO_DEV_TS *chanP ){
    /* CTS initial state check */
    if( MZ25_GetModemControl(chanP->unitHdlP) == TRUE ){
        MZ25_ControlModemTxInt(chanP->unitHdlP);
    }
    else{
        /* enable Tx interrupt */
        MZ25_EnableInterrupt(chanP->unitHdlP, MZ25_THREIEN);
    }
}/* LocStartup */

/**********************************************************************/
/** Routine to handle the line status interrupt
 *
 *  This routine handles the line status interrupt and calls the  Z25
 *  low layer function.
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocLineInt(Z25_TY_CO_DEV_TS *chanP){
    IDBGWRT_4((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
        ">>> Z25/Z125 - LocLineInt\n"));
    MZ25_SetLineStatus(chanP->unitHdlP);
}/* LocLineInt */

/**********************************************************************/
/** Routine to handle receive interrupt
 *
 *  This routine handles the TTY receive interrupt.
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocTyReceiveInt(Z25_TY_CO_DEV_TS *chanP){
    u_int8 inByte = 0;  /* received byte */


    IDBGWRT_4((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
        ">>> Z25/Z125 - LocTyReceiveInt\n"));

    while(MZ25_RECEIVE_STATUS(chanP->addr))
    {
        inByte = MZ25_READ_BYTE(chanP->addr);

        tyIRd (&chanP->u.tyDev, (char)inByte );
    }
}/* LocTyReceiveInt */

/**********************************************************************/
/** Routine to handle the receive interrupt
 *
 *  This routine handles the SIO receive interrupt.
 *
 *  \param chanP          channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocSioReceiveInt(Z25_TY_CO_DEV_TS *chanP){
    u_int8 inByte = 0;  /* received byte */

    IDBGWRT_4((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
        ">>> Z25/Z125 - LocSioReceiveInt\n"));

    /* don't know how to handle characters?
     * throw them away! */
    if( chanP->u.sioT.putRcvChar == NULL ){
        while(MZ25_RECEIVE_STATUS(chanP->addr)){
            MZ25_READ_BYTE(chanP->addr);
        }
        return;
    }

    while(MZ25_RECEIVE_STATUS(chanP->addr)){
        inByte = MZ25_READ_BYTE(chanP->addr);

        (*chanP->u.sioT.putRcvChar)
            ( chanP->u.sioT.putRcvArg, inByte );
    }
}/* LocSioReceiveInt */

/**********************************************************************/
/** Routine to handle transmit interrupt for TTY interface
 *
 *  This routine is used to handle the VxWorks TTY transmit interrupt.
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocTyTransmitInt(Z25_TY_CO_DEV_TS *chanP){
    char  outByte = 0;     /* byte to transmit */
    u_int8 fifoBytes = 0;   /* FIFO size */

    fifoBytes =
        ((TS_16Z25_UNIT *)chanP->unitHdlP)->fifoTxTrigger;

    IDBGWRT_4((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
        ">>> Z25/Z125 - LocTyTransmitInt: fifoBytes = %d\n", fifoBytes));

    while( fifoBytes-- ){
        if( tyITx(&chanP->u.tyDev, &outByte) == OK ){
            /* fill output FIFO */
            MZ25_WRITE_BYTE(chanP->addr, outByte);
        }
        else{
            /* disable Tx interrupt - transmission finished */
            MZ25_DisableInterrupt(chanP->unitHdlP, MZ25_THREIEN);
            return;
        }
    }/*while*/
}/* LocTyTransmitInt */

/**********************************************************************/
/** Routine to handle the transmit interrupt for SIO interface
 *
 *  This routine is used to handle the VxWorks SIO transmit interrupt.
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocSioTransmitInt(Z25_TY_CO_DEV_TS *chanP){
    u_int8 outByte = 0;     /* byte to transmit */
    u_int8 fifoBytes = 0;   /* FIFO size */

    if( chanP->u.sioT.getTxChar == NULL ){
        return;
    }

    fifoBytes =
    ((TS_16Z25_UNIT *)chanP->unitHdlP)->fifoTxTrigger;

    IDBGWRT_4((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
    ">>> Z25/Z125 - LocSioTransmitInt: fifoBytes = %d\n", fifoBytes));

    while (fifoBytes--){
        if( (*chanP->u.sioT.getTxChar)
               (chanP->u.sioT.getTxArg, &outByte) == OK ){
            /* transmit byte */
            MZ25_WRITE_BYTE(chanP->addr, outByte);
        }
        else{
            /* disable Tx interrupt - transmission finished */
            MZ25_DisableInterrupt(chanP->unitHdlP, MZ25_THREIEN);
            return;
        }
    }/*while*/
}/* LocSioTransmitInt */

/**********************************************************************/
/** Routine to handle the modem status interrupt
 *
 *  This routine handles the modem interrupt. If CTS or DCTS is active,
 *  then the Tx interrupt will be enabled.
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocModemInt(Z25_TY_CO_DEV_TS *chanP){
    MZ25_ControlModemTxInt(chanP->unitHdlP);
}/* LocModemInt */

/**********************************************************************/
/** Routine to handle the 16Z025/16Z125 and 16Z057 interrupts
 *
 *  This routine is the low-level interrupt routine. It is used to serve
 *  the interrupts for the different sources, e.g. receive IRQ,
 *  modem IRQ ... .
 *
 *  \param chanP            channel that caused the interrupt
 *
 *  \return no return value
 */
LOCAL void LocInterruptRoutine(Z25_TY_CO_DEV_TS *chanP){
    u_int8 interruptIdent = 0;      /* interrupt ID */

    interruptIdent = MZ25_IRQ_IDENT(chanP->addr);

    if( interruptIdent & MIZ25_IRQN ) {
        IDBGWRT_3((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
            ">>> Z25/Z125 - LocInterruptRoutine IRQ not mine\n"));
        return;
    }

    interruptIdent &= ~MIZ25_IRQN; /* we know now that is ours */

    IDBGWRT_4((((TS_16Z25_UNIT *)chanP->unitHdlP)->dbgHdlP,
        ">>> Z25/Z125 - LocInterruptRoutine irqId=0x%08x\n", interruptIdent));

    if( (interruptIdent == MZ25_RCV_LINE_STATUS) ){
        LocLineInt(chanP);
    }

    if( (interruptIdent == MZ25_DATA_RECEIVED) ||
        (interruptIdent == MZ25_CHAR_TIMEOUT) )
    {

        if( chanP->created == LZ25_CREATE_TYPE_LATE ){
            LocTyReceiveInt(chanP);
        }
        else if( chanP->created == LZ25_CREATE_TYPE_AT_BOOT ){
            LocSioReceiveInt(chanP);
        }
    }

    /* transmitter holding register empty */
    if( (interruptIdent == MZ25_THR_REG_EMPTY) ){

        if( chanP->created == LZ25_CREATE_TYPE_LATE ){
            LocTyTransmitInt(chanP);
        }
        else if( chanP->created == LZ25_CREATE_TYPE_AT_BOOT ){
            LocSioTransmitInt(chanP);
        }
    }

    if( (interruptIdent == MZ25_MODEM_STAT) ){
        LocModemInt(chanP);
    }
}/* LocInterruptRoutine */

/**********************************************************************/
/** Main interrupt routine
 *
 *  This routine does the global interrupt proccessing and calls the
 *  LocInterruptRoutine function with the correct channel.
 *
 *  \param chanP            channel structure
 *
 *  \return no return value
 */
LOCAL void LocInterrupt(Z25_TY_CO_DEV_TS *chanP){

    if( chanP->uartCore == IZ25_MODID_2){
        LocInterruptRoutine(chanP);
    }
    else {
        u_int8 interruptSource = 0;     /* interrupt source */

        /* read out global interrupt register, to detect which
           UART channel caused the interrupt */
        interruptSource = MZ25_UART_INTERRUPT(chanP->baseAddr);

        /* UART 0..3 interrupt ? */
        if( interruptSource > 0x00 ){   /* yes */
            if( (interruptSource&MZ25_U1_IRQ) == MZ25_U1_IRQ ){
                /* channel 0 interrupt handling */
                LocInterruptRoutine(chanP);
            }

            if( (interruptSource&MZ25_U2_IRQ) == MZ25_U2_IRQ ){
                /* channel 1 interrupt handling */
                LocInterruptRoutine((chanP+1));
            }

            if( (interruptSource&MZ25_U3_IRQ) == MZ25_U3_IRQ ){
                /* channel 2 interrupt handling */
                LocInterruptRoutine((chanP+2));
            }

            if( (interruptSource&MZ25_U4_IRQ) == MZ25_U4_IRQ ){
                /* channel 3 interrupt handling */
                LocInterruptRoutine((chanP+3));
            }
        }
    }
}/* LocInterrupt */

/**********************************************************************/
/** Routine to initialize driver handle
 *
 *  This routine allocates the memory for the Z25 module handle.
 *  If debugging is enabled, the debug library will be initialized.
 *  File local function !
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocInitHandle(Z25_HDL **hdlP){
    Z25_DEV_TS *z25DevP;      /* Z25 resources */
    u_int32 gotSize = 0;        /* size of memory block */

    if( (z25DevP = (Z25_DEV_TS *)
        OSS_MemGet(NULL, sizeof(Z25_DEV_TS),&gotSize)) == NULL ){
        return Z25_ERROR;
    }


    OSS_MemFill(NULL, gotSize, (char*)z25DevP, 0);

    DBGINIT(("Z25Driver", (DBG_HANDLE **)&z25DevP->dbgHdlP));

    *hdlP = z25DevP;
    return Z25_OK;
}/* LocInitHandle */

/**********************************************************************/
/** Routine to initialize UART channels
 *
 *  This routine initializes a UART channel to its default values. It must
 *  be called for each channel.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param unit         quad UART unit to connect and enable interrupt
 *  \param channel      channel number
 *  \param createType   channel number
 *
 *  \return no return value
 */
LOCAL STATUS LocInitChannels(Z25_HDL *hdlP,
                           u_int16 unit,
                           u_int16 channel,
                           u_int8 createType ){
    Z25_DEV_TS *z25DevP;  /* Z25 resources */
    HDL_16Z25 *unitHdlP;   /* 16Z025 handle */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25DevP = hdlP;
    }

    unitHdlP = (HDL_16Z25 *)z25DevP->quadUart[unit][channel].unitHdlP;

    DBGWRT_2((z25DevP->dbgHdlP,
        "Z25/Z125 - channel %d (%d.unit) initialization (address=0x%x, irq=0x%x)\n",
        channel,
        (unit+1),
        z25DevP->quadUart[unit][channel].addr,
        (u_int8)z25DevP->quadUart[unit][channel].irq));

    /* disable all interrupts */
    MZ25_DisableInterrupt(unitHdlP, 0);

    /* /\* set UART to RS232 mode as default *\/  ts: moved to LocOpen() */
    /* MZ25_SetSerialMode(unitHdlP, MZ25_MODE_RS232); */

    /* set baudrate */
    MZ25_SetSerialParameter(unitHdlP,
        (MZ25_DATABITS_8 | MZ25_STOPBITS_1 | MZ25_NO_PARITY));

    MZ25_SetBaudrate(unitHdlP, Z25_DEFAULT_BAUDRATE);

    /* enable FIFO, set initial FIFO trigger level to 4 bytes */
    MZ25_SetFifoTriggerLevel(unitHdlP, 0, Z25_RX_FIFO_TRIG_LEVEL);  /* rx trigger level */
    MZ25_SetFifoTriggerLevel(unitHdlP, 1, Z25_TX_FIFO_TRIG_LEVEL);  /* tx trigger level */

    MZ25_SetRts(unitHdlP, TRUE);
    MZ25_SetDtr(unitHdlP, TRUE);
    MZ25_SetOut1(unitHdlP, FALSE);
    MZ25_SetOut2(unitHdlP, FALSE);

    z25DevP->quadUart[unit][channel].created  = createType;

    return Z25_OK;
}/* LocInitChannels */

/**********************************************************************/
/** Routine to connect and enable the interrupt
 *
 *  This routine connects the UART interrupt with the PCI interrupt and
 *  enables it.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param unit         quad UART unit to connect and enable interrupt
 *  \param channel      channel number
 *
 *  \return Z25_OK or Z25_ERROR
 */
LOCAL STATUS LocEnableInterrupt(Z25_HDL *hdlP,
                                u_int16 unit,
                                u_int16 channel){
    Z25_DEV_TS *z25DevP;  /* Z25 resources */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25DevP = hdlP;
    }

    DBGWRT_1((z25DevP->dbgHdlP,
        "Z25/Z125 - LocEnableInterrupt unit %d, channel %d (address=0x%lx, irq=0x%x)\n",
        unit, channel,
        z25DevP->quadUart[unit][0].addr,
        z25DevP->quadUart[unit][0].irq));

    /* Connect.. */
    (*z25DevP->irqFct.fIntConnectP)(
#ifdef Z25_USE_VXBPCI_FUNCS
    (VXB_DEVICE_ID)(G_Z25_vxbDevID),
#endif
    (VOIDFUNCPTR*) INUM_TO_IVEC(INT_NUM_GET((int)z25DevP->quadUart[unit][0].irqvec)),
    (VOIDFUNCPTR)  LocInterrupt,
    (int)          &z25DevP->quadUart[unit][0] );

    /* ..and enable the IRQ */
    (*z25DevP->irqFct.fIntEnableP)(z25DevP->quadUart[unit][0].irq);

    /* enable receive interrupt of channel */
    MZ25_EnableInterrupt( (HDL_16Z25 *)z25DevP->quadUart[unit][channel].unitHdlP,
                          MZ25_RDAIEN );

    return Z25_OK;
} /* LocEnableInterrupt */

/**********************************************************************/
/** Routine to convert a string to hexadecimal values
 *
 *  This routine converts a byte coded string to its hexdecimal values.
 *
 *  \param pathStringP      device path string
 *  \param devicePathP      device path hexadecimal
 *
 *  \return no return value
 */
LOCAL void LocBuildPciPath(int8 *pathStringP,
                           int8 *devicePathP){
    u_int8 i = 0;           /* loop index */
    u_int8 hlpIndex = 0;    /* index of devicePathP */



    for( i=0; i<strlen((char*)pathStringP); i++ ){
        if( pathStringP[i] == 'x' ){
            int8 tmpString[3];
            u_int32 tmp = 0;

            strncpy((char*)&tmpString[0],(char*)&pathStringP[i+1], 2);
            tmpString[2] = '\0';

            sscanf((char*)tmpString, "%x", (unsigned int *)(void*)&tmp);

            devicePathP[hlpIndex] = (int8)(tmp&0xff);

            ++hlpIndex;
        }
    }
}/* LocBuildPciPath */

/**
 * \defgroup _Z25_GLOB_FUNC Z25 driver functions
 *  @{
 */
/*--------------------------------------*/
/*    PUBLIC PROTOTYPES                 */
/*--------------------------------------*/
/**********************************************************************/
/** Routine to get the driver identification
 *
 *  This routine prints the build date, time, module name and author
 *  to the standard output (console).
 *
 *  \return no return value
 */
void Z25_DriverId(void){
    printf("%s\n", G_z25RcsId );
}/* Z25DriverId */

/**********************************************************************/
/** Routine to find Chameleon devices
 *
 *  This routine does the complete procedure to find the 16Z025/16Z125
 *  and 16Z057 UART units and to initialize them. It calls the necessary
 *  subroutines.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param pathIndex    index of PCI path in its array
 *  \param unitP        first found unit
 *  \param maxUnitsP    total number of units found
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_FindUartUnits(Z25_HDL *hdlP,
                          u_int16 pathIndex,
                          u_int16 *unitP,
                          u_int16 *maxUnitsP){
    Z25_DEV_TS *z25HdlP;      /* Z25 resources */
    CHAM_FUNCTBL        chamFctTable;   /* Chameleon function table */
    CHAMELEONV2_HANDLE *chamHdl;        /* Chameleon handle */
    CHAMELEONV2_FIND    chamFind;       /* Chameleon find */
    CHAMELEONV2_UNIT    chamInfo;       /* Chameleon unit information */
    u_int16 i;                          /* for loop index */
    u_int16 k = 0;                      /* while loop index */
    u_int16 unit = 0;                   /* UART unit counter */
    int32 irq = 0;                         /* UART unit interrupt */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25HdlP = hdlP;
    }

    i = pathIndex;

    DBGWRT_1((z25HdlP->dbgHdlP, "Z25_FindUartUnits: pathIndex = %d @ PCI dom./b.d.f %d/%d.%d.%d\n",
            pathIndex,
            OSS_DOMAIN_NBR(z25HdlP->pathInfo[i].pci.bus),
            OSS_BUS_NBR(z25HdlP->pathInfo[i].pci.bus),
            z25HdlP->pathInfo[i].pci.dev,
            z25HdlP->pathInfo[i].pci.fct ));

    if(CHAM_INIT(&chamFctTable) != CHAMELEON_OK){
        DBGWRT_ERR( (z25HdlP->dbgHdlP,
            "*** Z25/Z125 - Chameleon V2 initialization failed.\n") );
        return Z25_ERROR;
    }
    /* when the MDIS project for the CPU on which this driver runs
     * is compiled with OSS_VXBUS_SUPPORT then the PCI domain which should be
     * merged into the upper 16bit of bus nr. are evaluated as the PCI domain
     * */
    if( chamFctTable.InitPci( NULL,
                              z25HdlP->pathInfo[i].pci.bus,
                              z25HdlP->pathInfo[i].pci.dev,
                              z25HdlP->pathInfo[i].pci.fct,
                              &chamHdl ) != CHAMELEON_OK )
    {
        DBGWRT_ERR( (z25HdlP->dbgHdlP,
            "*** Z25/Z125 - Chameleon V2 PCI initialization failed.\n") );
        return Z25_ERROR;
    }

    while( G_modId[k] != IZ25_MODID_END ){
        /* find 16Z025/16Z125 and 16Z057 units in Chameleon table */
        chamFind.devId    =  CHAM_ModCodeToDevId( G_modId[k] );
        chamFind.variant  = -1; chamFind.instance = -1;
        chamFind.busId    = -1; chamFind.group    = -1;
        chamFind.bootAddr = -1;

        while( chamFctTable.InstanceFind( chamHdl,
                                          unit,
                                          chamFind,
                                          &chamInfo,
                                          NULL,
                                          NULL ) == CHAMELEONV2_UNIT_FOUND )
        {
            DBGWRT_2( (z25HdlP->dbgHdlP,
                    "Z25/Z125 - uart id 0x%lx found in chameleon table (unit=%d)\n",
                    G_modId[k], unit) );

            if((*maxUnitsP >= Z25_MAX_UNITS) || (z25HdlP->no16Z25Dev >= Z25_MAX_UNITS) ){
                /* maximum number of supported UART units reached */
                /* should appear as error, but return Z25_OK because
                   devices were correctly recognized and initialized */
                z25HdlP->no16Z25Dev = Z25_MAX_UNITS;
                DBGWRT_ERR( (z25HdlP->dbgHdlP,
                "*** Z25/Z125 - Maximum number of supported devices reached !\n") );

                DBGWRT_2( (z25HdlP->dbgHdlP,
                    "Z25/Z125 - %d 16Z25/16Z125 uart(s) found => %d single channels.\n",
                    z25HdlP->no16Z25Dev, z25HdlP->noUarts) );
                return Z25_OK;
            }

            if( z25HdlP->usePciIrq == Z25_PCI_IRQ_ENABLED ){
                 DBGWRT_2( (z25HdlP->dbgHdlP,
                    "Z25/Z125 - uart unit %d uses PCI interrupt.\n",
                    unit) );

                OSS_PciGetConfig( NULL,
                                  z25HdlP->pathInfo[i].pci.bus,
                                  z25HdlP->pathInfo[i].pci.dev,
                                  z25HdlP->pathInfo[i].pci.fct,
                                  OSS_PCI_INTERRUPT_LINE,
                                  &irq );
            }
            else {
                DBGWRT_2( (z25HdlP->dbgHdlP,
                    "Z25/Z125 - uart unit %d uses chameleon interrupt.\n",
                    unit) );
                irq = chamInfo.interrupt + z25HdlP->irqOffset;
            }

            /* initalize UART unit */
            if( LocInitUartUnit(hdlP, (u_int32)chamInfo.addr, irq, G_modId[k]) != 0 ){
                DBGWRT_ERR((z25HdlP->dbgHdlP,
                    "*** Z25/Z125 - Error initializing uart unit %d.\n", unit));
                return Z25_ERROR;
            }

            /* store start unit */
            if( i == 0 ){/* first device */
                z25HdlP->pathInfo[i].start16Z25Unit = 0;
            }
            else{/* second and follwing devices */
                z25HdlP->pathInfo[i].start16Z25Unit =
                    z25HdlP->pathInfo[i-1].no16Z25Units +
                    z25HdlP->pathInfo[i-1].start16Z25Unit;
            }

            /* no of units in PCI path */
            *unitP = z25HdlP->pathInfo[i].start16Z25Unit;
            if( *maxUnitsP < Z25_MAX_UNITS ){
                *maxUnitsP = ++z25HdlP->pathInfo[i].no16Z25Units;
            }

            unit++;
        }/* while */
        k++;
    } /* end for */

    DBGWRT_2( (z25HdlP->dbgHdlP,
        "Z25/Z125 - %d 16Z25/16Z125 uart(s) found => %d single channels.\n",
        z25HdlP->no16Z25Dev, z25HdlP->noUarts) );

    return Z25_OK;
} /* Z25_FindUartUnits */

/**********************************************************************/
/** Routine to set the interrupt functions and the IRQ base offset
 *
 *  This routine sets the function pointers for interrupt connect and
 *  enable functions. In addition the IRQ base offset is set.
 *
 *  \param hdlP             Z25 handle returned by Z25_Init
 *  \param irqBase          external interrupt base offset
 *  \param intConnectAddr   interrupt connect function pointer
 *  \param intEnableAddr    interrupt enable function pointer
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_SetIntFunctions(Z25_HDL *hdlP,
                            u_int32 irqBase,
                            FUNCPTR intConnectAddr,
                            FUNCPTR intEnableAddr){
    Z25_DEV_TS *z25DevP;      /* Z25 resources */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25DevP = hdlP;
    }

    z25DevP->irqFct.extIrqBase = irqBase;

    /* previous non-pci domain behaviour */
    if( intConnectAddr == NULL ){/* take VxWorks pciIntConnect */

        printf("Z25_SetIntFunctions: No ISR connect routine specified. Using ");
#ifndef Z25_USE_VXBPCI_FUNCS
        printf("pciIntConnect.\n");
        z25DevP->irqFct.fIntConnectP = (FUNCPTR)pciIntConnect;
#else
        printf("intConnect.\n");
        z25DevP->irqFct.fIntConnectP = (FUNCPTR)vxbPciIntConnect;
#endif
    }
    else{/* take user interrupt connect function */
        z25DevP->irqFct.fIntConnectP = (FUNCPTR)intConnectAddr;
    }

    /* intEnable works both in non-vxBus and vxBus environments */
    if( intEnableAddr == NULL ){/* take VxWorks intEnable */
        printf("Z25_SetIntFunctions: No IRQ enable routine specified. Using intEnable.\n");
        z25DevP->irqFct.fIntEnableP = (FUNCPTR)intEnable;
    }
    else{/* take user interrupt enable function */
        z25DevP->irqFct.fIntEnableP = (FUNCPTR)intEnableAddr;
    }



    return Z25_OK;
}/* Z25_SetIntFunctions */

/**********************************************************************/
/** Routine to set the UART clock frequency
 *
 * This routine sets the UART clock frequency.
 * This routine must be called before the baudrate adjustment takes place.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param frequency    system frequency, e.g. 33000000
 *  \param unit         UART unit
 *
 *  \return no return value
 */
STATUS Z25_SetBaseBaud(Z25_HDL *hdlP,
                        u_int32 frequency,
                        u_int16 unit){
    Z25_DEV_TS *z25DevP;    /* Z25 resources */
    HDL_16Z25 *unitHdlP;    /* 16Zx25 handle */
    /* TS_16Z25_UNIT *tmphdlP;*//* 16Z025 resources */
    u_int16 j;              /* loop index */
    int32 baudrate = 0;     /* baudrate value */
    u_int16 maxChannels = 0;

    if( hdlP == NULL ){
        return 1;
    }
    else {
        z25DevP = hdlP;
    }

    /* set UART clock frequency for all channels */
    if( z25DevP->quadUart[unit][0].uartCore == IZ25_MODID_2){
        maxChannels = IZ25_MIN_UARTS_PER_DEV;
    }
    else {
        maxChannels = Z25_MAX_UARTS_PER_DEV;
    }

    for(j=0; j<maxChannels; j++ ){
        unitHdlP = (HDL_16Z25 *)z25DevP->quadUart[unit][j].unitHdlP;

        if( unitHdlP != NULL ){
            MZ25_GetBaudrate(unitHdlP, &baudrate);
            MZ25_SetBaseBaud(unitHdlP, frequency);
            MZ25_SetBaudrate(unitHdlP, baudrate);
        }
    }

    return Z25_OK;
}/* Z25_SetBaseBaud */

/**********************************************************************/
/** Routine to initialize the driver resources
 *
 *  This routine allocates memory for the device resources and calls the
 *  routine LocFindChameleonDevices to find the devices.
 *
 *  \return Z25_HDL - Z25 handle
 */
Z25_HDL * Z25_InitDriver(void){
    Z25_DEV_TS *z25DevP = NULL;      /* Z25 resources */

    /*------------------------------+
     | initialization section       |
     +------------------------------*/
    /* create Z25 handle */
    if( LocInitHandle((Z25_HDL *)&z25DevP) != Z25_OK ){
        return NULL;
    }

    /*----------------------------+
    | Find Chameleon Devices      |
    +-----------------------------*/
    logMsg("Z25_InitDriver:\n",1,2,3,4,5,6);
    if( LocFindChameleonDevices(z25DevP, (G_Z25_ChamPciDomain & 0xff) ) != Z25_OK ){
        DBGWRT_ERR( (z25DevP->dbgHdlP,
            "*** Z25/Z125 - No Chameleon devices found.\n") );
        Z25_FreeHandle((Z25_HDL *)&z25DevP);
        return NULL;
    }

    logMsg("OK\n",1,2,3,4,5,6);

    return z25DevP;
}/* Z25_InitDriver */

/**********************************************************************/
/** Routine to parse a directly specified PCI location for a
 *  Z(1)25 UART containing device
 *
 *  The string must be directly passed in the form of "PCIX:BBB.DD:FF"
 *  (bus/device/function in DECIMAL numbers. Bus=[0:255] device=[0:31],
 *  function is usually 0 (MEN Chameleon FPGA contains no multifunction
 *  PCI headers).
 *
 **/
LOCAL STATUS DRV_ParseDirectPciLoc( char *pStr, int* dom, int* bus, int* dv, int* fn)
{
    #define PCI_LOC_NR_ITEMS    4  /* we want domain,bus,dev,fct number to be parsed */

    STATUS errval = OK;

    if (sscanf(pStr, "PCI%d:%d.%d.%d", dom, bus, dv, fn ) < PCI_LOC_NR_ITEMS)
    {
        errval = ERROR;
    }

    return errval;
}

/**********************************************************************/
/** Routine to install driver in the VxWorks I/O system
 *
 *  This routine is used to install the driver after the boot sequence.\n
 *  Z25_CreateDevice("/tyZ25", "0x1e 0x0e",1,0,0x80,0,intConnect,inEnable)\n
 *  -> "tyZ25", e.g. for device name string\n
 *  -> "0x1e 0x... 0x0e", 0x1e and following=device number of bridges and
 *     0x0e=device number of desired PCI device \n
 *  If intConnectP or intEnableP is zero, the default VxWorks functions
 *  pciIntConnect and intEnable are used.
 *
 *  \param devNameP         serial device name
 *  \param pathStringP      PCI path of device
 *  \param usePciIrq        use PCI irq
 *  \param irqBase          interrupt base
 *  \param irqOffset        BSP specific irq offset for chameleon V2 FPGAs
 *                          for chameleon V1 devices always zero
 *  \param uartFreq         uart frequency base, if 0 DOS compatibility freqency
 *                          (1.8432MHz)
 *  \param intConnectAddr   interrupt connect function pointer
 *  \param intEnableAddr    interrupt enable function pointer
 *
 *  \return Z25_HDL - Z25 handle or NULL in case of error
 */
Z25_HDL * Z25_CreateDevice(int8 *devNameP,
                           int8 *pathStringP,
                           u_int8  usePciIrq,
                           u_int32 irqBase,
                           u_int16 irqOffset,
                           u_int32 uartFreq,
                           FUNCPTR intConnectAddr,
                           FUNCPTR intEnableAddr )
{
    Z25_DEV_TS *z25DevP=NULL;               /* Z25 resources */
    int8 tmpDevicePath[IZ25_PATH_LENGTH];   /* hexadecimal device path */
    u_int8 i, j, l_const  = 255;            /* loop index */
    u_int8 k = 0;                           /* index */
    int pdom, pb, pd, pf;                   /* PCI domain, bus, dev, function */
    u_int16 unit = 0;                       /* UART unit */
    u_int16 maxUnit = 0;                    /* number of units */
    u_int16 pathIndex = 0;                  /* array index of PCI path */
    u_int16 loopIndex = 0;                  /* loop index */
    u_int32 frequency = 0;

    bzero((void*)tmpDevicePath, sizeof(tmpDevicePath));

    /*------------------------------+
     | check input parameter        |
     +------------------------------*/
    if( devNameP == NULL ){
        return NULL;
    }

    if( pathStringP == NULL ){
        return NULL;
    }

    /*------------------------------+
     | initialization section       |
     +------------------------------*/
    /* create Z25 handle */
    if( (z25DevP=Z25_InitDriver()) == NULL ){
        printf("*** Z25_InitDriver() failed, exit.\n");
        return NULL;
    }

    /* basic settings */
    z25DevP->irqOffset = irqOffset;

    if( uartFreq == 0 ){
        frequency = IZ25_DOS_COMPATIBILITY;
    }
    else {
        frequency = uartFreq;
    }

    if( usePciIrq > 0 ){
        z25DevP->usePciIrq = Z25_PCI_IRQ_ENABLED;
    }

    /* set interrupt functions */
    Z25_SetIntFunctions(z25DevP, irqBase, intConnectAddr, intEnableAddr);

    /* convert hex string to number. If a direct pci target is specified
     * in the form of "PCIn:b.d.f" (e.g. "PCI1:2.3.0" = PCI domain 1, bus 2 dev. 3 func. 0)
     * then use this. Also needed for newer vxBus architecture */

    /* ts@men: poor mans strstr() variant... */
    if (pathStringP[0] == 'P' && pathStringP[1] == 'C' && pathStringP[2] == 'I')
    {

        DBGWRT_2((z25DevP->dbgHdlP,
                  "Z25_CreateDevice: direct PCI location specified, searching UART units..\n" ));

        /* direct PCI device specified, omit PCI path scanning. Interpret the number behind "PCI */
        pathIndex = 0;
        DRV_ParseDirectPciLoc((char*)pathStringP, &pdom,&pb,&pd,&pf);
        z25DevP->pathInfo[pathIndex].pci.bus    = OSS_MERGE_BUS_DOMAIN(pb, pdom);
        z25DevP->pathInfo[pathIndex].pci.dev    = (u_int16)pd;
        z25DevP->pathInfo[pathIndex].pci.fct    = (u_int16)pf;

        if( Z25_FindUartUnits(z25DevP, pathIndex, &unit, &maxUnit) != Z25_OK ){
            DBGWRT_ERR((z25DevP->dbgHdlP,
                        "*** Z25/Z125 - Error initializing chameleon units.\n"));
            goto CLEANUP;
        }
    }
    else
    {
        DBGWRT_2((z25DevP->dbgHdlP, "Z25_CreateDevice: standard PCI path specified, iterating through it.\n" ));

            /* regular search path */
        LocBuildPciPath(pathStringP, tmpDevicePath);
    
            /* find units */
        if( Z25_GetPciPathInfo((Z25_HDL *)z25DevP, tmpDevicePath, &pathIndex) != Z25_OK ){
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25/Z125 - Unknown PCI bus path !\n"));
            goto CLEANUP;
        }
    
            DBGWRT_2((z25DevP->dbgHdlP, "Z25_GetPciPathInfo returned pathIndex %d\n", pathIndex));
    
        if( Z25_FindUartUnits(z25DevP, pathIndex, &unit, &maxUnit) != Z25_OK ){
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25/Z125 - Error initializing chameleon units.\n"));
            goto CLEANUP;
        }
    }

    z25DevP->pathInfo[pathIndex].installed = TRUE;

    for(j=unit; j<(unit+maxUnit); j++){
        /*------------------------------+
        | driver installation section  |
        +------------------------------*/

    /* KlocWork: check validity of index j first */
        if ( j >= IZ25_MAX_UNITS ) {
                    DBGWRT_ERR((z25DevP->dbgHdlP, "*** Z25/Z125 - internal bug: too many /tty instances found ?\n"));
                    goto CLEANUP;
        }

        if( z25DevP->quadUart[j][0].uartCore == IZ25_MODID_2 ){
            if( k >= Z25_MAX_UARTS_PER_DEV ){
                k = 0;
            }
            loopIndex = IZ25_MIN_UARTS_PER_DEV;
            ++k;
            if( l_const == 255 ){
                l_const = j;
            }
        }
        else {
            k = 1;
            loopIndex = Z25_MAX_UARTS_PER_DEV;
        }

        if( k == 1 ){
            if( Z25_GetIosDriverNumber((Z25_HDL *)z25DevP, j) != Z25_OK ) {
                DBGWRT_ERR((z25DevP->dbgHdlP, "*** Z25/Z125 - Z25_GetIosDriverNumber\n"));
                goto CLEANUP;
            }
        }

        if( (z25DevP->quadUart[j][0].uartCore == IZ25_MODID_2) && ( k <= Z25_MAX_UARTS_PER_DEV )) {
                /* four z125 uarts are sharing one interrupt */
            z25DevP->driverNumber[j] =  z25DevP->driverNumber[l_const];
        }

        /* set base baud for all units in IP core */
        Z25_SetBaseBaud((Z25_HDL *)z25DevP, frequency, j);

        for(i=0; i<loopIndex; i++ ){
            if( Z25_InstallTtyInterface( (Z25_HDL *)z25DevP, devNameP, j, i, Z25_RX_BUFF_SIZE, Z25_TX_BUFF_SIZE) != Z25_OK ) {
                DBGWRT_ERR((z25DevP->dbgHdlP,
                    "*** Z25/Z125 - Z25_InstallTtyInterface\n"));
                goto CLEANUP;
            }
        }
    }
    DBGWRT_2((z25DevP->dbgHdlP,
        "Z25/Z125 - Driver initialization finished.\n"));

    DBGWRT_1((z25DevP->dbgHdlP,
        "Z25/Z125 - UART base frequency set to %ld Hz\n", frequency));

    return (Z25_HDL *)z25DevP;

    CLEANUP:
    DBGWRT_ERR((z25DevP->dbgHdlP,
        "*** Z25/Z125 - Driver initialization aborted.\n"));

    Z25_FreeHandle((Z25_HDL *)&z25DevP);
    return NULL;
}/* Z25_CreateDevice */

/**********************************************************************/
/** Routine to initialize the driver at boot time
 *
 *  This routine is used to make the BSP initialization of the driver.
 *  It must be called in sysSerialHwInit2. Before using this function call
 *  Z25_InitDriver and Z25_FindUartUnits to obtain a valid Z25 handle
 *  and the available UART units.
 *
 *  \param hdlP             Z25 handle returned by Z25_Init
 *  \param unit             number of 16Zx25 unit
 *  \param channel          number of channel in unit
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_InitDriverAtBoot(Z25_HDL *hdlP,
                             u_int16 unit,
                             u_int16 channel)
{
    STATUS error = Z25_OK;
    Z25_DEV_TS *z25DevP;  /* Z25 resources */

    if( (z25DevP = hdlP) == NULL ||
        z25DevP->quadUart[unit][channel].addr == 0 ){
        return Z25_ERROR;
    }

    DBGWRT_1((z25DevP->dbgHdlP,
        "Z25/Z125 - Z25_InitDriverAtBoot\n"));

    z25DevP->quadUart[unit][channel].u.sioT.pDrvFuncs  =
        (SIO_DRV_FUNCS  *)&G_sioDrvFuncs;

    error = LocInitChannels(z25DevP, unit, channel, LZ25_CREATE_TYPE_AT_BOOT);

    if( error == Z25_OK ){
        LocEnableInterrupt(z25DevP, unit, channel);
    }
    return error;
} /* Z25_InitDriverAtBoot */

/**********************************************************************/
/** Routine to install the UARTs in the VxWorks TTY environment
 *
 *  This routine install the UARTs in the VxWorks TTY environment.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param devNameP     device name
 *  \param unit         unit to install
 *  \param noOfChan     number of channels to install
 *  \param rxBuffSize   Rx buffer size
 *  \param txBuffSize   Tx buffer size
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_InstallTtyInterface(Z25_HDL * hdlP,
                                int8 *devNameP,
                                u_int16 unit,
                                u_int16 noOfChan,
                                u_int16 rxBuffSize,
                                u_int16 txBuffSize){
    Z25_DEV_TS *z25DevP;  /* Z25 resources */
    int8 tyName[IZ25_MAX_DEV_NAME];        /* TTY device name */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25DevP = hdlP;
    }

    bzero((void*)tyName, sizeof(tyName));

    if( z25DevP->quadUart[unit][noOfChan].addr != 0 ){/* init ty interface */
        if( tyDevInit( &z25DevP->quadUart[unit][noOfChan].u.tyDev,
                        rxBuffSize,
                        txBuffSize,
                       (TY_LOC_CAST)LocStartup ) != OK ){
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25/Z125 - Error tyDevInit\n"));
            return Z25_ERROR;
        }

        LocInitChannels(z25DevP, unit, noOfChan, LZ25_CREATE_TYPE_LATE);

        /* add channel list to io system */
        if( z25DevP->quadUart[unit][noOfChan].uartCore == IZ25_MODID_2 ){
            /*  each z125 unit does only consist of one uart channel */
            sprintf ((char*)tyName, "%s%s%d", devNameP, "/", unit);
        }
        else {
            sprintf ((char*)tyName, "%s%d%s%d", devNameP, unit, "/", noOfChan);
        }
        
        /* store channel nr so we can unit value  */
        z25DevP->quadUart[unit][noOfChan].unit = noOfChan;

        if( iosDevAdd(&z25DevP->quadUart[unit][noOfChan].u.tyDev.devHdr,
                      (char*)tyName, z25DevP->driverNumber[unit]) ){
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25/Z125 - Error iosDevAdd\n"));
            return Z25_ERROR;
        }
    }
    LocEnableInterrupt(z25DevP, unit, noOfChan);

    return Z25_OK;
}/* Z25_InstallTtyInterface */

/**********************************************************************/
/** Routine to get the UART unit information
 *
 *  This routine checks if the given PCI path corresponds with a PCI
 *  path found by the PCI scan at driver initialization time.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param devicePathP  device path
 *  \param pathIndexP   array index of pciInfo
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_GetPciPathInfo(Z25_HDL *hdlP,
                           int8 *devicePathP,
                           u_int16 *pathIndexP){
    u_int16 i, j;           /* loop index */
    Z25_DEV_TS *z25DevP;  /* Z25 resources */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25DevP = hdlP;
    }

    for( i=0; i<z25DevP->noPciPaths; i++ ){
        for( j=0; j<z25DevP->pathInfo[i].pci.pathLen; j++ ){
            if( *(devicePathP+j) != z25DevP->pathInfo[i].pci.path[j] ){
                /* last PCI path reached */
                if( i == (z25DevP->noPciPaths-1) ){
                    /* yes, we can return with a message */
                    printf("*** Z25/Z125 - Unknown pci path ! ***\n");
                    return Z25_ERROR;
                }
                else{   /* end for j loop */
                    j = z25DevP->pathInfo[i].pci.pathLen;
                }
            }
            else{
                if( j == (z25DevP->pathInfo[i].pci.pathLen-1) ){
                    *pathIndexP = i;
                    /* end for i loop */
                    i = z25DevP->noPciPaths;
                }
            }
        }
    }

    return Z25_OK;
}/* Z25_GetPciPathInfo */

/**********************************************************************/
/** Routine to install the driver in the VxWorks I/O system
 *
 *  This routine installs a quad UART unit in the VxWorks I/O system.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *  \param unit         unit to install
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_GetIosDriverNumber(Z25_HDL * hdlP,
                              u_int16 unit){
    Z25_DEV_TS *z25DevP;  /* Z25 resources */

    if( hdlP == NULL ){
        return Z25_ERROR;
    }
    else{
        z25DevP = hdlP;
    }
#if 0   /* parameter reminder  */
int iosDrvInstall
    (
    FUNCPTR pCreate,          /* pointer to driver create function */
    FUNCPTR pDelete,          /* pointer to driver delete function */
    FUNCPTR pOpen,            /* pointer to driver open function */
    FUNCPTR pClose,           /* pointer to driver close function */
    FUNCPTR pRead,            /* pointer to driver read function */
    FUNCPTR pWrite,           /* pointer to driver write function */
    FUNCPTR pIoctl            /* pointer to driver ioctl function */
    )
#endif
    if( z25DevP->driverNumber[unit] == 0 ){/* only once */
        z25DevP->driverNumber[unit] = iosDrvInstall( LocCreate,
                                                     (FUNCPTR)NULL,
                                                     LocOpen,
                                                     LocClose,
                                                     tyRead,
                                                     tyWrite,
                                                     LocIoctl );
        if( z25DevP->driverNumber[unit] == ERROR ){
            DBGWRT_ERR((z25DevP->dbgHdlP,
                "*** Z25/Z125 - Driver installation in i/o system failed !\n"));
             DBGWRT_1((z25DevP->dbgHdlP, "*** Z25/Z125 - driverNumber[%d]=%d\n",
                unit, z25DevP->driverNumber[unit]));
            return Z25_ERROR;
        }
        else{
            DBGWRT_2((z25DevP->dbgHdlP, "Z25/Z125 - driverNumber[%d]=%d\n",
                unit, z25DevP->driverNumber[unit]));
        }
    }
    else{
        DBGWRT_ERR((z25DevP->dbgHdlP,
            "*** Z25/Z125 - Driver already installed.\n"));
        return Z25_ERROR;
    }

    return Z25_OK;
}/* Z25_GetIosDriverNumber */

/**********************************************************************/
/** Routine to free driver handle
 *
 *  This routine frees the Z25 driver resources, which were allocated
 *  by LocInitHandle.
 *
 *  \param hdlP         Z25 handle returned by Z25_Init
 *
 *  \return Z25_OK or Z25_ERROR
 */
STATUS Z25_FreeHandle(Z25_HDL **hdlP){
    Z25_DEV_TS *z25DevP;      /* Z25 resources */
    u_int16 i, j;
    u_int16 loopIndex = 0;

    z25DevP = *hdlP;

    if( z25DevP == NULL ){
        return Z25_ERROR;
    }

    DBGEXIT((&z25DevP->dbgHdlP));

    /* free low-level instance, if already initialized */
    for(i=0; i<z25DevP->no16Z25Dev; i++){
        if( z25DevP->quadUart[i][0].uartCore == IZ25_MODID_2){
            loopIndex = IZ25_MIN_UARTS_PER_DEV;
        }
        else {
            loopIndex = Z25_MAX_UARTS_PER_DEV;
        }

        for(j=0; j<loopIndex; j++ ){
            if( z25DevP->quadUart[i][j].unitHdlP != NULL ){
                MZ25_FreeHandle(&z25DevP->quadUart[i][j].unitHdlP);
            }
        }
    }

    OSS_MemFree(NULL, (void *)z25DevP, sizeof(Z25_DEV_TS));
    z25DevP = NULL;

    *hdlP = z25DevP;

    return Z25_OK;
}/* Z25_FreeHandle */

#ifdef DBG
/**********************************************************************/
/** Routine to set the debug level
 *
 *  This routine sets the debug level.
 *
 *  \param level            debug level
 *
 *  \return no return value
 */
void Z25_SetDebugLevel(u_int32 level){
    G_Z25_DebugLevel = level;
}/*Z25_SetDebugLevel*/

/**********************************************************************/
/** routine to get the debug level
 *
 *  This routine returns the current adjusted debug level.
 *
 *  \return current debug level
 */
u_int32 Z25_GetDebugLevel(void){
    return G_Z25_DebugLevel;
}/*Z25_GetDebugLevel*/
/*! @} */
#endif










