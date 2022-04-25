#ifndef _3D_H_
#define _3D_H_

// graphics for controller-less monochrome LCD display with 4bit data bus,
// hsync, vsync and fr signals.  The fr signal is generated externally
// in hw from the vsync signal. So we need to encode the 4bit data + hsync + vsync
// on a 6bit wide parallel bus to the LCD. For this we use the ESP32 i2s bus
// in parallel LCD 8bit mode.

#define NO_OUT_MASK  ((uint8_t)0xf8)
#define DATA_MASK    (0x01)
#define BIT_DE ((uint8_t)(1<<3))
#define BIT_HS	((uint8_t)(1<<4))
#define BIT_VS	((uint8_t)(1<<5))

#define NUM_COLS 		   408
#define NUM_COLS_FRONT  68
#define NUM_COLS_BACK   20
#define HS_WIDTH        2
#define NUM_ROWS 		   262
#define NUM_ROWS_FRONT  18
#define NUM_ROWS_BACK   4
#define VS_WIDTH        2

#define FRAME_SIZE (NUM_COLS * NUM_ROWS)

void CNFGClearScreen(uint8_t pattern, uint8_t* buffer);
void CNFGDrawPixel(int x, int y, bool level, uint8_t* buffer);
void CNFGDrawText(int char_idx, int x, int y, bool level, uint8_t* buffer);

#endif



