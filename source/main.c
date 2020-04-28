#include "Intellisense.h"

//define some basic types
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t		u8;
typedef uint16_t 	u16;
typedef uint32_t 	u32;
typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
//define volatile types
typedef volatile uint8_t	vu8;
typedef volatile uint16_t	vu16;
typedef volatile uint32_t	vu32;
typedef volatile int8_t		vs8;
typedef volatile int16_t	vs16;
typedef volatile int32_t	vs32;


#define REG_DISPCTR *((vu32*)(0x04000000))
#define VIDEOMODE_3 0x0003
#define BG_ENABLE2 0x0400

#define SCREENBUFFER ((vu16*)(0x06000000))
#define SCREEN_W  240
#define SCREEN_H 160

#define INPUT *((vu16*)(0x04000130))

u16 setColor( u8 a_red, u8 a_green, u8 a_blue){
	return (a_red & 0x1F) | (a_green & 0x1F) << 5 | (a_blue & 0x1F) << 10;
}

s32 abs(s32 a){
	s32 mask = a >> 31; //bit shift
	return (a ^ mask) - mask; //absolute using xor and substraction
}

void plotPixel(s32 a_x, s32 a_y, s16 a_color){
	SCREENBUFFER[a_x+a_y*SCREEN_W] = a_color;
}

void plotLine(s32 x1, s32 y1, s32 x2, s32 y2, s16 a_color){
	s32 w = x2-x1;
	s32 h = y2-y1;
	s32 dx1, dx2, dy1, dy2;
	dx1 = dy1 = dx2 = dy2 = 0;
	if(w < 0) dx1 = dx2 = -1; else if (w > 0) dx1 = dx2 = 1;
	if(h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
	s32 longest = abs(w);
	s32 shortest = abs(h);
	if(shortest > longest){
		longest ^= shortest; shortest ^= longest; longest ^= shortest; //swapping values using xors
		if(h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
		dx2 = 0;
	}
	s32 numerator = longest >> 1;
	for(s32 i = 0; i <= longest; ++i){
		plotPixel(x1, y1, a_color);
		numerator += shortest;
		if(numerator > longest){
			numerator -= longest;
			x1 += dx1;
			y1 += dy1;
		}
		else{
			x1 += dx2;
			y1 += dy2;
		}
	}
}

void plotTri(s32 xa, s32 ya, s32 xb, s32 yb, s32 xc, s32 yc, s16 color){
	plotLine(xa, ya, xb, yb, color);
	plotLine(xb, yb, xc, yc, color);
	plotLine(xc, yc, xa, ya, color);
}

#define REG_VCOUNT (*(vu16*)(0x04000006))
void vsync()
{
	while (REG_VCOUNT >= SCREEN_H);
	while (REG_VCOUNT < SCREEN_H);
}

int main()
{
	//set GBA rendering context to MODE 3 Bitmap Rendering
	REG_DISPCTR = VIDEOMODE_3 | BG_ENABLE2;
	u32 x = SCREEN_W>>1;
	u32 y = SCREEN_H>>1;
	u16 color = setColor(16, 0, 0);
	u8 r = 16, g = 0, b = 0;
	while(1){
		vsync();
		plotPixel(x,y,color);
		if( INPUT % 2 == 0 ){
			
		}
		if( (INPUT >> 4) % 2 == 0 && x<SCREEN_W){
			x++;
		}
		if( (INPUT >> 5) % 2 == 0 && x>0){
			x--;
		}
		if( (INPUT >> 6) % 2 == 0 && y>0){
			y--;
		}
		if( (INPUT >> 7) % 2 == 0 && y<SCREEN_H){
			y++;
		}
	}

	return 0;
}

/*s32 t = 0;
	while(1){
		s32 x,y;
		for(x = 0; x < SCREEN_W; ++x){
			for( y = 0; y < SCREEN_H; ++y){
				plotPixel(x, y, setColor((x&y)+t*5, (x&y)+t*3, (x&y)+t));
			}
		}
		++t;
	}*/