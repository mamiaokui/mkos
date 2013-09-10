#ifndef PAINTPACK_H
#define PAINTPACK_H
#define COL000000		0
#define COLFF0000		1
#define COL00FF00		2
#define COLFFFF00		3
#define COL0000FF		4
#define COLFF00FF		5
#define COL00FFFF		6
#define COLFFFFFF		7
#define COLC6C6C6		8
#define COL840000		9
#define COL008400		10
#define COL848400		11
#define COL000084		12
#define COL840084		13
#define COL008484		14
#define COL848484		15
extern unsigned char table_rgb[16 * 3];
void initPalette(void);
void setPalette(int start, int end, unsigned char *rgb);
void drawRect(unsigned char *vram, int screenWidth, unsigned char c, int x0, int y0, int x1, int y1);
void printFont(char *vram, int xsize, int x, int y, char c, char character);
void printString(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void initScreen(char *vram, int width, int height);
void paintBlock(char *vram, int screenWidth, int blockWidth,
                int blockHeight, int paintPositionX, int paintPositionY, char *imageData);
//the mouse cursor size is 16*16
void initMouseCursor(char *mouseBuffer256, char backgroundColor);

#endif
