/********************************************
*                 INI files
*
*********************************************/

Folder contains 4 sample INI files for the various platforms on which the patch is validated. The INI files remain the same across different PG version of the device.

* OMAP-L138_ARM_nand_ecc_patch_evm.ini
* OMAP-L138_ARM_nand_ecc_patch_lcdk.ini
* OMAP-L138_DSP_nand_ecc_patch_evm.ini
* OMAP-L138_DSP_nand_ecc_patch_lcdk.ini

Primary difference:

OMAPL138/C6748 LCDK has 16 bit NAND and DDR2 memory
OMAPL138/C6748 EVM has 8 bit NAND  and mDDR memory


To apply the patch the following lines are added to the INI files.

; Get the NAND ECC patch for the ARM
[INPUTFILE]
FILENAME=DSP_nand_ecc_patch_OMAP-L138.out

; Alter the hEccInfo pointer in the NAND_InfoObj structure
; to patch the NAND ECC handling routine
[AIS_Jump]
LOCATION=_NAND_ECC_patchApply

The symbol _NAND_ECC_patchApply is defined in the source of the patch.