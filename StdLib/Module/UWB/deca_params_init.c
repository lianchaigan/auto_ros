/*****************************************************************************
 @Project	: 
 @File 		: deca_params_init.c
 @Details  	: DW1000 configuration parameters
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "deca_device_api.h"
#include "deca_param_types.h"
#include "deca_regs.h"

/*****************************************************************************
 Define
******************************************************************************/

/*****************************************************************************
 Type definition
******************************************************************************/

/*****************************************************************************
 Const Local Variables
******************************************************************************/

/* map the channel number to the index in the configuration arrays below
0th element is chan 1, 1st is chan 2, 2nd is chan 3, 3rd is chan 4, 4th 
is chan 5, 5th is chan 7 */
uint8 const chan_idx[NUM_CH_SUPPORTED] = { 0, 0, 1, 2, 3, 4, 0, 5 };

uint32 const tx_config[NUM_CH] =
    {
        RF_TXCTRL_CH1,
        RF_TXCTRL_CH2,
        RF_TXCTRL_CH3,
        RF_TXCTRL_CH4,
        RF_TXCTRL_CH5,
        RF_TXCTRL_CH7,
    };

/* Frequency Synthesiser - PLL configuration */
uint32 const fs_pll_cfg[NUM_CH] =
    {
        FS_PLLCFG_CH1,
        FS_PLLCFG_CH2,
        FS_PLLCFG_CH3,
        FS_PLLCFG_CH4,
        FS_PLLCFG_CH5,
        FS_PLLCFG_CH7
    };

/* Frequency Synthesiser - PLL tuning */
uint8 const fs_pll_tune[NUM_CH] =
    {
        FS_PLLTUNE_CH1,
        FS_PLLTUNE_CH2,
        FS_PLLTUNE_CH3,
        FS_PLLTUNE_CH4,
        FS_PLLTUNE_CH5,
        FS_PLLTUNE_CH7
    };

/* bandwidth configuration */
uint8 const rx_config[NUM_BW] =
    {
        RF_RXCTRLH_NBW,
        RF_RXCTRLH_WBW
    };

agc_cfg_struct const agc_config =
    {
        AGC_TUNE2_VAL,
        { AGC_TUNE1_16M, AGC_TUNE1_64M } /* adc target */
    };

/* DW non-standard SFD length for 110k, 850k and 6.81M */
uint8 const dwnsSFDlen[NUM_BR] =
    {
        DW_NS_SFD_LEN_110K,
        DW_NS_SFD_LEN_850K,
        DW_NS_SFD_LEN_6M8
    };

/* SFD Threshold */
uint16 const sftsh[NUM_BR][NUM_SFD] =
    {
        { DRX_TUNE0b_110K_STD,
            DRX_TUNE0b_110K_NSTD },

        { DRX_TUNE0b_850K_STD,
            DRX_TUNE0b_850K_NSTD },

        { DRX_TUNE0b_6M8_STD,
            DRX_TUNE0b_6M8_NSTD }
    };

uint16 const dtune1[NUM_PRF] =
    {
        DRX_TUNE1a_PRF16,
        DRX_TUNE1a_PRF64
    };

uint32 const digital_bb_config[NUM_PRF][NUM_PACS] =
    {
        { DRX_TUNE2_PRF16_PAC8,
            DRX_TUNE2_PRF16_PAC16,
            DRX_TUNE2_PRF16_PAC32,
            DRX_TUNE2_PRF16_PAC64 },
        { DRX_TUNE2_PRF64_PAC8,
            DRX_TUNE2_PRF64_PAC16,
            DRX_TUNE2_PRF64_PAC32,
            DRX_TUNE2_PRF64_PAC64 }
    };

uint16 const lde_replicaCoeff[PCODES] =
    {
        0, // No preamble code 0
        LDE_REPC_PCODE_1,
        LDE_REPC_PCODE_2,
        LDE_REPC_PCODE_3,
        LDE_REPC_PCODE_4,
        LDE_REPC_PCODE_5,
        LDE_REPC_PCODE_6,
        LDE_REPC_PCODE_7,
        LDE_REPC_PCODE_8,
        LDE_REPC_PCODE_9,
        LDE_REPC_PCODE_10,
        LDE_REPC_PCODE_11,
        LDE_REPC_PCODE_12,
        LDE_REPC_PCODE_13,
        LDE_REPC_PCODE_14,
        LDE_REPC_PCODE_15,
        LDE_REPC_PCODE_16,
        LDE_REPC_PCODE_17,
        LDE_REPC_PCODE_18,
        LDE_REPC_PCODE_19,
        LDE_REPC_PCODE_20,
        LDE_REPC_PCODE_21,
        LDE_REPC_PCODE_22,
        LDE_REPC_PCODE_23,
        LDE_REPC_PCODE_24
    };

double const txpwr_compensation[NUM_CH] =
    {
        0.0,
        0.035,
        0.0,
        0.0,
        0.065,
        0.0
    };

/*****************************************************************************
 Global Variables
******************************************************************************/

/*****************************************************************************
 Local Variables
******************************************************************************/

/*****************************************************************************
 Local Functions
******************************************************************************/

/*****************************************************************************
 Implementation
******************************************************************************/