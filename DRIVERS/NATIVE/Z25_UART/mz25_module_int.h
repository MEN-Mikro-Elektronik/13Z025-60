/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  mz25_module_int.h
 *
 *      \author  sv
 *        $Date: 2012/11/21 23:08:20 $
 *    $Revision: 1.5 $
 *
 *        \brief  16Z025 low-level internal definitions
 *
 *            	  This header file consists of the internal definitions of the
 *                16Z025 quad UART. These definitions should not be exported
 *                to other modules !
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mz25_module_int.h,v $
 * Revision 1.5  2012/11/21 23:08:20  ts
 * R: UART regs in P513 had to be 32bit aligned to match x602 PMC 32bit accesses
 * M: added a shift factor depending on define MEN_P513
 *
 * Revision 1.4  2008/06/09 17:15:48  SVogel
 * R:1. each transmitted byte generated an interrupt,
 *      if fifo trigger level was set to 1
 * M:1. fifoTrigger splittet into fifoRxTrigger and fifoTxTrigger
 *
 * Revision 1.3  2008/02/26 10:56:03  cs
 * R:1. part of baudrate was stripped off because of used data type u_int16
 * M:1. change type of TS_16Z25_UNIT:baudrate to u_int32
 *
 * Revision 1.2  2007/07/06 14:52:52  SVogel
 * + cosmetics
 * + added RCFC bit definition to modem control register for z125 operation
 *
 * Revision 1.1  2005/06/23 08:53:06  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef	_MZ25_MODULE_INT_H
#define	_MZ25_MODULE_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------+
 |  INCLUDES        |
 +------------------*/
/*------------------+
 |  DEFINES         |
 +------------------*/
/*--------------------------------+
 |  Register definitions          |
 +--------------------------------*/

/*
 *  handle special Register layout of P513 UARTs. 
 *  Because x602 PMC slot is capable of only 32bit read access,
 *  the UARTs in P513 FPGA Rev. 5 are 4 byte aligned.
 */
#if defined(MEN_P513)
# define MZ25_REGSHFT	2
#else
# define MZ25_REGSHFT	0
#endif

#define MIZ25_THR_OFFSET		(0x00<<MZ25_REGSHFT)		/**< transmitter holding register */
#define MIZ25_RHR_OFFSET		(0x00<<MZ25_REGSHFT)		/**< receiver holding register */
#define MIZ25_FCR_OFFSET		(0x02<<MZ25_REGSHFT)		/**< FIFO control register */
#define MIZ25_IIR_OFFSET		(0x02<<MZ25_REGSHFT)		/**< interrupt status register */
#define MIZ25_IER_OFFSET		(0x01<<MZ25_REGSHFT)		/**< interrupt enable register */
#define MIZ25_LCR_OFFSET		(0x03<<MZ25_REGSHFT)		/**< line control register */
#define MIZ25_MCR_OFFSET		(0x04<<MZ25_REGSHFT)		/**< modem control register */
#define MIZ25_LSR_OFFSET		(0x05<<MZ25_REGSHFT)		/**< line status register */
#define MIZ25_MSR_OFFSET		(0x06<<MZ25_REGSHFT)		/**< modem status register */
#define MIZ25_ACR_OFFSET		(0x07<<MZ25_REGSHFT)		/**< additional control register */
#define MIZ25_DLL_OFFSET		(0x00<<MZ25_REGSHFT)		/**< divisor latch low byte */
#define MIZ25_DLH_OFFSET		(0x01<<MZ25_REGSHFT)		/**< divisor latch high byte */


#define MIZ25_IDIRQ_OFFSET	    (0x40)        /**< global interrupt register */


/*--------------------------------+
 |  IDIRQ register                |
 |  global interrupt  register    |
 +--------------------------------*/
/*
   ---------------------------------------------------------------------
  |  bit7  |  bit6  | bit5   | bit4   |  bit3  | bit2  |  bit1  |  bit0 |
     -------------------------------------------------------------------
  |U4exists|U3exists|U2exists|U1exists| U4irq  | U3irq | U2irq  | U1irq |
   ---------------------------------------------------------------------
*/
#define MIZ25_U1_IRQ			(0x01)		/**< UART 1 interrupt is pending */
#define MIZ25_U2_IRQ			(0x02)		/**< UART 2 interrupt is pending */
#define MIZ25_U3_IRQ			(0x04)		/**< UART 3 interrupt is pending */
#define MIZ25_U4_IRQ			(0x08)		/**< UART 4 interrupt is pending */
#define MIZ25_U1_EXISTS			(0x10)		/**< UART 1 exists */
#define MIZ25_U2_EXISTS			(0x20)		/**< UART 2 exists */
#define MIZ25_U3_EXISTS			(0x40)		/**< UART 3 exists */
#define MIZ25_U4_EXISTS			(0x80)		/**< UART 4 exists */

/*--------------------------------+
 |  THR register                  |
 |  transmitter holding register  |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |              databits 7..0 to be transmitted                      |
   -------------------------------------------------------------------
  initial: 0x00
*/

/*--------------------------------+
 |  RHR register                  |
 |  receiver holding register     |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |                      Received data bits 7..0                      |
   -------------------------------------------------------------------
  initial: 0x00

*/

/*--------------------------------+
 |  FCR register                  |
 |  FIFO control register         |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  | RXTRLV1| RXTRLV0|   -   |   -   | DMASEL | TXRST |  RXRST | FIFOEN|
   -------------------------------------------------------------------
  initial: 0x00
*/
 #define MIZ25_FIFOEN			(0x01)		/**< enable FIFO */
 #define MIZ25_RXRST			(0x02)		/**< not implemented */
 #define MIZ25_TXRST			(0x04)		/**< not implemented */
 #define MIZ25_DMASEL			(0x08)		/**< not implemented */
 #define MIZ25_RXTRLV0		    (0x40)		/**< Rx FIFO trigger level low byte */
 #define MIZ25_RXTRLV1          (0x80)		/**< Rx FIFO trigger level high byte */


/*--------------------------------+
 |  IIR register                  |
 |  interrupt status register     |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  | FIFOEN1| FIFOEN0|   -   |   -   |   IID2 |  IID1 |  IID0  |  IRQN |
   -------------------------------------------------------------------
  initial: 0x00
*/
 #define MIZ25_IRQN			    (0x01)		/**< interrupt pending bit */
 #define MIZ25_IID0			    (0x02)		/**< interrupt identification bit0 */
 #define MIZ25_IID1			    (0x04)		/**< interrupt identification bit1 */
 #define MIZ25_IID2			    (0x08)		/**< interrupt identification bit2 */
 #define MIZ25_FIFOEN0		    (0x40)		/**< FIFO status bit0 */
 #define MIZ25_FIFOEN1		    (0x80)		/**< FIFO status bit1 */

/*--------------------------------+
 |  IER register                  |
 |  interrupt enable register     |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |    -   |    -   |   -   |   -   | MSIEN  |RLSIEN |THREIEN |RDAIEN |
   -------------------------------------------------------------------
  initial: 0x00
*/
#define MIZ25_RDAIEN        (0x01)		/**< receiver data available */
#define MIZ25_THREIEN		(0x02)		/**< transmitter holding register status */
#define MIZ25_RLSIEN		(0x04)		/**< receive line status interrupt */
#define MIZ25_MSIEN			(0x08)		/**< modem status interrupt */

/*--------------------------------+
 |  LCR register                  |
 |  line control register         |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |  DLAB  |   SBK  |  SPEN | PTYPE |   PEN  | NOSTP |   WL1  |  WL0  |
   -------------------------------------------------------------------
  initial: 0x00
*/
 #define MIZ25_WL0			    (0x01)		/**< databit bit0 */
 #define MIZ25_WL1			    (0x02)		/**< databit bit1 */
 #define MIZ25_NOSTP			(0x04)		/**< stopbit */
 #define MIZ25_PEN			    (0x08)		/**< parity enable */
 #define MIZ25_PTYPE			(0x10)		/**< parity type */
 #define MIZ25_SPEN			    (0x20)		/**< not implemented */
 #define MIZ25_SBK			    (0x40)		/**< serial break */
 #define MIZ25_DLAB			    (0x80)		/**< divisor latch byte  */

/*--------------------------------+
 |  MCR register                  |
 |  modem control register        |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |    -   |    -   | RCFC  |  LOOP |   OUT2 |  OUT1 |   RTS  |  DTR  |
   -------------------------------------------------------------------
  initial: 0x0f
*/
 #define MIZ25_DTR			(0x01)		/**< data terminal ready */
 #define MIZ25_RTS			(0x02)		/**< RTS line */
 #define MIZ25_OUT1			(0x04)		/**< user out1 line */
 #define MIZ25_OUT2			(0x08)		/**< user out2 line */
 #define MIZ25_LOOP			(0x10)		/**< loopback mode */
 #define MIZ25_RCFC			(0x20)		/**< automatic RTS/CTS handshake (Z125 only) */

/*--------------------------------+
 |  LSR register                  |
 |  line status register          |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |RXFIFOER|   TXEP |  THEP |   BI  |   FE   |   PE  |   OE   |   DR  |
   -------------------------------------------------------------------
  initial: 0x00
*/
 #define MIZ25_DR				(0x01)		/**< Rx FIFO status */
 #define MIZ25_OE				(0x02)		/**< overrun error */
 #define MIZ25_PE				(0x04)		/**< parity error */
 #define MIZ25_FE				(0x08)		/**< framing error */
 #define MIZ25_BI				(0x10)		/**< break indication */
 #define MIZ25_THEP			    (0x20)		/**< transmitter holding status */
 #define MIZ25_TXEP			    (0x40)		/**< transmitt status */
 #define MIZ25_RXFIFOER         (0x80)		/**< Rx fifo data error */

/*--------------------------------+
 |  MSR register                  |
 |  modem status register         |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |   DCD  |    RI  |   DSR |   CTS |   DDCD |   DRI |   DDSR |  DCTS |
   -------------------------------------------------------------------
  initial: 0x00
*/
 #define MIZ25_DCTS			    (0x01)		/**< delta clear to send */
 #define MIZ25_DDSR			    (0x02)		/**< delta data set ready */
 #define MIZ25_DRI			    (0x04)		/**< delta ring indicator */
 #define MIZ25_DDCD			    (0x08)		/**< delta data carrier detect */
 #define MIZ25_CTS			    (0x10)		/**< complement of cts input */
 #define MIZ25_DSR			    (0x20)		/**< complement of dsr input */
 #define MIZ25_RI				(0x40)		/**< complement of ri input */
 #define MIZ25_DCD			    (0x80)		/**< complement of dcd input */

/*--------------------------------+
 |  ACR register                  |
 |  additional control register   |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |   SET  | GETRXD | GETCTS| SETRTS|    HD  |  DIFF | ECHON  |  RXEN |
  | CONFIG |        |       |       |        |       |        | SETTXD|
   -------------------------------------------------------------------
  initial: 0x04
*/
 #define MIZ25_RXEN			    (0x01)		/**< receiver disabled, SETCONFIG=0 */
 #define MIZ25_SETTXD			(0x01)		/**< value TXT on selftest, SETCONFIG=1 */
 #define MIZ25_ECHON			(0x02)		/**< echo mode */
 #define MIZ25_DIFF			    (0x04)		/**< differential mode (RS485) */
 #define MIZ25_HD				(0x08)		/**< full/half duplex */
 #define MIZ25_SETRTS			(0x10)		/**< not implemented */
 #define MIZ25_GETCTS			(0x20)		/**< not implemented */
 #define MIZ25_GETRXD			(0x40)		/**< not implemented */
 #define MIZ25_SETCONFIG		(0x80)		/**< port self test */

/*--------------------------------+
 |  DLL register                  |
 |  divisor latch low byte        |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |                      Divisor low byte 7..0                        |
   -------------------------------------------------------------------
  initial: 0x00

*/

/*--------------------------------+
 |  DLH register                  |
 |  divisor latch high byte       |
 +--------------------------------*/
/*
   -------------------------------------------------------------------
  |  bit7  |  bit6  | bit5  | bit4  |  bit3  | bit2  |  bit1  |  bit0 |
   -------------------------------------------------------------------
  |    -   |    -   |   -   |Divisor|         High byte 4..0          |
   -------------------------------------------------------------------
   initial: 0x00

*/

/*------------------+
 |  TYPEDEFS        |
 +------------------*/
 /** This structure describes a 16Z025_UART unit.
 */
 typedef struct { /* TS_16Z25_UNIT */
	u_int32 address;				/**< uart address */
	u_int32 baudrate;				/**< uart baudrate */
	u_int8  stopbits;				/**< number of stopbits */
	u_int8  databits;				/**< number of databits */
	u_int8  parity;					/**< parity selection */
	u_int8  fifoRxTrigger;			/**< receive fifo bytes */
	u_int8  fifoTxTrigger;			/**< transmit fifo bytes */
	u_int8  lineStatus;             /**< status of data transfer */

	BOOL    dlabSet;				/**< divisior latch byte status */
    BOOL    modemControl;           /**< true=modem control active,
                                       false=modem control disabled */
	BOOL	rts;					/**< rts status */
	BOOL    dtr;					/**< dtr status */
	BOOL    out1;					/**< out1 status */
	BOOL    out2;					/**< out2 status */
	BOOL    cts;					/**< cts status */
	BOOL    dsr;					/**< dsr status */
	BOOL    dcd;					/**< dcd status */

	BOOL    rtsCtsHs;               /**< rts/cts handshake */

    u_int32 uartFreq;               /**< UART clock frequency */
    u_int16 divisorConst;           /**< divisor constant for calculating
                                       the baudrate */
	DBG_HANDLE *dbgHdlP;			/**< debug handle */
} TS_16Z25_UNIT;


#ifdef __cplusplus
}
#endif

#endif/* _MZ25_MODULE_INT_H */



