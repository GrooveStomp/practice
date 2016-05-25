#include <stdio.h>
#include <termbox.h>
#include <stdlib.h>

#define false 0
#define true !false
typedef int bool;

bool
StringEqual(char *LeftString, char *RightString, int MaxNumToMatch)
{
	int NumMatched = 0;

	for(;
	    *LeftString == *RightString && NumMatched < MaxNumToMatch;
	    LeftString++, RightString++, MaxNumToMatch++)
	{
		if(*LeftString == '\0') return(true);
	}
	return(false);
}

int
StringLength(char *String)
{
	char *P = String;
	while(*P != '\0') P++;
	return(P - String);
}

void
ShowMessage(char *String)
{
	uint32_t BoxTopLeft = 0x250C;
	uint32_t BoxLowerLeft = 0x2514;
	uint32_t BoxTopRight = 0x2510;
	uint32_t BoxLowerRight = 0x2518;
	uint32_t BoxHorizontal = 0x2500;
	uint32_t BoxVertical = 0x2502;

	int Fg = TB_WHITE;
	int Bg = TB_BLACK;

	int BoxWidth = StringLength(String) + 4;
	int BoxHeight = 3;

	int Column;

	tb_change_cell(0, 0, BoxTopLeft, Fg, Bg);
	for(Column = 1; Column < BoxWidth - 1; ++Column)
	{
		tb_change_cell(Column, 0, BoxHorizontal, Fg, Bg);
	}
	tb_change_cell(BoxWidth-1, 0, BoxTopRight, Fg, Bg);


	tb_change_cell(0, 1, BoxVertical, Fg, Bg);
	tb_change_cell(1, 1, ' ', Bg, Bg);
	for(Column = 0; Column < StringLength(String); ++Column)
	{
		tb_change_cell(Column + 2, 1, String[Column], Fg, Bg);
	}

	tb_change_cell(BoxWidth-2, 1, ' ', Bg, Bg);
	tb_change_cell(BoxWidth-1, 1, BoxVertical, Fg, Bg);


	tb_change_cell(0, 2,BoxLowerLeft, Fg, Bg);
	for(Column = 1; Column < BoxWidth - 1; ++Column)
	{
		tb_change_cell(Column, 2, BoxHorizontal, Fg, Bg);
	}
	tb_change_cell(BoxWidth-1, 2, BoxLowerRight, Fg, Bg);
}

int
main(int ArgCount, char **Args)
{
	tb_init(); /* initialization */

	tb_clear(); /* clear buffer */
	ShowMessage("Hello, World!");
	tb_present(); /* sync internal buffer with terminal */

	struct tb_event Event;
	while(tb_poll_event(&Event))
	{
		if(TB_KEY_ESC == Event.key &&
		   TB_EVENT_KEY == Event.type)
		{
			break;
		}
	}

	tb_shutdown();
	return(EXIT_SUCCESS);
}
