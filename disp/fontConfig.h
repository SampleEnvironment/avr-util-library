#/*
 * fontConfig.h
 *
 * Created: 11.05.2022 12:05:19
 *  Author: qfj
 */ 


#ifndef FONTCONFIG_H_
#define FONTCONFIG_H_

#ifdef ili9341


#define FONT1_H 16
#define FONT1_W 8

#define FONT2_H 24
#define FONT2_W 12

// High res Font

#define FONT3_H 40
#define FONT3_W 34
#define FONT3_W1 29



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