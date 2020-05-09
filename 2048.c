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

//Import the pallet and startup screen
extern const byte game_title_pal[16];
extern const byte game_title_rle[];
extern const byte game_board_rle[];

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
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0}
};

//Borrow routine to fade in the splash screen.
void fade_in() {
  byte vb;
  for (vb=0; vb<=4; vb++) {
    // set virtual bright value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
}
//and borrowed routine to display the spash screen.
void show_title_screen(const byte* pal, const byte* rle) {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);
  pal_bright(0);
  // unpack nametable into the VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(rle);
  // enable rendering
  ppu_on_all();
  // fade in from black
  fade_in();
}


// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // clear buffer
  vrambuf_clear();
  // set palette colors
  pal_all(game_title_pal);
  // set NMI handler
  set_vram_update(updbuf);
  //init ppu
  ppu_on_all();
}

void fill_tile(int x, int y, int val) {
  int nx = (x * 4) + 9;
  int ny = (y * 4) + 9;
  
  char vals[2][2] = {
    { 0, 0},
    { 0, 0}
  };
  switch (val) {
    case 2:
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0xB2;      
      break;
    case 4:
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0xB4;     
      break;
    case 8:
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0xB8;      
      break;
    case 16:
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0xB1;
      vals[1][1] = 0xB6;      
      break;
    case 32:
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0xB3;
      vals[1][1] = 0xB2;      
      break;
    case 64:
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0xB6;
      vals[1][1] = 0xB4;      
      break;
    case 128:
      vals[0][0] = 0x03;
      vals[0][1] = 0xB1;
      vals[1][0] = 0xB2;
      vals[1][1] = 0xB8;      
      break;
    case 256:
      vals[0][0] = 0x03;
      vals[0][1] = 0xB2;
      vals[1][0] = 0xB5;
      vals[1][1] = 0xB6;      
      break;
    case 512:
      vals[0][0] = 0x03;
      vals[0][1] = 0xB5;
      vals[1][0] = 0xB1;
      vals[1][1] = 0xB2;      
      break;
    case 1024:
      vals[0][0] = 0xB1;
      vals[0][1] = 0xB0;
      vals[1][0] = 0xB2;
      vals[1][1] = 0xB4;      
      break;
    case 2048:
      vals[0][0] = 0xB2;
      vals[0][1] = 0xB0;
      vals[1][0] = 0xB4;
      vals[1][1] = 0xB8;      
      break;
      
    default:
      //set it blank
      /* // Debug output version
      vals[0][0] = 4*(4*x+y)+0;
      vals[0][1] = 4*(4*x+y)+1;
      vals[1][0] = 4*(4*x+y)+2;
      vals[1][1] = 4*(4*x+y)+3;
      */
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0x03;
  }
  vrambuf_put(NTADR_A(nx,ny),vals[0],2);
  vrambuf_put(NTADR_A(nx,ny+1),vals[1],2);
}

void draw_gameboard_bg() {
  ppu_wait_frame();
  ppu_off();
  // unpack nametable into the VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(game_board_rle);
  ppu_on_all();
}

//take the current state and draw it
void draw_gameboard() {
  int i;
  int j;
  //We run this twice, as it is too many updates for the code otherwise
  //TODO: genericize this for larger boards.
  for( i = 0; i < BOARD_H / 2; i++) {
    for( j = 0; j < BOARD_W; j++) {
      fill_tile(i, j, board[j][i]);
    }
  }
  vrambuf_flush();
  for( i = BOARD_H / 2; i < BOARD_H; i++) {
    for( j = 0; j < BOARD_W; j++) {
      fill_tile(i, j, board[j][i]);
    }
  }
  vrambuf_flush();
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
  //init random number generator, use a framecounter to make it more random
  srand(nesclock());
  add_block();
  add_block();
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
     //we do the shift twice to handle gaps after merge
     move_shift(PAD_LEFT);
     move_merge(PAD_LEFT);
     move_shift(PAD_LEFT);
   } else if (pad & PAD_RIGHT) {
     move_shift(PAD_RIGHT);
     move_merge(PAD_RIGHT);
     move_shift(PAD_RIGHT);
   } else if (pad & PAD_UP) {
     move_shift(PAD_UP);
     move_merge(PAD_UP);
     move_shift(PAD_UP);
   } else if (pad & PAD_DOWN) {
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
  //we run out of time to draw this, causing it to not get into vram
  //instead we wait a frame...
  ppu_on_all();
  ppu_wait_frame();
  ppu_off();
  vram_adr(NTADR_A(12,24));
  if(state == GAME_WIN) vram_write("WINNER!", 7);
  if(state == GAME_LOSE) vram_write("GAME OVER", 9);
  //vrambuf TODO
  vram_adr(NTADR_A(11,25));
  vram_write("PRESS START", 11);
}

void reset_gameboard() {
  int i, j;
  for( j = 0; j < BOARD_W ; j++) {
    for( i = 0; i < BOARD_H ; i++) {
      board[i][j] = 0;
    }
  }
}

void main(void)
{
  char pad;	// controller flags

  setup_graphics();
  show_title_screen(game_title_pal, game_title_rle);
  while(1){
    pad = pad_poll(0);
    if(pad & PAD_START)	break;
  }
  
  
  /*init_gameboard();
  draw_gameboard();
  // enable rendering
  ppu_on_all();*/
  // infinite loops
  while(1) {
    init_gameboard();
    draw_gameboard_bg();
    draw_gameboard();
 
    while (1) {
      int state;
      pad = pad_poll(0);
      state = game_win_lose();
      //TODO: fix this state logic block somehow....
      if(state != 0) {
        draw_winscreen(state);
        if(pad & PAD_START) break;
      } else {
        if(is_valid_move(pad)) move_gameboard(pad);
        state = game_win_lose();
        if(state != 0) {
          draw_winscreen(state);
          //if(pad & PAD_START) break;
        }
      }
      while (!pad_trigger(0)) {
        ppu_wait_nmi();
      }
    }
    //I keep this down here so I can pre-populate the array during testing
    reset_gameboard();
  }
}

//#link "title.s"
