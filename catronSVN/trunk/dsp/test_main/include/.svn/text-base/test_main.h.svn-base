/*****************************************************************************
 *	\file		.h
 *
 *	\date		2013-May-15
 *
 *	\brief		Header for 
 *
 *	Copyright	Limes Audio AB
 *
 *	This code should not be modified, copied, duplicated, reproduced, licensed
 *	or sublicensed without the prior written consent of Limes Audio AB.
 *	This code (or any right in the code) should not be transferred or conveyed
 *	without the prior written consent of Limes Audio AB.
 *
 *****************************************************************************/
#ifndef TEST_MAIN
#define TEST_MAIN

// ---< Include >---
#include "true_voice.h"

// ---< Defines >---
#define N_RX 15
#define N_MIC 5
#define N_TX 8
#define N_LS 6

// ---< Composites declaration >---

// ---< Function prototypes >---
static void config_true_voice_test(void);
static void display_version_number(void);
static void init_audio_signals_test(void);
void sin_block(short [], const unsigned int, unsigned int *);
void create_PESQ_files(void);
void test_mixer(void);
void test_pilot_connections(void);
void test_dtmf(void);
void test_noise_reduction(void);
void test_fileInput(void);
void test_subband(void);
void test_lec(void);
void test_all_lec(void);
void test_audio_loop(void);
void test_mute(void);
void test_main(void);

// ---< External variables >---
extern short audioRx[N_RX][BUFLEN_8KHZ];
extern short audioMic[N_MIC][BUFLEN_8KHZ];
extern short audioTx[N_TX][BUFLEN_8KHZ];
extern short audioLs[N_LS][BUFLEN_8KHZ];

extern tvRxChannels_t rx;
extern tvMicChannels_t mic;
extern tvTxChannels_t tx;
extern tvSpeakerChannels_t ls;

#endif // TEST_MAIN
