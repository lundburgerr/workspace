How to flash the ARM application.
http://processors.wiki.ti.com/index.php/Serial_Boot_and_Flash_Loading_Utility_for_OMAP-L138

.net needs to be installd.

Example for the Zoom Logic board

Erase the spi-flash on your board
 From command prompt >sfh_OMAP-L138.exe -p COM2 -erase
 Set board to boot from uart (dip switches S7-7 and s7-8 on)
 reboot and erase begins

build your application in ccs (use release configuration)

Generate a AIS (load/flash-file)
 Start AISgen_d800k008.exe
 Load configuration LogicZoom.cfg
 Select "Arm applicaton File" (the .out-file is in ccs projcet directory /Release)
 Select a output name (.bin)
 Generate AIS

Write your AIS-file to the board 
 From command prompt >sfh_OMAP-L138.exe -flash_noubl -P COM2 c:\ais.bin
 Reboot the board
 The sfh-program proceeds and starts flashing

Set the board dip-switches to boot from flash
 All switched on S7 is off

Reboot and your application is running.

When debugging, make sure to NOT load GEL-files (defined in target configuration) 
and only load symbols (defined in degug configurations).
If you built a release configuraton symbols might be disabled.
