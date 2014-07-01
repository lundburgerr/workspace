/******************** Copyright (c) 2012 Catron Elektronik AB ******************
* Filename          : trace.c
* Author            : Björn Lindberg ÅF
* Description       : Defines register address for the AD7242 ic and also Command enumerators.
*                   :
********************************************************************************
* Revision History
* 2013-04-09        : First Issue
*******************************************************************************/
#ifndef ADF7242_H_
#define ADF7242_H_

/****************************************************************************/
/*                   MACRO DEFINITIONS                          */
/****************************************************************************/

/* All Registers */

#define REG_EXT_CTRL   0x100 /* RW External LNA/PA and internal PA control configuration bits */
#define REG_TX_FSK_TEST 0x101 /* RW TX FSK test mode configuration */
#define REG_CCA1       0x105 /* RW RSSI threshold for CCA */
#define REG_CCA2       0x106 /* RW CCA mode configuration */
#define REG_BUFFERCFG  0x107 /* RW RX_BUFFER overwrite control */
#define REG_PKT_CFG    0x108 /* RW FCS evaluation configuration */
#define REG_DELAYCFG0  0x109 /* RW RC_RX command to SFD or sync word search delay */
#define REG_DELAYCFG1  0x10A /* RW RC_TX command to TX state */
#define REG_DELAYCFG2  0x10B /* RW Mac delay extention */
#define REG_SYNC_WORD0 0x10C /* RW sync word bits [7:0] of [23:0]  */
#define REG_SYNC_WORD1 0x10D /* RW sync word bits [15:8] of [23:0]  */
#define REG_SYNC_WORD2 0x10E /* RW sync word bits [23:16] of [23:0]  */
#define REG_SYNC_CONFIG        0x10F /* RW sync word configuration */
#define REG_RC_CFG     0x13E /* RW RX / TX packet configuration */
#define REG_RC_VAR44   0x13F /* RW RESERVED */
#define REG_CH_FREQ0   0x300 /* RW Channel Frequency Settings - Low Byte */
#define REG_CH_FREQ1   0x301 /* RW Channel Frequency Settings - Middle Byte */
#define REG_CH_FREQ2   0x302 /* RW Channel Frequency Settings - 2 MSBs */
#define REG_TX_FD      0x304 /* RW TX Frequency Deviation Register */
#define REG_DM_CFG0    0x305 /* RW RX Discriminator BW Register */
#define REG_TX_M       0x306 /* RW TX Mode Register */
#define REG_RX_M       0x307 /* RW RX Mode Register */
#define REG_RRB                0x30C /* R RSSI Readback Register */
#define REG_LRB                0x30D /* R Link Quality Readback Register */
#define REG_DR0                0x30E /* RW bits [15:8] of [15:0] for data rate setting */
#define REG_DR1                0x30F /* RW bits [7:0] of [15:0] for data rate setting */
#define REG_PRAMPG     0x313 /* RW RESERVED */
#define REG_TXPB       0x314 /* RW TX Packet Storage Base Address */
#define REG_RXPB       0x315 /* RW RX Packet Storage Base Address */
#define REG_TMR_CFG0   0x316 /* RW Wake up Timer Configuration Register - High Byte */
#define REG_TMR_CFG1   0x317 /* RW Wake up Timer Configuration Register - Low Byte */
#define REG_TMR_RLD0   0x318 /* RW Wake up Timer Value Register - High Byte */
#define REG_TMR_RLD1   0x319 /* RW Wake up Timer Value Register - Low Byte */
#define REG_TMR_CTRL   0x31A /* RW Wake up Timer Timeout flag */
#define REG_PD_AUX     0x31E /* RW Battmon enable */
#define REG_GP_CFG     0x32C /* RW GPIO Configuration */
#define REG_GP_OUT     0x32D /* RW GPIO Configuration */
#define REG_GP_IN      0x32E /* R GPIO Configuration */
#define REG_SYNT       0x335 /* RW bandwidth calibration timers */
#define REG_CAL_CFG    0x33D /* RW Calibration Settings */
#define REG_SYNT_CAL   0x371 /* RW Oscillator and Doubler Configuration */
#define REG_IIRF_CFG   0x389 /* RW BB Filter Decimation Rate */
#define REG_CDR_CFG    0x38A /* RW CDR kVCO */
#define REG_DM_CFG1    0x38B /* RW Postdemodulator Filter */
#define REG_AGCSTAT    0x38E /* R RXBB Ref Osc Calibration Engine Readback */
#define REG_RXCAL0     0x395 /* RW RX BB filter tuning, LSB */
#define REG_RXCAL1     0x396 /* RW RX BB filter tuning, MSB */
#define REG_RXFE_CFG   0x39B /* RW RXBB Ref Osc & RXFE Calibration */
#define REG_PA_RR      0x3A7 /* RW Set PA ramp rate */
#define REG_PA_CFG     0x3A8 /* RW PA enable */
#define REG_EXTPA_CFG  0x3A9 /* RW External PA BIAS DAC */
#define REG_EXTPA_MSC  0x3AA /* RW PA Bias Mode */
#define REG_ADC_RBK    0x3AE /* R Readback temp */
#define REG_AGC_CFG1   0x3B2 /* RW GC Parameters */
#define REG_AGC_MAX    0x3B4 /* RW Slew rate  */
#define REG_AGC_CFG2   0x3B6 /* RW RSSI Parameters */
#define REG_AGC_CFG3   0x3B7 /* RW RSSI Parameters */
#define REG_AGC_CFG4   0x3B8 /* RW RSSI Parameters */
#define REG_AGC_CFG5   0x3B9 /* RW RSSI & NDEC Parameters */
#define REG_AGC_CFG6   0x3BA /* RW NDEC Parameters */
#define REG_AGC_CFG7   0x3BC
#define REG_OCL_CFG1   0x3C4 /* RW OCL System Parameters */
#define REG_IRQ1_EN0   0x3C7 /* RW Interrupt Mask set bits  [7:0] of [15:0] for IRQ1 */
#define REG_IRQ1_EN1   0x3C8 /* RW Interrupt Mask set bits  [15:8] of [15:0] for IRQ1 */
#define REG_IRQ2_EN0   0x3C9 /* RW Interrupt Mask set bits  [7:0] of [15:0] for IRQ2 */
#define REG_IRQ2_EN1   0x3CA /* RW Interrupt Mask set bits  [15:8] of [15:0] for IRQ2 */
#define REG_IRQ1_SRC0  0x3CB /* RW Interrupt Source  bits  [7:0] of [15:0] for IRQ */
#define REG_IRQ1_SRC1  0x3CC /* RW Interrupt Source bits  [15:8] of [15:0] for IRQ */
#define REG_OCL_BW0    0x3D2 /* RW OCL System Parameters */
#define REG_OCL_BW1    0x3D3 /* RW OCL System Parameters */
#define REG_OCL_BW2    0x3D4 /* RW OCL System Parameters */
#define REG_OCL_BW3    0x3D5 /* RW OCL System Parameters */
#define REG_OCL_BW4    0x3D6 /* RW OCL System Parameters */
#define REG_OCL_BWS    0x3D7 /* RW OCL System Parameters */
#define REG_OCL_CFG13  0x3E0 /* RW OCL System Parameters */
#define REG_GP_DRV     0x3E3 /* RW I/O pads Configuration and bg trim */
#define REG_BM_CFG     0x3E6 /* RW Battery Monitor Threshold Voltage setting */
#define REG_SFD_15_4   0x3F4 /* RW Option to set non standard SFD */
#define REG_AFC_CFG    0x3F7 /* RW AFC mode and polarity */
#define REG_AFC_KI_KP  0x3F8 /* RW AFC ki and kp */
#define REG_AFC_RANGE  0x3F9 /* RW AFC range */
#define REG_AFC_READ   0x3FA /* RW Readback frequency error */
#define REG_NUMBER_PREAMBL  0x102
#define REG_AGC_CFG7   0x3BC
#define REG_OCL_CFG0   0x3BF
#define REG_PREAMBL_VALID   0x3F3

#define CMD_SPI_NOP            0xFF /* No operation. Use for dummy writes */
#define CMD_SPI_PKT_WR         0x10 /* Write telegram to the Packet RAM starting from the TX packet base address pointer tx_packet_base */
#define CMD_SPI_PKT_RD         0x30 /* Read telegram from the Packet RAM starting from RX packet base address pointer rxpb.rx_packet_base */
#define CMD_SPI_MEM_WR(x)      (0x18 + (x >> 8)) /* Write data to MCR or Packet RAM sequentially */
#define CMD_SPI_MEM_RD(x)      (0x38 + (x >> 8)) /* Read data from MCR or Packet RAM sequentially */
#define CMD_SPI_MEMR_WR(x)     (0x08 + (x >> 8)) /* Write data to MCR or Packet RAM as random block */
#define CMD_SPI_MEMR_RD(x)     (0x28 + (x >> 8)) /* Read data from MCR or Packet RAM as random block */
#define CMD_SPI_PRAM_WR                0x1E /* Write data sequentially to current PRAM page selected */
#define CMD_RC_SLEEP           0xB1 /* Invoke transition of radio controller into SLEEP state */
#define CMD_RC_IDLE            0xB2 /* Invoke transition of radio controller into IDLE state */
#define CMD_RC_PHY_RDY         0xB3 /* Invoke transition of radio controller into PHY_RDY state */
#define CMD_RC_RX              0xB4 /* Invoke transition of radio controller into RX state */
#define CMD_RC_TX              0xB5 /* Invoke transition of radio controller into TX state */
#define CMD_RC_MEAS            0xB6 /* Invoke transition of radio controller into MEAS state */
#define CMD_RC_CCA             0xB7 /* Invoke Clear channel assessment */
#define CMD_RC_CSMACA          0xC1 /* initiates CSMA-CA channel access sequence and frame transmission */



#endif /* ADF7242_H_ */
