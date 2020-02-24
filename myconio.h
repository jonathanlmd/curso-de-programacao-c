#include <termios.h>
#include <stdio.h>
#include <stdlib.h>


//gotoxy() function definition
void gotoxy(int x,int y)    
{
    printf("%c[%d;%df",0x1B,y,x);
}

//clrscr() function definition
void clrscr(void)
{
    system("clear");
}
 
static struct termios old, novo;
 
/* Initialize novo terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); //grab old terminal i/o settings
  novo = old; //make novo settings same as old settings
  novo.c_lflag &= ~ICANON; //disable buffered i/o
  novo.c_lflag &= echo ? ECHO : ~ECHO; //set echo mode
  tcsetattr(0, TCSANOW, &novo); //apply terminal io settings
}
 
/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}
 
/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}
 
/* 
Read 1 character without echo 
getch() function definition.
*/
char getch(void) 
{
  return getch_(0);
}
 
/* 
Read 1 character with echo 
getche() function definition.
*/
char getche(void) 
{
  return getch_(1);
}
