#define REG_DISPCTL *(unsigned short *)0x4000000 // Display Control Register

#define BG2_ENABLE (1<<10)
#define MODE3 3

#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006

#define OFFSET(r, c, rowlen) ((r)*(rowlen)+(c))

#define COLOR(r,g,b)  ((r) | (g)<<5 | (b) << 10)
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define CYAN COLOR(0,31,31)
#define MAGENTA COLOR(31,0,31)
#define YELLOW COLOR(31,31,0)
#define WHITE COLOR(31,31,31)
#define GREY COLOR(15,15,15)
#define BLACK 0

// Buttons
// Masks, one for each button
#define BUTTON_A		(1<<0)
#define BUTTON_B		(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R		(1<<8)
#define BUTTON_L		(1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)
#define BUTTON_PRESSED(key) (!(~(oldButtons)&(key)) && (~(buttons) & (key)))

#define BUTTONS *(volatile unsigned int *)0x4000130

extern unsigned short *videoBuffer;

/* DMA */

#define REG_DMA0SAD         *(const volatile u32*)0x40000B0 // source address
#define REG_DMA0DAD         *(volatile u32*)0x40000B4       // destination address
#define REG_DMA0CNT         *(volatile u32*)0x40000B8       // control register

// DMA channel 1 register definitions
#define REG_DMA1SAD         *(const volatile u32*)0x40000BC // source address
#define REG_DMA1DAD         *(volatile u32*)0x40000C0       // destination address
#define REG_DMA1CNT         *(volatile u32*)0x40000C4       // control register

// DMA channel 2 register definitions
#define REG_DMA2SAD         *(const volatile u32*)0x40000C8 // source address
#define REG_DMA2DAD         *(volatile u32*)0x40000CC       // destination address
#define REG_DMA2CNT         *(volatile u32*)0x40000D0       // control register

// DMA channel 3 register definitions
#define REG_DMA3SAD         *(const volatile u32*)0x40000D4 // source address
#define REG_DMA3DAD         *(volatile u32*)0x40000D8       // destination address
#define REG_DMA3CNT         *(volatile u32*)0x40000DC       // control register

typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

//image stuff
#define GRIT_TEST4_H
#define GRIT_TEST4_H

typedef struct POINTER
{
	int position;
	int oldPosition;
	int value; //the type/number it is pointing at
}POINTER;

typedef struct STACK
{
	unsigned int s[16];
	int base;
}STACK;

typedef unsigned short u16;

// Prototypes
void setPixel(int, int, unsigned short);
void drawRect(int, int, int, int, unsigned short);
void waitForVblank();
void drawTriangle(int, int, int, int,  unsigned short);
void drawHeart(int, int, unsigned short);
void fillScreen(unsigned short);
void drawbgImage(const unsigned short *);
int getValue(unsigned int *, int);
void changeBase(unsigned int *, int, int);
void toArray(int, unsigned int *, int);
void calculate(unsigned int *, int, unsigned int *, int);