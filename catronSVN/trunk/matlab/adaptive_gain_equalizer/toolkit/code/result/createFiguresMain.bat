@ECHO off
REM File:		createFiguresMain.bat
REM Purpose:	Modifies .eps files so that text is treated correct.
REM				The output files are copied to correct location
REM	Input:		None
REM Author:		Markus Borgh
REM Date:		2011-Jan-11
REM Copyright:	Limes Audio AB
REM E-mail:		markus.borgh@limesaudio.com

REM This is the output files
set y1=fig1.eps
set y2=fig2.eps
set y3=fig3.eps
set y4=fig4.eps
set y5=fig5.eps
set y6=fig6.eps

REM This is the input files
set x1=result4
set x2=result1a
set x3=result1b
set x4=result2
set x5=sdi
set x6=snrgain

REM Call batch file that creates output file %y#% from input %x#%. Each call has to be made in a new console, otherwise this file will not reach it's end.
@ECHO on
cmd /c convertFigure %x1% %y1%
cmd /c convertFigure %x2% %y2%
cmd /c convertFigure %x3% %y3%
cmd /c convertFigure %x4% %y4%
cmd /c convertFigure %x5% %y5%
cmd /c convertFigure %x6% %y6%
@ECHO off

REM Copy output files to text directory
copy %y1% ..\..\text\%y1%
copy %y2% ..\..\text\%y2%
copy %y3% ..\..\text\%y3%
copy %y4% ..\..\text\%y4%
copy %y5% ..\..\text\%y5%
copy %y6% ..\..\text\%y6%

pause