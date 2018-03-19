#***************************  M a k e f i l e  *******************************
#  
#         Author: sv
#          $Date: 2008/06/09 17:15:58 $
#      $Revision: 1.2 $
#  
#    Description: MDIS makefile for 13z025 driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_sw.mak,v $
#   Revision 1.2  2008/06/09 17:15:58  SVogel
#   R:1. no possibiltiy to switch between chameleon and PCI interrupt
#   M:1. added compiler switch USE_PCI_IRQ
#
#   Revision 1.1  2005/06/23 08:53:12  SVogel
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany 
#*****************************************************************************
MAK_NAME=z25_sw

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
           $(SW_PREFIX)MAC_BYTESWAP \
           $(SW_PREFIX)USE_PCI_IRQ \
           $(SW_PREFIX)ID_SW \
           $(SW_PREFIX)PLD_SW \
           $(SW_PREFIX)Z25_MdisDriver=Z25_MdisDriver_sw \
           $(SW_PREFIX)Z25_SW \

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id_sw$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	\

MAK_INCL=

MAK_INP1=z25_mdis$(INP_SUFFIX)
MAK_INP2=z25_driver$(INP_SUFFIX)
MAK_INP3=mz25_module$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3)


