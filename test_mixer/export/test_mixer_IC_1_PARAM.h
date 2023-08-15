/*
 * File:           C:\GitHub\adau_teensy_software\teensy_rotary_redux_mono\test_mixer\export\test_mixer_IC_1_PARAM.h
 *
 * Created:        Saturday, April 15, 2023 3:20:02 PM
 * Description:    test_mixer:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2023 Analog Devices, Inc. All rights reserved.
 */
#ifndef __TEST_MIXER_IC_1_PARAM_H__
#define __TEST_MIXER_IC_1_PARAM_H__


/* Module Gain_wet_out - Gain*/
#define MOD_GAIN_WET_OUT_COUNT                         1
#define MOD_GAIN_WET_OUT_DEVICE                        "IC1"
#define MOD_GAIN_WET_OUT_GAIN1940ALGNS1_ADDR           0
#define MOD_GAIN_WET_OUT_GAIN1940ALGNS1_FIXPT          0x00FF64C1
#define MOD_GAIN_WET_OUT_GAIN1940ALGNS1_VALUE          SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.99526231496888)
#define MOD_GAIN_WET_OUT_GAIN1940ALGNS1_TYPE           SIGMASTUDIOTYPE_FIXPOINT

/* Module Gain_wet_in - Gain*/
#define MOD_GAIN_WET_IN_COUNT                          1
#define MOD_GAIN_WET_IN_DEVICE                         "IC1"
#define MOD_GAIN_WET_IN_GAIN1940ALGNS3_ADDR            1
#define MOD_GAIN_WET_IN_GAIN1940ALGNS3_FIXPT           0x004026E7
#define MOD_GAIN_WET_IN_GAIN1940ALGNS3_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.501187233627272)
#define MOD_GAIN_WET_IN_GAIN1940ALGNS3_TYPE            SIGMASTUDIOTYPE_FIXPOINT

/* Module 2XMixer1 - Cross Mixer (2 Inputs)*/
#define MOD_2XMIXER1_COUNT                             2
#define MOD_2XMIXER1_DEVICE                            "IC1"
#define MOD_2XMIXER1_ALG0_STAGE0_VOLONE_ADDR           2
#define MOD_2XMIXER1_ALG0_STAGE0_VOLONE_FIXPT          0x004026E7
#define MOD_2XMIXER1_ALG0_STAGE0_VOLONE_VALUE          SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.501187233627272)
#define MOD_2XMIXER1_ALG0_STAGE0_VOLONE_TYPE           SIGMASTUDIOTYPE_FIXPOINT
#define MOD_2XMIXER1_ALG0_STAGE0_VOLTWO_ADDR           3
#define MOD_2XMIXER1_ALG0_STAGE0_VOLTWO_FIXPT          0x004026E7
#define MOD_2XMIXER1_ALG0_STAGE0_VOLTWO_VALUE          SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.501187233627272)
#define MOD_2XMIXER1_ALG0_STAGE0_VOLTWO_TYPE           SIGMASTUDIOTYPE_FIXPOINT

#endif
