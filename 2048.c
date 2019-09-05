
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
  { 1, 0, 0, 2},
  { 0, 3, 0, 0},
  { 5, 6, 7, 0},
  { 0, 4, 0, 8}
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
  int vi = 0;
  int vj = 0;
  char *strval;
  vrambuf_clear();

  //header
  vram_adr(NTADR_A(19,2));
  vram_write("2048", 4);
  
  for( vi = BOARD_H; vi > 0; vi--) {
    for( vj = BOARD_W; vj > 0; vj--) {
      itoa(board[vi-1][vj-1], strval, 10);
      
      vram_adr(NTADR_A( (4 +  (vj - 1) * 5 ) + (4 - strlen(strval)), 1 + ( vi * 4 )));
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
  //TODO: wait for user to press start and use the frame count to init the srand() function
  int i = 0;
  int j = 0;
  
  //init random number generator....
  srand(0);
  /*add_block();
  add_block();//TODO: bring back to only 2 times after testing
  add_block();
  add_block();
  add_block();
  add_block();*/
}

//shift the numbers a direction
void move_shift(char dir) {
  int i = 1;
  int j = 0;
  int c = 0;
  
  
  switch (dir) {
    case PAD_LEFT: 
      for ( i = BOARD_H - 1; i >= 0; i--) {
        c = 0;
        for( j = 0; j < BOARD_W ; j++) {	
          if (board[i][j] != 0) {
            board[i][c] = board[i][j];
            c++;
          }
        }
        for( j = c; j < BOARD_W; j++) {
          board[i][j] = 0;
        }
      }
      break;
      
    case PAD_RIGHT:
      for ( i = BOARD_H - 1; i >= 0; i--) {
        c = 0;
        for( j = BOARD_W - 1; j >= 0 ; j--) {	
          if (board[i][j] != 0) {
            board[i][3 - c] = board[i][j];
            c++;
          }
        }
        for( j = BOARD_W - 1 - c; j >= 0; j--) {
          board[i][j] = 0;
        }
      }
      break;
      
    case PAD_UP:
      for( j = 0; j < BOARD_W ; j++) {
        c = 0;
        for( i = 0; i < BOARD_H ; i++) {
          if (board[i][j] != 0) {
            board[c][j] = board[i][j];
            c++;
          }
        }
        for( i = c; i < BOARD_W; i++) {
          board[i][j] = 0;
        }
      }
    break;
      
    case PAD_DOWN:
      for ( j = BOARD_W - 1; j >= 0; j--) {
        c = 0;
        for( i = BOARD_H - 1; i >= 0 ; i--) {	
          if (board[i][j] != 0) {
            board[3 - c][j] = board[i][j];
            c++;
          }
        }
        for( i = BOARD_H - 1 - c; i >= 0; i--) {
          board[i][j] = 0;
        }
      }
      break;
      
    default:
      //this should not happen... do nothing for now
      break;
  }
  
}

//do merges based on direction pressed
void move_merge(char dir) {
  if (dir == PAD_LEFT) {
  }
}

//let's move the numbers...
void move_gameboard(char pad) {
   if (pad & PAD_LEFT) {
     //vram_write("\x1c\x1d\x1e\x1f to move metasprite", 24);
     vram_adr(NTADR_A(7,2));
     vram_write("\x1e", 1);
     //we do the shift twice to handle gaps after merge
     move_shift(PAD_LEFT);
     move_merge(PAD_LEFT);
     move_shift(PAD_LEFT);
   } else if (pad & PAD_RIGHT) {
     vram_adr(NTADR_A(7,2));
     vram_write("\x1f", 1);
     move_shift(PAD_RIGHT);
     move_merge(PAD_RIGHT);
     move_shift(PAD_RIGHT);
   } else if (pad & PAD_UP) {
     vram_adr(NTADR_A(7,2));
     vram_write("\x1c", 1);
     move_shift(PAD_UP);
     move_merge(PAD_UP);
     move_shift(PAD_UP);
   } else if (pad & PAD_DOWN) {
     vram_adr(NTADR_A(7,2));
     vram_write("\x1d", 1);
     move_shift(PAD_DOWN);
     move_merge(PAD_DOWN);
     move_shift(PAD_DOWN);
   }
   draw_gameboard();
}

//check if this is a valid move
bool is_valid_move(char pad) {
  char a = pad;
  return true; 
}

void main(void)
{
  char pad;	// controller flags

  setup_graphics();
  init_gameboard();
  draw_gameboard();
  // enable rendering
  ppu_on_all();
  // infinite loop
  while (1) {
    ppu_off();
    pad = pad_poll(0);
    if(is_valid_move(pad)) move_gameboard(pad);
    ppu_on_all();

    while (!pad_trigger(0)) {
      ppu_wait_nmi();
    }
  }
}
