
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>
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

//some values to define win/loss/continue state
#define GAME_CONTINUE 0
#define GAME_LOSE 1
#define GAME_WIN 2


//Let's store the 2048 board, for now we will just use ints
int board[BOARD_H][BOARD_W] = {
  { 0, 0, 0, 1024},
  { 0, 0, 0, 1024},
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
  char buf[32];
  vrambuf_clear();

  //header
  vram_adr(NTADR_A(19,2));
  vram_write("2048", 4);
  
  //trust me, works so much better then my first attempt
  for( i = BOARD_H - 1; i >= 0; i--) {
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%4d %4d %4d %4d", board[i][0], board[i][1], board[i][2], board[i][3]);
    vram_adr(NTADR_A(4, 5 + ( i * 4 )));
    vram_write(buf, sizeof(buf));
  }
}

//add a block to the board
void add_block() {
  bool done = false;
  int v = rand() % 10 ? 2: 4;	//1 in 10 will be a 4 block
  
  while(!done) {
    int i = rand() % BOARD_H;
    int j = rand() % BOARD_W;
    
    if(board[i][j] == 0) {
      board[i][j] = v;
      done = true;
    }
  }
}

//init the board with some values
void init_gameboard() {
  //TODO: wait for user to press start and use the frame count to init the srand() function
  int i = 0;
  int j = 0;
  
  //init random number generator....
  srand(0);
  add_block();
  add_block();//TODO: bring back to only 2 times after testing
  /*add_block();
  add_block();
  add_block();
  add_block();*/
}

//shift the numbers a direction
void move_shift(char dir) {
  int i = 0;
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
  int i = 0;
  int j = 0;
  switch (dir) {
    case PAD_LEFT: 
      for ( i = BOARD_H - 1; i >= 0; i--) {
        //BOARD_W - 1 here, as there is no match past the edge of the board
        for( j = 0; j < BOARD_W - 1 ; j++) {
          if(board[i][j] == board[i][j+1]){
            board[i][j] *= 2;
            board[i][j+1] = 0;
          }
        }
      }
      break;
    case PAD_RIGHT:
      for ( i = BOARD_H - 1; i >= 0; i--) {
        //here we have j >= 1 to avoid going past the edge
        for( j = BOARD_W - 1; j >= 1 ; j--) {
          if(board[i][j] == board[i][j-1]){
            board[i][j] *= 2;
            board[i][j-1] = 0;
          }
        }
      }
      break;
      
    case PAD_UP:
      for( j = 0; j < BOARD_W ; j++) {
        //again BOARD_H - 1 to avoid the edge
        for( i = 0; i < BOARD_H - 1; i++) {
          if(board[i][j] == board[i+1][j]){
            board[i][j] *= 2;
            board[i+1][j] = 0;
          }
        }
      }      
      break;
      
    case PAD_DOWN:
      for( j = 0; j < BOARD_W ; j++) {
        //again to i >= 1 to avoid the edge
        for( i = BOARD_H - 1; i >= 1 ; i--) {	
          if(board[i][j] == board[i-1][j]){
            board[i][j] *= 2;
            board[i-1][j] = 0;
          }
        }
      }     
      break;

    default: 
      break;
  }
}

//let's move the numbers...
void move_gameboard(char pad) {
   if (pad & PAD_LEFT) {
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
  
   add_block();
   draw_gameboard();

}

//check a pair of values if they are a valid move pair
// should be double numbers or 0 followed by a number
bool is_valid_pair(int a, int b) {
  if((a == 0) && (b != 0)) return true;
  if((a != 0) && (a == b)) return true;
}

//check if this is a valid move
bool is_valid_move(char pad) {
  int i = 0;
  int j = 0;

  if (pad & PAD_LEFT) {
    for ( i = BOARD_H - 1; i >= 0; i--) {
      for( j = 0; j < BOARD_W - 1 ; j++) {
        if(is_valid_pair(board[i][j], board[i][j+1])) return true;
      }
    }
  } else if (pad & PAD_RIGHT) {
    for ( i = BOARD_H - 1; i >= 0; i--) {
      for( j = BOARD_W - 1; j >= 1 ; j--) {
        if(is_valid_pair(board[i][j], board[i][j-1])) return true;
      }
    }
  } else if (pad & PAD_UP) {
    for( j = 0; j < BOARD_W ; j++) {
      for( i = 0; i < BOARD_H - 1; i++) {
        if(is_valid_pair(board[i][j], board[i+1][j])) return true;
      }
    }
  } else if (pad & PAD_DOWN) {
    for( j = 0; j < BOARD_W ; j++) {
      for( i = BOARD_H - 1; i >= 1 ; i--) {	
        if(is_valid_pair(board[i][j], board[i-1][j])) return true;
      }
    }
  }
  return false; 
}

//check the status of the game
bool game_win_lose() {
  //brute force, just roll over all values and check for 2048 and 0s
  int i = 0;
  int j = 0;
  bool win = false;
  bool lose = false;

  for( i = 0; i < BOARD_H; i++) {
    for( j = 0; j < BOARD_W; j++) {
      if(board[i][j] == 2048) return GAME_WIN;	//TODO make this changable for harder modes
    }
  }
  //we have to check for continue after win, otherwise it often will find a 0 first.
  for( i = 0; i < BOARD_H; i++) {
    for( j = 0; j < BOARD_W; j++) {
      if(board[i][j] == 0) return GAME_CONTINUE;
    }
  }
     
  //no 0s on the board, and no 2048, so player loses
  return GAME_LOSE;
}

//let the player know they won/lost
void draw_winscreen(int state) {
  vram_adr(NTADR_A(9,20));
  if(state == GAME_WIN) vram_write("WINNER!", 7);
  if(state == GAME_LOSE) vram_write("GAME OVER", 9);
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
    int state;
    ppu_off();
    pad = pad_poll(0);
    if(is_valid_move(pad)) move_gameboard(pad);
    state = game_win_lose();
    if(state != 0) draw_winscreen(state);
    ppu_on_all();

    while (!pad_trigger(0)) {
      ppu_wait_nmi();
    }
  }
}
