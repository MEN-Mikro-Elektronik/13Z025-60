#***************************  M a k e f i l e  *******************************
#  
#         Author: sv
#          $Date: 2006/02/22 11:42:16 $
#      $Revision: 1.2 $
#  
#    Description: makefile for 13z025 driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: makef.mak,v $
#   Revision 1.2  2006/02/22 11:42:16  ts
#   + additional PATH for 85xx gnu
#
#   Revision 1.1  2005/06/23 08:53:13  SVogel
#   Initial Revision
#
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany 
#*****************************************************************************

#**************************************
#   define used tools
#
include $(WIND_BASE)/target/h/make/make.$(CPU)$(TOOL)
COMPILER=$(CC) -Wall
LIB=$(AR)
FLAGS=$(CC_OPTIM_TARGET) -DCPU=$(CPU)
LIB_FLAG_DEL=-d
LIB_FLAG_ADD=-r
MK=$(GNUMAKE)


ACCESS=-DMAC_MEM_MAPPED

#**************************************
#   additional settings
#
OPT_0=$(DBG)
OPT_1=$(DBG)
OPT_2=$(DBG)
INP_SUFFIX=

#**************************************
#   output directories
#

OBJ_DIR=$(MEN_WORK_DIR)/VXWORKS/LIB/MEN/obj$(CPU)$(TOOL)$(DBGDIR)
MEN_LIB_DIR=$(MEN_WORK_DIR)/VXWORKS/LIB/MEN/lib$(CPU)$(TOOL)$(DBGDIR)

#**************************************
#   input directories
#

MEN_INC_DIR=$(MEN_WORK_DIR)/VXWORKS/INCLUDE

#**************************************
#   include paths
#
INC=-I$(MEN_INC_DIR)/COM     \
    -I$(MEN_INC_DIR)/NATIVE  \
    -I$(WIND_BASE)/target    \
    -I$(WIND_BASE)/target/h  \
    -I.

#additional paths needed for 85xx gnu
ifeq ($(CPU),PPC85XX)
INC+= -I$(WIND_BASE)/target/h/wrn/coreip  \
      -I$(WIND_BASE)/host/gnu/3.3/$(WIND_HOST_TYPE)/lib/gcc-lib/powerpc-wrs-vxworks/3.3-e500/include
endif


#**************************************
#   dependencies and commands
#	
all: $(OBJ_DIR)/mz25_module.o

$(OBJ_DIR)/mz25_module.o:  mz25_module.c 
	$(COMPILER) $(FLAGS)  $(DBG) $(INC) $(DEF) $(ACCESS) -c $< -o $@

all: $(OBJ_DIR)/z25_driver.o     

$(OBJ_DIR)/z25_driver.o: z25_driver.c                   
	$(COMPILER) $(FLAGS)  $(DBG) $(INC) $(DEF) $(ACCESS) -c $< -o $@

all: $(OBJ_DIR)/z25.o 	

FILES =  $(OBJ_DIR)/z25_driver.o \
         $(OBJ_DIR)/mz25_module.o	

$(OBJ_DIR)/z25.o: $(FILES)
	$(LD) -r -o $@ $(FILES)  
	$(LIB)      $(LIB_FLAG_DEL) $(MEN_LIB_DIR)/z25.a $@
	$(LIB)      $(LIB_FLAG_ADD) $(MEN_LIB_DIR)/z25.a $@    
    