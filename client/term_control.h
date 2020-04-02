#ifndef __TERM_CONTROL_H__
#define __TERM_CONTROL_H__

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#ifndef ECHOFLAGS
#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)
#endif

//option == 1? get back to old setting
//option == 0? Echo off, cursor off, enter flush off
int set_disp_mode(int option)
{
		int err;
		int fd = STDIN_FILENO;
		struct termios term;
		if(tcgetattr(fd,&term)==-1){
				perror("Cannot get the attribution of the terminal");
				return 1;
		}
		if(option) {
				term.c_lflag|=ECHOFLAGS;
				term.c_lflag|=ICANON;
				printf("\e[?25h");
		}
		else  {
				term.c_lflag &=~ECHOFLAGS;
				term.c_lflag &=~ICANON;
				printf("\e[?25l");
		}
		err=tcsetattr(fd,TCSAFLUSH,&term);
		if(err==-1 && err==EINTR){
				perror("Cannot set the attribution of the terminal");
				return 1;
		}
		return 0;

}

//Clear screen and put cursor on the left-top on display
void clearScreen()
{
		printf("\e[2J \e[0;0H");


}

void putCursor(int x, int y)
{
		printf("\e[%d;%dH", y+1, x+1);
}

enum{ NYX_COLOR_BLACK = 0, NYX_COLOR_RED, NYX_COLOR_GREEEN, NYX_COLOR_BROWN, NYX_COLOR_BLUE, NYX_COLOR_PURPLE, NYX_COLOR_TURQUOISE, NYX_COLOR_WHITE, NYX_COLOR_DEFAULT = 9};
void setColor(int x)
{
		if(x < 0 || x > 7){
				printf("\e[0;39;49m");
				return;
		}
		printf("\e[0;3%dm", x);
}


#endif
