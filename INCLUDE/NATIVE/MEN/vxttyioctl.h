/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: vxttyioctl.h
 *      Project:
 *
 *       Author: sv
 *        $Date: 2012/05/15 17:37:35 $
 *    $Revision: 2.9 $
 *
 *  Description: MEN specific VxWorks TTY ioctl definitions
 *
 *     Required:
 *     Switches:
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: vxttyioctl.h,v $
 * Revision 2.9  2012/05/15 17:37:35  channoyer
 * R: Not possible to read the UART mode
 * M: Add ioctl MEN_UART_IOCTL_MODE_GET
 *
 * Revision 2.8  2008/06/09 13:17:36  SVogel
 * R:1. each transmitted byte generated an interrupt,
 *      if fifo trigger level was set to 1
 * M:1. added additional i/o control to set fifo tx trigger level
 *
 * Revision 2.7  2008/02/26 11:12:59  cs
 * cosmetic: added newline at end of file
 *
 * Revision 2.6  2007/10/18 17:13:21  SVogel
 * cosmetics
 *
 * Revision 2.3  2007/02/27 09:26:10  svogel
 * renamed:
 * MEN_UART_XON_XOFF      ==> MEN_UART_IOCTL_XON_XOFF
 * MEN_UART_RTS_CTS       ==> MEN_UART_IOCTL_RTS_CTS
 * MEN_UART_DTR_DSR       ==> MEN_UART_IOCTL_DTR_DSR
 * MEN_UART_HANDSHAKE_OFF ==> MEN_UART_IOCTL_HANDSHAKE_OFF
 *
 * Revision 2.2  2007/02/14 17:12:18  RLange
 * added defines (SVogel)
 *
 * Revision 2.1  2005/06/23 08:59:57  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifndef __VXTTYIOCTL_H
#define __VXTTYIOCTL_H


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------+
|   INCLUDES                             |
+--------------------------------------*/

#if _WRS_VXWORKS_MAJOR == 7

#include <sys/ioctl.h>

#define VX_IOCG_MEN_Z135 'Z'

#endif /* VXW 7 */
/*--------------------------------------+
| DEFINES |
+--------------------------------------*/
#if _WRS_VXWORKS_MAJOR == 7

#define MEN_UART_IOCTL_DATABITS	                    _IOW(VX_IOCG_MEN_Z135, 0, UINT32)
#define MEN_UART_IOCTL_PARITY                       _IOW(VX_IOCG_MEN_Z135, 1, UINT32)
#define MEN_UART_IOCTL_STOPBITS                     _IOW(VX_IOCG_MEN_Z135, 2, UINT32)
#define MEN_UART_IOCTL_SET_RTS                      _IOW(VX_IOCG_MEN_Z135, 3, UINT32)
#define MEN_UART_IOCTL_SET_DTR                      _IOW(VX_IOCG_MEN_Z135, 4, UINT32)
#define MEN_UART_IOCTL_SET_OUT1                     _IOW(VX_IOCG_MEN_Z135, 5, UINT32)
#define MEN_UART_IOCTL_SET_OUT2                     _IOW(VX_IOCG_MEN_Z135, 6, UINT32)
#define MEN_UART_IOCTL_GET_CTS                      _IOR(VX_IOCG_MEN_Z135, 7, UINT32)
#define MEN_UART_IOCTL_GET_DSR                      _IOR(VX_IOCG_MEN_Z135, 8, UINT32)
#define MEN_UART_IOCTL_GET_DCD                      _IOR(VX_IOCG_MEN_Z135, 9, UINT32)
#define MEN_UART_IOCTL_GET_DTR                      _IOR(VX_IOCG_MEN_Z135, 10, UINT32)
#define MEN_UART_IOCTL_GET_DTS                      _IOR(VX_IOCG_MEN_Z135, 11, UINT32)
#define MEN_UART_IOCTL_GET_RTS                      _IOR(VX_IOCG_MEN_Z135, 12, UINT32)
#define MEN_UART_IOCTL_GET_RING                     _IOR(VX_IOCG_MEN_Z135, 13, UINT32)



#define MEN_UART_IOCTL_MODE_SELECT                  _IOW(VX_IOCG_MEN_Z135, 14, UINT32)
#define MEN_UART_IOCTL_AUTO_RS485                   _IOW(VX_IOCG_MEN_Z135, 15, UINT32)
#define MEN_UART_IOCTL_MODE_GET                     _IOR(VX_IOCG_MEN_Z135, 16, UINT32)
#define MEN_UART_IOCTL_MODEM                        _IOW(VX_IOCG_MEN_Z135, 17, UINT32)
#define MEN_UART_IOCTL_SET_FIFO_BYTES               _IOW(VX_IOCG_MEN_Z135, 18, UINT32)
#define MEN_UART_IOCTL_GET_RCV_BYTES                _IOR(VX_IOCG_MEN_Z135, 19, UINT32)
#define MEN_UART_IOCTL_SET_TX_FIFO_BYTES            _IOW(VX_IOCG_MEN_Z135, 20, UINT32)
#define MEN_UART_IOCTL_LINE_STATUS                  _IOR(VX_IOCG_MEN_Z135, 21, UINT32)

/* handshake variants */
#define MEN_UART_IOCTL_XON_XOFF                    _IOW(VX_IOCG_MEN_Z135, 22, UINT32)
#define MEN_UART_IOCTL_RTS_CTS                     _IOW(VX_IOCG_MEN_Z135, 23, UINT32)
#define MEN_UART_IOCTL_DTR_DSR                     _IOW(VX_IOCG_MEN_Z135, 24, UINT32)
#define MEN_UART_IOCTL_HANDSHAKE_OFF               _IOW(VX_IOCG_MEN_Z135, 25, UINT32)

#else /* VXW 7 */

#define MEN_UART_IOCTL_DATABITS           100
#define MEN_UART_IOCTL_PARITY             101
#define MEN_UART_IOCTL_STOPBITS           102
#define MEN_UART_IOCTL_SET_RTS            113
#define MEN_UART_IOCTL_SET_DTR            114
#define MEN_UART_IOCTL_SET_OUT1           115
#define MEN_UART_IOCTL_SET_OUT2           116
#define MEN_UART_IOCTL_GET_CTS            117
#define MEN_UART_IOCTL_GET_DSR            118
#define MEN_UART_IOCTL_GET_DCD            119
#define MEN_UART_IOCTL_GET_DTR            120
#define MEN_UART_IOCTL_GET_DTS            121
#define MEN_UART_IOCTL_GET_RTS            122
#define MEN_UART_IOCTL_GET_RING           123

#define MEN_UART_IOCTL_MODE_SELECT        130
#define MEN_UART_IOCTL_AUTO_RS485         131
#define MEN_UART_IOCTL_MODE_GET           132
#define MEN_UART_IOCTL_MODEM              140
#define MEN_UART_IOCTL_SET_FIFO_BYTES     141
#define MEN_UART_IOCTL_GET_RCV_BYTES      142
#define MEN_UART_IOCTL_SET_TX_FIFO_BYTES  143
#define MEN_UART_IOCTL_LINE_STATUS        150

/* handshake variants */
#define MEN_UART_IOCTL_XON_XOFF           160
#define MEN_UART_IOCTL_RTS_CTS            161
#define MEN_UART_IOCTL_DTR_DSR            162
#define MEN_UART_IOCTL_HANDSHAKE_OFF      163

#endif /* VXW 7 */

#ifdef __cplusplus
}
#endif

#endif /* __VXTTYIOCTL_H */

