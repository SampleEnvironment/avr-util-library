#/*
 * fontConfig.h
 *
 * Created: 11.05.2022 12:05:19
 *  Author: qfj
 */ 


#ifndef FONTCONFIG_H_
#define FONTCONFIG_H_

#ifdef ili9341

// Please Select Font for HE-Level Display

//#define inconsolata
//#define Lucida_Console
#define Lucida_Console_Alpha_Num
//#define source_code_pro
//#define consolas


#define FONT1_H 16
#define FONT1_W 8

#define FONT2_H 24
#define FONT2_W 12

// High res Fonts
#ifdef Lucida_Console_Alpha_Num
#define FONT3_H 40
#define FONT3_W 34
#define FONT3_W1 29
#endif

#ifdef Lucida_Console
#define FONT3_H 40
#define FONT3_W 27
#endif

#ifdef inconsolata
#define FONT3_H 40
#define FONT3_W 27
#endif

#ifdef consolas
#define FONT3_H 40
#define FONT3_W 30
#endif

#ifdef source_code_pro
#define FONT3_H 40
#define FONT3_W 31
#endif


#endif

#ifdef DISP_3000
#define FONT1_W 6
#define FONT1_H 9


#define FONT2_W 9
#define FONT2_H 15

#define FONT3_W 9
#define FONT3_H 15

//Font Params //TODO use Values that are defined by the Font
#define CHAR_CELL_WIDTH_FONT_1 6
#define HALF_SPACE_WIDTH_FONT_1 3
#define CHAR_CELL_WIDTH_FONT_2 9
#define HALF_SPACE_WIDTH_FONT_2 3

#endif

#endif /* FONTCONFIG_H_ */