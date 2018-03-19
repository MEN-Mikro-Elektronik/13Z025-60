@echo off
REM ***********************************************************************
REM
REM          Author: sv
REM           $Date: 2006/02/22 11:42:44 $
REM       $Revision: 1.2 $
REM
REM     Description: script to make different CPU objects
REM                  calls makefile makef.mak
REM
REM ---------------------------------[ History ]----------------------------
REM
REM    $Log: mk.bat,v $
REM    Revision 1.2  2006/02/22 11:42:44  ts
REM    +PPC85XX
REM
REM    Revision 1.1  2005/06/23 08:53:14  SVogel
REM    Initial Revision
REM
REM    
REM ------------------------------------------------------------------------
REM    (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
REM
REM ************************************************************************
@echo "START"   > out

set WIND_HOST_TYPE=x86-win32
set GNUMAKE=%WIND_BASE%\host\%WIND_HOST_TYPE%\bin\make.exe
set GCC_EXEC_PREFIX=%WIND_BASE%\host\%WIND_HOST_TYPE%\lib\gcc-lib\

set TOOL=gnu
rem set TOOL=GNU
set MEN_WORK_DIR=s:/work

REM ====== NO DBG ==========
set DBG=
set DBGDIR=
REM

rem set CPU=I80386

rem set CPU=I80486

rem set CPU=PENTIUM3

rem set CPU=PENTIUM

rem set CPU=PPC403

rem set CPU=PPC603

rem set CPU=PPC604

rem set CPU=PPC860

set CPU=PPC85XX

rem set CPU=MC68000

%GNUMAKE% -r -f makef.mak   >> out

if errorlevel 1 GOTO FAIL

REM ====== NO DBG ==========
set DBG=
set DBGDIR=
REM


REM ====== DBG ==========
set DBG= -g -O0
set DBGDIR=test
REM

rem set CPU=I80386

rem set CPU=I80486

rem set CPU=PENTIUM

rem set CPU=MC68000

set DBG=-DDBG

rem set CPU=PPC403

rem set CPU=PPC603

set CPU=PPC85XX

rem set CPU=PPC604

rem set CPU=PPC860

%GNUMAKE% -r -f makef.mak   >> out

if errorlevel 1 GOTO FAIL

goto ENDEND

:FAIL
  echo ===========================
  echo "=> ERRORs detected"
  echo ===========================
goto ENDEND


:ENDEND

