/* u0080.h */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Copyright (C) 2006-2015 Stuart Swales */

#ifndef __u0080_h
#define __u0080_h

/*
0080..00FF Latin-1 Supplement
*/

/*
0080..009F C1 Controls
*/

/*
00A0..00FF ISO 8859-1
*/

#define UCH_NO_BREAK_SPACE 0x00A0U
#define UCH_INVERTED_EXCLAMATION_MARK 0x00A1U
#define UCH_CENT_SIGN 0x00A2U
#define UCH_POUND_SIGN 0x00A3U
#define UCH_CURRENCY_SIGN 0x00A4U
#define UCH_YEN_SIGN 0x00A5U
#define UCH_BROKEN_BAR 0x00A6U
#define UCH_SECTION_SIGN 0x00A7U
#define UCH_DIAERESIS 0x00A8U
#define UCH_COPYRIGHT_SIGN 0x00A9U
#define UCH_FEMININE_ORDINAL_INDICATOR 0x00AAU
#define UCH_LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK 0x00ABU
#define UCH_NOT_SIGN 0x00ACU
#define UCH_SOFT_HYPHEN 0x00ADU
#define UCH_REGISTERED_SIGN 0x00AEU
#define UCH_MACRON 0x00AFU

#define UCH_DEGREE_SIGN 0x00B0U
#define UCH_PLUS_MINUS_SIGN 0x00B1U
#define UCH_SUPERSCRIPT_TWO 0x00B2U
#define UCH_SUPERSCRIPT_THREE 0x00B3U
#define UCH_ACUTE_ACCENT 0x00B4U
#define UCH_MICRO_SIGN 0x00B5U
#define UCH_PILCROW_SIGN 0x00B6U
#define UCH_MIDDLE_DOT 0x00B7U
#define UCH_CEDILLA 0x00B8U
#define UCH_SUPERSCRIPT_ONE 0x00B9U
#define UCH_MASCULINE_ORDINAL_INDICATOR 0x00BAU
#define UCH_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK 0x00BBU
#define UCH_VULGAR_FRACTION_ONE_QUARTER 0x00BCU
#define UCH_VULGAR_FRACTION_ONE_HALF 0x00BDU
#define UCH_VULGAR_FRACTION_THREE_QUARTERS 0x00BEU
#define UCH_INVERTED_QUESTION_MARK 0x00BFU

#define UCH_LATIN_CAPITAL_LETTER_A_WITH_GRAVE 0x00C0U
#define UCH_LATIN_CAPITAL_LETTER_A_WITH_ACUTE 0x00C1U
#define UCH_LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX 0x00C2U
#define UCH_LATIN_CAPITAL_LETTER_A_WITH_TILDE 0x00C3U
#define UCH_LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS 0x00C4U
#define UCH_LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE 0x00C5U
#define UCH_LATIN_CAPITAL_LETTER_AE 0x00C6U
#define UCH_LATIN_CAPITAL_LETTER_C_WITH_CEDILLA 0x00C7U
#define UCH_LATIN_CAPITAL_LETTER_E_WITH_GRAVE 0x00C8U
#define UCH_LATIN_CAPITAL_LETTER_E_WITH_ACUTE 0x00C9U
#define UCH_LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX 0x00CAU
#define UCH_LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS 0x00CBU
#define UCH_LATIN_CAPITAL_LETTER_I_WITH_GRAVE 0x00CCU
#define UCH_LATIN_CAPITAL_LETTER_I_WITH_ACUTE 0x00CDU
#define UCH_LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX 0x00CEU
#define UCH_LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS 0x00CFU

#define UCH_LATIN_CAPITAL_LETTER_ETH 0x00D0U
#define UCH_LATIN_CAPITAL_LETTER_N_WITH_TILDE 0x00D1U
#define UCH_LATIN_CAPITAL_LETTER_O_WITH_GRAVE 0x00D2U
#define UCH_LATIN_CAPITAL_LETTER_O_WITH_ACUTE 0x00D3U
#define UCH_LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX 0x00D4U
#define UCH_LATIN_CAPITAL_LETTER_O_WITH_TILDE 0x00D5U
#define UCH_LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS 0x00D6U
#define UCH_MULTIPLICATION_SIGN 0x00D7U
#define UCH_LATIN_CAPITAL_LETTER_O_WITH_STROKE 0x00D8U
#define UCH_LATIN_CAPITAL_LETTER_U_WITH_GRAVE 0x00D9U
#define UCH_LATIN_CAPITAL_LETTER_U_WITH_ACUTE 0x00DAU
#define UCH_LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX 0x00DBU
#define UCH_LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS 0x00DCU
#define UCH_LATIN_CAPITAL_LETTER_Y_WITH_ACUTE 0x00DDU
#define UCH_LATIN_CAPITAL_LETTER_THORN 0x00DEU
#define UCH_LATIN_SMALL_LETTER_SHARP_S 0x00DFU

#define UCH_LATIN_SMALL_LETTER_A_WITH_GRAVE 0x00E0U
#define UCH_LATIN_SMALL_LETTER_A_WITH_ACUTE 0x00E1U
#define UCH_LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX 0x00E2U
#define UCH_LATIN_SMALL_LETTER_A_WITH_TILDE 0x00E3U
#define UCH_LATIN_SMALL_LETTER_A_WITH_DIAERESIS 0x00E4U
#define UCH_LATIN_SMALL_LETTER_A_WITH_RING_ABOVE 0x00E5U
#define UCH_LATIN_SMALL_LETTER_AE 0x00E6U
#define UCH_LATIN_SMALL_LETTER_C_WITH_CEDILLA 0x00E7U
#define UCH_LATIN_SMALL_LETTER_E_WITH_GRAVE 0x00E8U
#define UCH_LATIN_SMALL_LETTER_E_WITH_ACUTE 0x00E9U
#define UCH_LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX 0x00EAU
#define UCH_LATIN_SMALL_LETTER_E_WITH_DIAERESIS 0x00EBU
#define UCH_LATIN_SMALL_LETTER_I_WITH_GRAVE 0x00ECU
#define UCH_LATIN_SMALL_LETTER_I_WITH_ACUTE 0x00EDU
#define UCH_LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX 0x00EEU
#define UCH_LATIN_SMALL_LETTER_I_WITH_DIAERESIS 0x00EFU

#define UCH_LATIN_SMALL_LETTER_ETH 0x00F0U
#define UCH_LATIN_SMALL_LETTER_N_WITH_TILDE 0x00F1U
#define UCH_LATIN_SMALL_LETTER_O_WITH_GRAVE 0x00F2U
#define UCH_LATIN_SMALL_LETTER_O_WITH_ACUTE 0x00F3U
#define UCH_LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX 0x00F4U
#define UCH_LATIN_SMALL_LETTER_O_WITH_TILDE 0x00F5U
#define UCH_LATIN_SMALL_LETTER_O_WITH_DIAERESIS 0x00F6U
#define UCH_DIVISION_SIGN 0x00F7U
#define UCH_LATIN_SMALL_LETTER_O_WITH_STROKE 0x00F8U
#define UCH_LATIN_SMALL_LETTER_U_WITH_GRAVE 0x00F9U
#define UCH_LATIN_SMALL_LETTER_U_WITH_ACUTE 0x00FAU
#define UCH_LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX 0x00FBU
#define UCH_LATIN_SMALL_LETTER_U_WITH_DIAERESIS 0x00FCU
#define UCH_LATIN_SMALL_LETTER_Y_WITH_ACUTE 0x00FDU
#define UCH_LATIN_SMALL_LETTER_THORN 0x00FEU
#define UCH_LATIN_SMALL_LETTER_Y_WITH_DIAERESIS 0x00FFU

#endif /* __u0080_h */

/* end of u0080.h */
