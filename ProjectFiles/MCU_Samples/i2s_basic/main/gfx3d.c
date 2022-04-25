/* The MIT License (MIT)

Copyright (c) 2016 CNLohr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// HN Modified version of 3d.c from https://github.com/cnlohr/channel3	
// for a controller-less monochrome 240x160 lcd with 4bit databus, hsync & vsync
// using espressif ESP32 I2S parallel 8-bit interface. Also added bitmap font drawing

#include "common.h"
#include "gfx3d.h"
#include "font_6x8.h"

// I2S peripheral has 32bit internal bus. Unfortunately, there is a byte alignment quirk where in 8bit parallel mode,
// the bytes have to be sent to the I2S peripheral in 2,3,0,1 order if you want them to appear at the bus interface in 0,1,2,3 order
int bytePosLkp[4] = {2,3,0,1}; 

void CNFGClearScreen(uint8_t pattern, uint8_t* buffer) {
	for (int row = 0; row < NUM_ROWS; row++) {
		uint8_t d4;
		int col;
		for (col = 0; col < NUM_COLS; col++) {
			if(pattern == 0xff){
				if(col%2 == 1){
					d4 = pattern | BIT_HS | BIT_VS;
				} else {
					d4 = (~pattern) | BIT_HS | BIT_VS;
				}
			} else {
				d4 = pattern | BIT_HS | BIT_VS;
			}
			if(col < HS_WIDTH){
				d4 = (d4 & (~BIT_HS)); // encode hsync
			}
			if(row < VS_WIDTH){
				d4 = (d4 & (~BIT_VS)); //encode vsync
			}
			if((row >= NUM_ROWS_FRONT) && (row < NUM_ROWS - NUM_ROWS_BACK) && (col >= NUM_COLS_FRONT) && (col < NUM_COLS - NUM_COLS_BACK)){
				d4 = (d4 | BIT_DE); //encode de signals
			}
			else{
				d4 = (d4 & (~BIT_DE) & NO_OUT_MASK); //data to 0 on porches for vga problem from 385
			}
			buffer[(row*NUM_COLS)+col] = d4;
		} 
	}
}

void CNFGDrawPixel(int x, int y, bool level, uint8_t* buffer){
	int col = x + NUM_ROWS_FRONT;
	int row = y + NUM_COLS_FRONT;
	if(level){
		buffer[(row*NUM_COLS)+col] |= DATA_MASK;
	} else{
		buffer[(row*NUM_COLS)+col] &= (~DATA_MASK);
	}
}

//x, y = top left corner starting
//level = 1 if character is white with black background, 0 for reverse
void CNFGDrawText(int char_idx, int x, int y, bool level, uint8_t* buffer){
	bool pixel_level;
	for(int row = 0; row < FONT_HEIGHT; row++){
		for(int col = 0; col < FONT_WIDTH; col++){
			if((Font6x8[(char_idx*FONT_HEIGHT)+col] & (0x01 << row)) != 0x00){
				pixel_level = level;
			} else {
				pixel_level = !level;
			}
			CNFGDrawPixel(x+col, y+row, pixel_level, buffer);
		}
	}
}

/*
int CNFGPenX, CNFGPenY;

void  CNFGDrawText( const char * text, int scale ){
	const unsigned char * lmap;
	int16_t iox = (int16_t)CNFGPenX;
	int16_t ioy = (int16_t)CNFGPenY;

	int place = 0;
	unsigned short index;
	int bQuit = 0;
	while( text[place] ){
		unsigned char c = text[place];

		switch( c )	{
		case 9:
			iox += 12 * scale;
			break;
		case 10:
			iox = (int16_t)CNFGPenX;
			ioy += 6 * scale;
			break;
		default:
			index = FontCharMap[c&0x7f];
			if( index == 65535 ){
				iox += 3 * scale;
				break;
				}

			lmap = &FontCharData[index];
			do	{
				int x1 = (int)((((*lmap) & 0x70)>>4)*scale + iox);
				int y1 = (int)(((*lmap) & 0x0f)*scale + ioy);
				int x2 = (int)((((*(lmap+1)) & 0x70)>>4)*scale + iox);
				int y2 = (int)(((*(lmap+1)) & 0x0f)*scale + ioy);
				lmap++;
				CNFGTackSegment( x1, y1, x2, y2 );
				bQuit = *lmap & 0x80;
				lmap++;
			} while( !bQuit );

			iox += 3 * scale;
			}
			
		place++;
		}
	}
*/
