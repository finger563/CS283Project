#ifndef MAIN_H
#define MAIN_H

// constants
const int    SCREEN_WIDTH    = 400;
const int    SCREEN_HEIGHT   = 300;
const float  CAMERA_DISTANCE = 2.0f;
const int    TEXT_WIDTH      = 8;
const int    TEXT_HEIGHT     = 13;
const int    IMAGE_WIDTH = 256;
const int    IMAGE_HEIGHT = 256;
const int    CHANNEL_COUNT = 4;
const int    DATA_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * CHANNEL_COUNT;

#define SIZE_X	IMAGE_WIDTH
#define SIZE_Y	IMAGE_HEIGHT

//RGB is 16-bit coded as    G2G1G0B4 B3B2B1B0 R4R3R2R1 R0G5G4G3
#define RGB_MAKE(xR,xG,xB)    ( ( (((xG)>>2)&0x07)<<13 ) + ( (((xG)>>2))>>3 )  +      \
                                ( ((xB)>>3) << 8 )          +      \
                                ( ((xR)>>3) << 3 ) )
#define RED_RGB(xRGB)    ( ( (xRGB >> 3 ) & 0x1F ) * 255.0 / 32.0 )
#define GRN_RGB(xRGB)    ( ( (xRGB >> 13) & 0x07 + ( xRGB & 0x07 ) ) * 255.0 / 32.0 )
#define BLU_RGB(xRGB)    ( ( (xRGB >> 8 ) & 0x1F ) * 255.0 / 32.0 )
                                
#define RGB_RED         0x00F8
#define RGB_BLACK       0x0000
#define RGB_WHITE       0xffff
#define RGB_BLUE        0x1F00
#define RGB_GREEN       0xE007
#define RGB_YELLOW      (RGB_GREEN|RGB_RED)
#define RGB_MAGENTA     (RGB_BLUE|RGB_RED)
#define RGB_LIGHTBLUE   (RGB_BLUE|RGB_GREEN)
#define RGB_ORANGE      (RGB_RED | 0xE001)       // green/2 + red
#define RGB_PINK        (RGB_MAGENTA | 0xE001)   // green/2 + magenta

#endif