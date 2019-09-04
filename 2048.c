
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};

//define the board sizes here, so we can play with it later
// 4 x 4 is default and good for testing
#define BOARD_W 4
#define BOARD_H 4

//Let's store the 2048 board, for now we will just use ints
int board[BOARD_H][BOARD_W] = {
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0}
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

//take the current state and draw it, nothing too hard
void draw_gameboard() {
  //for now we just use the buildin text output routines, but will change to something visually nicer later
  int i = 0;
  int j = 0;
  char *strval;
  
  //header
  vram_adr(NTADR_A(19,2));
  vram_write("2048", 4);
  
  for( i = BOARD_H; i > 0; i--) {
    for( j = BOARD_W; j > 0; j--) {
      itoa(board[i-1][j-1], strval, 10);
      
      vram_adr(NTADR_A( (4 +  (j - 1) * 5 ) + (4 - strlen(strval)), 1 + ( i * 4 )));
      vram_write(strval, strlen(strval));
    }
  }
}

//add a block to the board
void add_block() {
  //just add a block, start with anywhere, but we will soon need to make sure it's an empty spot
  int i = rand() % BOARD_H;
  int j = rand() % BOARD_W;
  int v = rand() % 10 ? 2: 4;	//1 in 10 will be a 4 block
  board[i][j] = v;
}

//init the board with some values
void init_gameboard() {
  int i = 0;
  int j = 0;
  
  //init random number generator....
  srand(7);
  add_block();
  add_block();
}

void main(void)
{
  setup_graphics();
  init_gameboard();
  draw_gameboard();
  // enable rendering
  ppu_on_all();
  // infinite loop
  while (1) {
   
  }
}
