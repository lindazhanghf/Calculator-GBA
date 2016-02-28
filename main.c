#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "myLib.h"
#include "text.h"

// Prototypes
void start();
void game();
void pause();
void win();
void lose();

void initialize();
int endRound();
void update();
void draw();

unsigned int buttons;
unsigned int oldButtons;

char str[41];
//stack[0][0] != oldStack[0][0]
unsigned int stack[4][16];
unsigned int tempStack[16];
int oldBase;
int operation;
char hasUpdate[4] = {1, 1, 1, 1}; //for erasing stack
POINTER numPointer;
POINTER basePointer;
int base[4] = {2, 8, 10, 16};
int baseRow[4] = {110, 120, 130, 140}; //row values for base pointer
int maxDigits[4] = {16, 6, 5, 4}; //max numer of digit allowed for each base
//operations[13] = {'+', '-', '*', '/', '&&', '||', '!', '&', '|', '~', '^', '<<', '>>'};
//						  0    1    2    3    4     5     6    7    8    9    10   11    12

int main()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	initialize();

	while (1) {
		oldButtons = buttons;
		buttons = BUTTONS;
		update();
		draw();
	}
	return 0;
}


void initialize()
{
	operation = 0; //PLUS

	numPointer.position = 221;
	numPointer.value = 0; //pointing at the right-most bit
	basePointer.position = 130;
	basePointer.value = 2; //pointing at the third base - Decimal
	oldBase = basePointer.value;

	//print the rules
	sprintf(str, "Stack Calculator");
	drawString(10, 10, str, GREEN);
	sprintf(str, "B: operate  A: change operation");
	drawString(20, 10, str, GREEN);
	sprintf(str, "Start: move up  Select: move down");
	drawString(30, 10, str, GREEN);
	sprintf(str, "L: swap  R: change base");
	drawString(40, 10, str, GREEN);

	sprintf(str, "Binary");
	drawString(110, 10, str, GREEN);
	sprintf(str, "Octal");
	drawString(120, 10, str, GREEN);
	sprintf(str, "Decimal");
	drawString(130, 10, str, GREEN);
	sprintf(str, "Hexadecimal");
	drawString(140, 10, str, GREEN);

	draw();
}

void update()
{
	numPointer.oldPosition = numPointer.position;
	basePointer.oldPosition = basePointer.position;
	oldBase = basePointer.value;

	//changing the number pointer
	if (BUTTON_PRESSED(BUTTON_LEFT) && numPointer.value < maxDigits[basePointer.value] - 1)
	{
		numPointer.value++;
		numPointer.position = numPointer.position - 6;
	}
	else if (BUTTON_PRESSED(BUTTON_RIGHT) && numPointer.value > 0)
	{
		numPointer.value--;
		numPointer.position = numPointer.position + 6;
	}

	//chaning base
	if (BUTTON_PRESSED(BUTTON_R))
	{
		if (basePointer.value == 3)
		{
			basePointer.value = 0;
		}
		else
		{
			basePointer.value++;
		}
		basePointer.position = baseRow[basePointer.value];
		//correct number pointer
		if (numPointer.value >= maxDigits[basePointer.value] - 1)
		{
			numPointer.value = maxDigits[basePointer.value] - 1;
		}
		numPointer.position = 221 - 6 * numPointer.value;

		//convert numbers to the new base
		for (int i = 0; i < 4; i++)
		{
			changeBase(stack[i], oldBase, basePointer.value);
			hasUpdate[i] = 1;
		}

		// hasUpdate = 1;
	}

	//chaning operation
	if (BUTTON_PRESSED(BUTTON_A))
	{
		if (operation == 12)
		{
			operation = 0;
		}
		else
		{
			operation++;
		}
	}

	if (BUTTON_PRESSED(BUTTON_UP))
	{
		int value = getValue(stack[0], basePointer.value);
		if ((value + pow(base[basePointer.value], numPointer.value)) >= 65536)//stack[0][numPointer.value] == 0
		{
			value = value - 65536;
		}
		value = value + pow(base[basePointer.value], numPointer.value);
		toArray(value, stack[0], basePointer.value);
		hasUpdate[0] = 1;
	}
	if (BUTTON_PRESSED(BUTTON_DOWN))
	{
		int value = getValue(stack[0], basePointer.value);
		if ((value - pow(base[basePointer.value], numPointer.value)) < 0)//stack[0][numPointer.value] == 0
		{
			value = value + 65536;
		}
		value = value - pow(base[basePointer.value], numPointer.value);
		toArray(value, stack[0], basePointer.value);
		hasUpdate[0] = 1;
	}

	if (BUTTON_PRESSED(BUTTON_START)) //Roll up
	{
		for (int i = 2; i >= 0; i--)
		{
			for (int j = 0; j < 16; j++)
			{
				stack[i+1][j] = stack[i][j];
			}
			hasUpdate[i+1] = 1;
		}
		for (int i = 0; i < 16; i++)
		{
			stack[0][i] = 0;
		}
		hasUpdate[0] = 1;
	}
	else if (BUTTON_PRESSED(BUTTON_SELECT)) //Roll down
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				stack[i][j] = stack[i+1][j];
			}
			hasUpdate[i+1] = 1;
		}
		for (int i = 0; i < 16; i++)
		{
			stack[3][i] = 0;
		}
		hasUpdate[0] = 1;
	}
	else if (BUTTON_PRESSED(BUTTON_L)) //Switch stack 0 and 1
	{
		for (int i = 0; i < 16; i++)
		{
			tempStack[i] = stack[0][i];
			stack[0][i] = stack[1][i];
			stack[1][i] = tempStack[i];
		}	
		hasUpdate[1] = 1;
		hasUpdate[0] = 1;
	}

	if (BUTTON_PRESSED(BUTTON_B))
	{
		calculate(stack[1], operation, stack[0], basePointer.value);
		hasUpdate[1] = 1;
		hasUpdate[0] = 1;
	}
}

void draw() {
	waitForVblank();

	for (int j = 3; j >= 0; j--)
	{
		if (hasUpdate[j] == 1)
		{
			drawRect(140 - j*10, 130, 8, 6*16, BLACK);
		for (int i = 0; i < 16; i++)
		{
			//drawRect(140 - j*10, 130 + i*6, 8, 6, BLACK);
			if (basePointer.value == 3) //hexadecimal
			{
				switch (stack[j][15 - i])
				{
					case 10:
						sprintf(str, "A");
						break;
					case 11:
						sprintf(str, "B");
						break;
					case 12:
						sprintf(str, "C");
						break;
					case 13:
						sprintf(str, "D");
						break;
					case 14:
						sprintf(str, "E");
						break;
					case 15:
						sprintf(str, "F");
						break;
					default:
						sprintf(str, "%d", stack[j][15 - i]); 
						break;
				}

			}
			else
				sprintf(str, "%d", stack[j][15 - i]);
			drawString(140 - j*10, 130 + i*6, str, GREEN);
		}
	}
	hasUpdate[j] = 0;
	}
	//draw pointers
	if (numPointer.oldPosition != numPointer.position)
	{
		drawRect(149, numPointer.oldPosition, 1, 6, BLACK);
	}
	drawRect(149, numPointer.position, 1, 6, RED);
	if (basePointer.oldPosition != basePointer.position)
	{
		drawRect(basePointer.oldPosition, 7, 7, 2, BLACK);
	}
	drawRect(basePointer.position, 7, 7, 2, RED);

	switch (operation)
	{
		case 0:
			sprintf(str, " +");
			break;
		case 1:
			sprintf(str, " -");
			break;
		case 2:
			sprintf(str, " *");
			break;
		case 3:
			sprintf(str, " /");
			break;
		case 4:
			sprintf(str, "&&");
			break;
		case 5:
			sprintf(str, "||");
			break;
		case 6:
			sprintf(str, " !");
			break;
		case 7:
			sprintf(str, " &");
			break;
		case 8:
			sprintf(str, " |");
			break;
		case 9:
			sprintf(str, " ~");
			break;
		case 10:
			sprintf(str, " ^");
			break;
		case 11:
			sprintf(str, "<<");
			break;
		case 12:
			sprintf(str, ">>");
			break;
	}
	drawRect(140, 115, 8, 12, BLACK);
	drawString(140, 115, str, GREEN);
}