/****************************************************************************
* Prebuilt binaries of the ROM enhancement patch to be applied to the ECC correction function in device ROM
* 
****************************************************************************/
The patch provided in this package is a ROM enhancement patch for NAND boot mode on AM18x/OMAPL138/C6748 devices. For futher details refer to user notes documentation of the ECC data errors causing NAND boot .

The folder contains the following files

* ARM_nand_ecc_patch_OMAP-L138.out: This binary patch for ARM boot OMAPL138 devices
* DSP_nand_ecc_patch_OMAP-L138.out: This binary patch for DSP boot C6748 devices 
   Note: DSP side patch would also apply to secure boot OMAPL138 devices.
* HexAIS_OMAPL138.exe: Command line AIS gen utility that can be used with the INI files to apply the patch to the boot images.
* INI files: This folder contains sample ini files that were used for testing the patch on OMAPL138 and C6748 EVMs and LCDK platforms.

Instructions to apply the patch:

Copy the patched binary, application image, HexAISGen tool and the ini file into a folder 

Change the directory to the above directory on wndows command line and  execute the following to apply the patch to ARM boot devices

./HexAIS_OMAP-L138.exe -ini OMAP-L138_ARM_nand_ecc_patch_<platform>.ini  -o <boot_image_name>.bin <UserApp_name>.out

or execute the following to apply the patch to DSP boot devices

./HexAIS_OMAP-L138.exe -ini OMAP-L138_ARM_nand_ecc_patch_<platform>.ini -o <boot_image_name>.bin <UserApp_name>.out

