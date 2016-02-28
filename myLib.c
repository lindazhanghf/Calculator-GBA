#include <math.h>
#include "myLib.h"


unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int r, int c, unsigned short color)
{
	videoBuffer[OFFSET(r, c, 240)] = color;
}

void DMANow(int channel, volatile const void* source, volatile const void* destination, unsigned int control)
{
    DMA[channel].src = source;
    DMA[channel].dst = destination; //videoBuffer[OFFSET(row+r, col, 240)]
    DMA[channel].cnt = control | DMA_ON;    
}

void drawRect(int row, int col, int height, int width, unsigned short color)
{
    volatile unsigned short c = color;
    for(int i = 0; i <= height; i++)
    {
        DMANow(3, &c, &videoBuffer[OFFSET(row + i, col, 240)], width | DMA_SOURCE_FIXED );
    }       
}

void fillScreen(unsigned short color)
{
    volatile unsigned short c = color;
    DMANow(3, &c, &videoBuffer[OFFSET(0, 0, 240)], 38400 | DMA_SOURCE_FIXED );
}

void drawbgImage(const unsigned short * image)
{
     DMANow(3, image, &videoBuffer[OFFSET(0, 0, 240)], 38400);
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

int getValue(unsigned int * stack, int base)
{
    int power;
    switch (base)
    {
        case 0: //binary
            power = 2;
            break;
        case 1: //octal
            power = 8;
            break;
        case 2: //decimal
            power = 10;
            break;
        default: //hexadecimal
            power = 16;
    }
    double sum = 0;
    for (int i = 0; i < 16; i++)
    {
        sum += ((int)stack[i]) * pow(power, i);
    }
    return (int)sum;
}

void changeBase(unsigned int * stack, int oldBase, int newBase)
{
    int quotient = getValue(stack, oldBase);
    int i = 0;
    switch(newBase)
    {
        case 0:
            newBase = 2;
            break;
        case 1:
            newBase = 8;
            break;
        case 2:
            newBase = 10;
            break;
        case 3:
            newBase = 16;

    }
    while(quotient!=0)
    {
        stack[i++]= quotient % newBase;;
        quotient = quotient / newBase;
    }
    for (int j = i; j < 16; j++)
    {
        stack[j] = 0;
    }
}

void toArray(int quotient, unsigned int * stack, int base)
{
    int i = 0;
    switch(base)
    {
        case 0:
            base = 2;
            break;
        case 1:
            base = 8;
            break;
        case 2:
            base = 10;
            break;
        case 3:
            base = 16;

    }
    while(quotient!=0)
    {
        stack[i++]= quotient % base;;
        quotient = quotient / base;
    }
    for (int j = i; j < 16; j++)
    {
        stack[j] = 0;
    }
}
/*
void toBinary(X)
{
    char arr[33];
    arr[32] = '\0';
    BIT = 31;
    M = 1;
    while (BIT != 0)
    {
        if (X & M)
            arr[BIT] = '1';
        else
            arr[BIT] = '0';
        M = M << 1;
        BIT = BIT - 1;
    }
}*/

void calculate(unsigned int * stack1, int operation, unsigned int * stack0, int base)
{
    int val1 = getValue(stack1, base);
    int val2 = getValue(stack0, base);
    int result = 0, clearAbove = 1; //clear the stack on top
    switch (operation)
    {
        case 0:
            // sprintf(str, " +");
            result = val1 + val2;
            break;
        case 1:
            // sprintf(str, " -");
            result = val1 - val2;
            break;
        case 2:
            // sprintf(str, " *");
            result = val1 * val2;
            break;
        case 3:
            // sprintf(str, " /");
            result = val1 / val2;
            break;
        case 4:
            // sprintf(str, "&&");
            result = val1 && val2;
            break;
        case 5:
            // sprintf(str, "||");
            result = val1 || val2;
            break;
        case 6:
            // sprintf(str, " !");
            result = !val2;
            clearAbove = 0;
            break;
        case 7:
            // sprintf(str, " &");
            result = val1 & val2;
            break;
        case 8:
            // sprintf(str, " |");
            result = val1 | val2;
            break;
        case 9:
            // sprintf(str, " ~");
            result = 65536 - val2;
            clearAbove = 0;
            break;
        case 10:
            // sprintf(str, " ^");
            result = val1 ^ val2;
            break;
        case 11:
            // sprintf(str, "<<");
            result = val1 << val2;
            break;
        case 12:
            // sprintf(str, ">>");
            result = val1 >> val2;
            break;
    }
    toArray(result, stack0, base);
    if (clearAbove)
    {
        for (int i = 0; i < 16; i++)
        {
            stack1[i] = 0;
        }
    }
}