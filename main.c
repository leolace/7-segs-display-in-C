#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>

#define SEG_BASE_SIZE 8

#define SEG_H_WIDTH ((int)(SEG_BASE_SIZE * 2.5))
#define SEG_H_HEIGHT ((int)(SEG_BASE_SIZE / 2.5))
#define SEG_V_WIDTH SEG_BASE_SIZE
#define SEG_V_HEIGHT SEG_BASE_SIZE

typedef struct {
  int x;
  int y;
  char orientation;
  int sign;
} Seg;

int parse_4byte_to_decimal(int *byte)
{
  int res = 0;

  for (int i = 3; i >= 0 ; i--) {
    res += byte[i] * pow(2, 3 - i);
  }
  
  return res;
}

void hcreate_seg(int y, int x)
{
  for(int i = 0; i < SEG_H_HEIGHT; i++) {
    for (int j = 0; j < SEG_H_WIDTH; j++) {
      move(y + i, x + j);
      printw("█");
    }
  }

  move(0, 0);
}

void vcreate_seg(int y, int x)
{
  for (int i = 0; i < SEG_V_HEIGHT; i++) {
    for(int j = 0; j < SEG_V_WIDTH; j++) {
      move(y + i , x + j);
      printw("█");
    }
  }
  
  move(0, 0);
}

void create_seg(int y, int x, char type)
{
  int rows = 0;
  int cols = 0;
  getmaxyx(stdscr, rows, cols);

  x = x + (cols / 2) - ((SEG_H_WIDTH + (SEG_V_WIDTH * 2)) / 2);
  y = y + (rows / 2) - (((SEG_H_HEIGHT * 3) + (SEG_V_HEIGHT * 2)) / 2);
  
  switch (type) {
  case 'h':
    hcreate_seg(y, x);
    break;
  case 'v':
    vcreate_seg(y, x);
    break;
  default:
    break;
  }
}

int * parse_string_to_int(char *string)
{
  int *int_sequence = (int*)(malloc((sizeof(string)/sizeof(char)) * sizeof(int)));
 
  for (int i = 0; i < sizeof(string)/sizeof(char); i++) {
    int_sequence[i] = string[i] - '0';
  }
  
  return int_sequence;
}

int * get_byte()
{
  char *input = (char*)(malloc(sizeof(char) * 5));
  int rows = 0;
  int cols = 0;
  echo();
  
  getmaxyx(stdscr, rows, cols);
  
  do {
    mvprintw(rows - 2, 0, "digite um byte de 4 bits: ");
    refresh();
    scanw("%4s", input);
  } while(strlen(input) < 4);
    
  int *byte = parse_string_to_int(input);
  noecho();
  
  return byte;
}

Seg * render_display(Seg *segs)
{
  if(has_colors() == FALSE) {
    endwin();
    printf("Your terminal doesnt support colors");
    exit(0);
  }
  
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  
  for (int i = 1; i <= 7; i++) {
    int index = i - 1;
    switch(i) {
    case 1:
      segs[index].x = 0;
      segs[index].y = SEG_H_HEIGHT;
      segs[index].orientation = 'v';
      break;
    case 2:
      segs[index].x = SEG_V_WIDTH;
      segs[index].y = 0;
      segs[index].orientation = 'h';
      break;
    case 3:
      segs[index].x = SEG_H_WIDTH + SEG_V_WIDTH;
      segs[index].y = SEG_H_HEIGHT;
      segs[index].orientation = 'v';
      break;
    case 4:
      segs[index].x = SEG_V_WIDTH;
      segs[index].y = SEG_V_HEIGHT + SEG_H_HEIGHT;
      segs[index].orientation = 'h';
      break;
    case 5:
      segs[index].x = 0;
      segs[index].y = SEG_V_HEIGHT + (SEG_H_HEIGHT * 2);
      segs[index].orientation = 'v';
      break;
    case 6:
      segs[index].x = SEG_V_WIDTH;
      segs[index].y = (SEG_V_HEIGHT * 2) + (SEG_H_HEIGHT * 2);
      segs[index].orientation = 'h';
      break;
    case 7:
      segs[index].x = SEG_H_WIDTH + SEG_V_WIDTH;
      segs[index].y = SEG_V_HEIGHT + (SEG_H_HEIGHT * 2);
      segs[index].orientation = 'v';
      break;
    default:
      break;
    }
  }
  clear();
	 
  for (int i = 0; i < 7; i++) {
    if(segs[i].sign == 0) {
      attron(COLOR_PAIR(2));
      create_seg(segs[i].y, segs[i].x, segs[i].orientation);
      attroff(COLOR_PAIR(2));
      continue;
    }
    create_seg(segs[i].y, segs[i].x, segs[i].orientation);
  }

  return segs;
}

void byte_to_display(Seg *segs, int *byte)
{
  bool A = byte[0];
  bool B = byte[1];
  bool C = byte[2];
  bool D = byte[3];

  segs[0].sign = !(int)((!A && B && !C) || (!C && !D) || (B && !D) || (A && !B) || (A && C)); // f
  segs[1].sign = !(int)((A && !B && !C) || (!A && B && D) || (A && !D) || (!A && C) || (B && C) || (!B && !D)); // a
  segs[2].sign = !(int)((!A && !C && !D) || (!A && C && D) || (A && !C && D) || (!B && !C) || (!B && !D)); // b
  segs[3].sign = !(int)((!A && B && !C) || (!B && C) || (C && !D) || (A && !B) || (A && D)); // g
  segs[4].sign = !(int)((!B && !D) || (C && !D) || (A && C) || (A && B)); // e
  segs[5].sign = !(int)((!A && !B && !D) || (!B && C && D) || (B && !C && D) || (B && C && !D) || (A && !C)); // d
  segs[6].sign = !(int)((!A && !C) || (!A && D) || (!C && D) || (!A && B) || (A && !B)); // c
}

int main()
{
  char ch = 0;
  
  setlocale(LC_ALL, "");
  initscr();
  
  raw();
  while (ch != 'q') {
    Seg *segs = (Seg*)malloc(sizeof(Seg) * 7);
    int *byte = get_byte();
    byte_to_display(segs, byte);
    render_display(segs);

    ch = getch();
  }
  endwin();
 
    
  return 0;
}
