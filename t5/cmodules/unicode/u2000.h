/* u2000.h */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Copyright (C) 2006-2021 Stuart Swales */

#ifndef __u2000_h
#define __u2000_h

/*
2000..206F General Punctuation
*/

#define UCH_EN_QUAD 0x2000U
#define UCH_EM_QUAD 0x2001U
#define UCH_EN_SPACE 0x2002U
#define UCH_EM_SPACE 0x2003U
#define UCH_THREE_PER_EM_SPACE 0x2004U
#define UCH_FOUR_PER_EM_SPACE 0x2005U
#define UCH_SIX_PER_EM_SPACE 0x2006U
#define UCH_FIGURE_SPACE 0x2007U
#define UCH_PUNCTUATION_SPACE 0x2008U
#define UCH_THIN_SPACE 0x2009U
#define UCH_HAIR_SPACE 0x200AU
#define UCH_ZERO_WIDTH_SPACE 0x200BU
#define UCH_ZERO_WIDTH_NON_JOINER 0x200CU
#define UCH_ZERO_WIDTH_JOINER 0x200DU
#define UCH_LEFT_TO_RIGHT_MARK 0x200EU
#define UCH_RIGHT_TO_LEFT_MARK 0x200FU

#define UCH_HYPHEN 0x2010U
#define UCH_NON_BREAKING_HYPHEN 0x2011U
#define UCH_FIGURE_DASH 0x2012U
#define UCH_EN_DASH 0x2013U
#define UCH_EM_DASH 0x2014U
#define UCH_HORIZONTAL_BAR 0x2015U
#define UCH_DOUBLE_VERTICAL_LINE 0x2016U
#define UCH_DOUBLE_LOW_LINE 0x2017U
#define UCH_LEFT_SINGLE_QUOTATION_MARK 0x2018U
#define UCH_RIGHT_SINGLE_QUOTATION_MARK 0x2019U
#define UCH_SINGLE_LOW_9_QUOTATION_MARK 0x201AU
#define UCH_SINGLE_HIGH_REVERSED_9_QUOTATION_MARK 0x201BU
#define UCH_LEFT_DOUBLE_QUOTATION_MARK 0x201CU
#define UCH_RIGHT_DOUBLE_QUOTATION_MARK 0x201DU
#define UCH_DOUBLE_LOW_9_QUOTATION_MARK 0x201EU
#define UCH_DOUBLE_HIGH_REVERSED_9_QUOTATION_MARK 0x201FU

#define UCH_DAGGER 0x2020U
#define UCH_DOUBLE_DAGGER 0x2021U
#define UCH_BULLET 0x2022U
#define UCH_TRIANGULAR_BULLET 0x2023U
#define UCH_ONE_DOT_LEADER 0x2024U
#define UCH_TWO_DOT_LEADER 0x2025U
#define UCH_HORIZONTAL_ELLIPSIS 0x2026U
#define UCH_HYPHENATION_POINT 0x2027U
#define UCH_LINE_SEPARATOR 0x2028U
#define UCH_PARAGRAPH_SEPARATOR 0x2029U
#define UCH_LEFT_TO_RIGHT_EMBEDDING 0x202AU
#define UCH_RIGHT_TO_LEFT_EMBEDDING 0x202BU
#define UCH_POP_DIRECTIONAL_FORMATTING 0x202CU
#define UCH_LEFT_TO_RIGHT_OVERRIDE 0x202DU
#define UCH_RIGHT_TO_LEFT_OVERRIDE 0x202EU
#define UCH_NARROW_NO_BREAK_SPACE 0x202FU

#define UCH_PER_MILLE_SIGN 0x2030U
#define UCH_PER_TEN_THOUSAND_SIGN 0x2031U
#define UCH_PRIME 0x2032U
#define UCH_DOUBLE_PRIME 0x2033U
#define UCH_TRIPLE_PRIME 0x2034U
#define UCH_REVERSED_PRIME 0x2035U
#define UCH_REVERSED_DOUBLE_PRIME 0x2036U
#define UCH_REVERSED_TRIPLE_PRIME 0x2037U
#define UCH_CARET 0x2038U
#define UCH_SINGLE_LEFT_POINTING_ANGLE_QUOTATION_MARK 0x2039U
#define UCH_SINGLE_RIGHT_POINTING_ANGLE_QUOTATION_MARK 0x203AU
#define UCH_REFERENCE_MARK 0x203BU
#define UCH_DOUBLE_EXCLAMATION_MARK 0x203CU
#define UCH_INTERROBANG 0x203DU
#define UCH_OVERLINE 0x203EU
#define UCH_UNDERTIE 0x203FU

#define UCH_CHARACTER_TIE 0x2040U
#define UCH_CARET_INSERTION_POINT 0x2041U
#define UCH_ASTERISM 0x2042U
#define UCH_HYPHEN_BULLET 0x2043U
#define UCH_FRACTION_SLASH 0x2044U
#define UCH_LEFT_SQUARE_BRACKET_WITH_QUILL 0x2045U
#define UCH_RIGHT_SQUARE_BRACKET_WITH_QUILL 0x2046U
#define UCH_DOUBLE_QUESTION_MARK 0x2047U
#define UCH_QUESTION_EXCLAMATION_MARK 0x2048U
#define UCH_EXCLAMATION_QUESTION_MARK 0x2049U
#define UCH_TIRONIAN_SIGN_ET 0x204AU
#define UCH_REVERSED_PILCROW_SIGN 0x204BU
#define UCH_BLACK_LEFTWARDS_BULLET 0x204CU
#define UCH_BLACK_RIGHTWARDS_BULLET 0x204DU
#define UCH_LOW_ASTERISK 0x204EU
#define UCH_REVERSED_SEMICOLON 0x204FU

#define UCH_CLOSE_UP 0x2050U
#define UCH_TWO_ASTERISKS_ALIGNED_VERTICALLY 0x2051U
#define UCH_COMMERCIAL_MINUS_SIGN 0x2052U
#define UCH_SWUNG_DASH 0x2053U
#define UCH_INVERTED_UNDERTIE 0x2054U
#define UCH_FLOWER_PUNCTUATION_MARK 0x2055U
#define UCH_THREE_DOT_PUNCTUATION 0x2056U
#define UCH_QUADRUPLE_PRIME 0x2057U
#define UCH_FOUR_DOT_PUNCTUATION 0x2058U
#define UCH_FIVE_DOT_PUNCTUATION 0x2059U
#define UCH_TWO_DOT_PUNCTUATION 0x205AU
#define UCH_FOUR_DOT_MARK 0x205BU
#define UCH_DOTTED_CROSS 0x205CU
#define UCH_TRICOLON 0x205DU
#define UCH_VERTICAL_FOUR_DOTS 0x205EU
#define UCH_MEDIUM_MATHEMATICAL_SPACE 0x205FU

#define UCH_WORD_JOINER 0x2060U
#define UCH_FUNCTION_APPLICATION 0x2061U
#define UCH_INVISIBLE_TIMES 0x2062U
#define UCH_INVISIBLE_SEPARATOR 0x2063U
#define UCH_INVISIBLE_PLUS 0x2064U
#define UCH_LEFT_TO_RIGHT_ISOLATE 0x2066U
#define UCH_RIGHT_TO_LEFT_ISOLATE 0x2067U
#define UCH_FIRST_STRONG_ISOLATE 0x2068U
#define UCH_POP_DIRECTIONAL_ISOLATE 0x2069U
#define UCH_INHIBIT_SYMMETRIC_SWAPPING 0x206AU
#define UCH_ACTIVATE_SYMMETRIC_SWAPPING 0x206BU
#define UCH_INHIBIT_ARABIC_FORM_SHAPING 0x206CU
#define UCH_ACTIVATE_ARABIC_FORM_SHAPING 0x206DU
#define UCH_NATIONAL_DIGIT_SHAPES 0x206EU
#define UCH_NOMINAL_DIGIT_SHAPES 0x206FU

#endif /* __u2000_h */

/* end of u2000.h */
