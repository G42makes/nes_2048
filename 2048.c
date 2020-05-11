#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

//link in the titles file
//#link "title.s"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

//Import the pallet and startup screen
extern const byte game_title_pal[16];
extern const byte game_title_rle[];
extern const byte game_board_rle[];
extern const byte game_options_rle[];

//some values to define win/loss/continue state
#define GAME_CONTINUE 0
#define GAME_LOSE 1
#define GAME_WIN 2

//Let's store the 2048 board, we store the power of 2 in bytes
// so each element can store up to 2^256, but we can only use 4 numbers
// to display, so max score is 2^13=8192.
//TODO, make this more dynamic based on board size
byte board[4][4] = {
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0}
};

//Setup the initial state for all options:
//Win Score, max is based on we can only print 4 numers per tile
//TODO: can we surpass 2^13 with specially crafted number tiles to fit more?
byte win_score = 11;	//2^11 = 2048
#define MAX_SCORE 13	//2^13 = 8192

//Board definitions
byte board_width = 4;
byte board_height = 4;
#define BOARD_MAX_HEIGHT = 7; //might be into the borders
#define BOARD_MIN_HEIGHT = 2; //does this make sense?
#define BOARD_MAX_WIDTH  = 8; //might be into the borders
#define BOARD_MIN_WIDTH  = 2; //does this make sense?

//Visual settings
//not used yet
byte tile_style = 0;
byte pallette = 0;
byte background = 0;

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
  // unpack nametable into the VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(rle);
  //set it to faded out before enabling
  pal_bright(0);
  // enable rendering
  ppu_on_all();
  // fade in from black
  fade_in();
}

//bring up the options page and let uses select changes
//TODO: this does nothing right now
void show_options_page() {
  char pad;
  ppu_off();
  pal_bg(game_title_pal);
  vram_adr(NAMETABLE_A);
  vram_unrle(game_options_rle);
  pal_bright(0);
  ppu_on_all();
  fade_in();
  
  while(1){
      pad = pad_poll(0);
      if(pad & PAD_SELECT) {
        pal_bright(0);
        return;      
      }
  }
}

//add a block to the board
void add_block() {
  bool done = false;
  int v = rand() % 10 ? 1: 2;	//1 in 10 will be a 2^2 block
  
  while(!done) {
    int i = rand() % board_height;
    int j = rand() % board_width;
    
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

// put the specified value in the x,y location requested on the playboard
void fill_tile(int x, int y, byte pow) {
  //calculate the offset for each direction
  int nx = (x * 4) + 9;
  int ny = (y * 4) + 9;
  
  //start with an empty array
  char vals[2][2] = {
    { 0, 0},
    { 0, 0}
  };
  //update the values in the array based on the request
  // TODO: is there a more effecient way to store this data?
  switch (pow) {
    case 1:		//   2
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0xB2;      
      break;
    case 2:		//   4
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0xB4;     
      break;
    case 3:		//   8
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0xB8;      
      break;
    case 4:		//  16
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0xB1;
      vals[1][1] = 0xB6;      
      break;
    case 5:		//  32
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0xB3;
      vals[1][1] = 0xB2;      
      break;
    case 6:		//  64
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0xB6;
      vals[1][1] = 0xB4;      
      break;
    case 7:		// 128
      vals[0][0] = 0x03;
      vals[0][1] = 0xB1;
      vals[1][0] = 0xB2;
      vals[1][1] = 0xB8;      
      break;
    case 8:		// 256
      vals[0][0] = 0x03;
      vals[0][1] = 0xB2;
      vals[1][0] = 0xB5;
      vals[1][1] = 0xB6;      
      break;
    case 9:		// 512
      vals[0][0] = 0x03;
      vals[0][1] = 0xB5;
      vals[1][0] = 0xB1;
      vals[1][1] = 0xB2;      
      break;
    case 10:		//1024
      vals[0][0] = 0xB1;
      vals[0][1] = 0xB0;
      vals[1][0] = 0xB2;
      vals[1][1] = 0xB4;      
      break;
    case 11:		//2048
      vals[0][0] = 0xB2;
      vals[0][1] = 0xB0;
      vals[1][0] = 0xB4;
      vals[1][1] = 0xB8;      
      break;
    case 12:		//4096
      vals[0][0] = 0xB1;
      vals[0][1] = 0xB0;
      vals[1][0] = 0xB2;
      vals[1][1] = 0xB4;      
      break;
    case 13:		//8192
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
  //char tmp[20];
  //We run this twice, as it is too many updates for the code otherwise
  //TODO: genericize this for larger boards.
  for( i = 0; i < board_height / 2; i++) {
    for( j = 0; j < board_width; j++) {
      fill_tile(i, j, board[i][j]);
    }
  }
  vrambuf_flush();
  for( i = board_height / 2; i < board_height; i++) {
    for( j = 0; j < board_width; j++) {
      fill_tile(i, j, board[i][j]);
    }
  }
  vrambuf_flush();
  
  
  //debug code
  /*
  for( i = 0; i<4; i++){
    for( j=0; j<4; j++){
      tmp[i*5 +j] = (char)(board[i][j] + 0x30);
    }
    tmp[i*5+4]=0;
  }
  vrambuf_put(NTADR_A(5,5),tmp,20);
  vrambuf_flush();*/
}

//shift the numbers a direction, no merging of matched values
void move_shift(char dir) {
  int i = 0;
  int j = 0;
  int c = 0;
  
  
  switch (dir) {
    case PAD_UP: 
      for ( i = board_height - 1; i >= 0; i--) {
        c = 0;
        for( j = 0; j < board_width ; j++) {	
          if (board[i][j] != 0) {
            board[i][c] = board[i][j];
            c++;
          }
        }
        for( j = c; j < board_width; j++) {
          board[i][j] = 0;
        }
      }
      break;
      
    case PAD_DOWN:
      for ( i = board_height - 1; i >= 0; i--) {
        c = 0;
        for( j = board_width - 1; j >= 0 ; j--) {	
          if (board[i][j] != 0) {
            board[i][3 - c] = board[i][j];
            c++;
          }
        }
        for( j = board_width - 1 - c; j >= 0; j--) {
          board[i][j] = 0;
        }
      }
      break;
      
    case PAD_LEFT:
      for( j = 0; j < board_width ; j++) {
        c = 0;
        for( i = 0; i < board_height ; i++) {
          if (board[i][j] != 0) {
            board[c][j] = board[i][j];
            c++;
          }
        }
        for( i = c; i < board_width; i++) {
          board[i][j] = 0;
        }
      }
      break;
      
    case PAD_RIGHT:
      for ( j = board_width - 1; j >= 0; j--) {
        c = 0;
        for( i = board_height - 1; i >= 0 ; i--) {	
          if (board[i][j] != 0) {
            board[3 - c][j] = board[i][j];
            c++;
          }
        }
        for( i = board_height - 1 - c; i >= 0; i--) {
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
    case PAD_UP: 
      for ( i = board_height - 1; i >= 0; i--) {
        //board_width - 1 here, as there is no match past the edge of the board
        for( j = 0; j < board_width - 1 ; j++) {
          if((board[i][j] == board[i][j+1]) && (board[i][j] !=0)) {
            board[i][j] += 1;
            board[i][j+1] = 0;
          }
        }
      }
      break;
    case PAD_DOWN:
      for ( i = board_height - 1; i >= 0; i--) {
        //here we have j >= 1 to avoid going past the edge
        for( j = board_width - 1; j >= 1 ; j--) {
          if((board[i][j] == board[i][j-1]) && (board[i][j] !=0)){
            board[i][j] += 1;
            board[i][j-1] = 0;
          }
        }
      }
      break;
      
    case PAD_LEFT:
      for( j = 0; j < board_width ; j++) {
        //again board_height - 1 to avoid the edge
        for( i = 0; i < board_height - 1; i++) {
          if((board[i][j] == board[i+1][j]) && (board[i][j] !=0)){
            board[i][j] += 1;
            board[i+1][j] = 0;
          }
        }
      }      
      break;
      
    case PAD_RIGHT:
      for( j = 0; j < board_width ; j++) {
        //again to i >= 1 to avoid the edge
        for( i = board_height - 1; i >= 1 ; i--) {	
          if((board[i][j] == board[i-1][j]) && (board[i][j] !=0)){
            board[i][j] += 1;
            board[i-1][j] = 0;
          }
        }
      }     
      break;

    default: 
      break;
  }
}

//let's move and merge the numbers...
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

  //add a block to somewhere empty on the board
  add_block();
  //and draw the update
  draw_gameboard();
}

//check a pair of values if they are a valid move pair
// should be double numbers or 0 followed by a number
bool is_valid_pair(byte a, byte b) {
  if((a == 0) && (b != 0)) return true;
  if((a != 0) && (a == b)) return true;
}

//check if this is a valid move
bool is_valid_move(char pad) {
  int i = 0;
  int j = 0;

  if (pad & PAD_UP) {
    for ( i = board_height - 1; i >= 0; i--) {
      for( j = 0; j < board_width - 1 ; j++) {
        if(is_valid_pair(board[i][j], board[i][j+1])) return true;
      }
    }
  } else if (pad & PAD_DOWN) {
    for ( i = board_height - 1; i >= 0; i--) {
      for( j = board_width - 1; j >= 1 ; j--) {
        if(is_valid_pair(board[i][j], board[i][j-1])) return true;
      }
    }
  } else if (pad & PAD_LEFT) {
    for( j = 0; j < board_width ; j++) {
      for( i = 0; i < board_height - 1; i++) {
        if(is_valid_pair(board[i][j], board[i+1][j])) return true;
      }
    }
  } else if (pad & PAD_RIGHT) {
    for( j = 0; j < board_width ; j++) {
      for( i = board_height - 1; i >= 1 ; i--) {	
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

  for( i = 0; i < board_height; i++) {
    for( j = 0; j < board_width; j++) {
      if(board[i][j] == win_score) return GAME_WIN;	//TODO make this changable for harder modes
    }
  }
  //we have to check for continue after win, otherwise it often will find a 0 first.
  for( i = 0; i < board_height; i++) {
    for( j = 0; j < board_width; j++) {
      if(board[i][j] == 0) return GAME_CONTINUE;
    }
  }
     
  //no 0s on the board, and no 2048, so player loses
  return GAME_LOSE;
}

//let the player know they won/lost
void draw_winscreen(int state) {
  char str[9];
  memset(str, 0, sizeof(str));
  ppu_wait_frame();
  if(state == GAME_WIN)  sprintf(str, "WINNER!");
  if(state == GAME_LOSE) sprintf(str, "GAME OVER");
  vrambuf_put(NTADR_A(12,24), str, 9);
  sprintf(str, "PRESS START");
  vrambuf_put(NTADR_A(11,25), str, 11);
  vrambuf_flush();
}

void reset_gameboard() {
  int i, j;
  for( j = 0; j < board_width ; j++) {
    for( i = 0; i < board_height ; i++) {
      board[i][j] = 0;
    }
  }
}

void main(void)
{
  char pad;	// controller flags

  //run all the video setup routines
  setup_graphics();
    
  while(1) {
    
    //show the title screen and wait for input
    show_title_screen(game_title_pal, game_title_rle);
    while(1){
      pad = pad_poll(0);
      if(pad & PAD_START)	break;
      if(pad & PAD_SELECT) {
        show_options_page();
        fade_in();
      }
    }

    // initialize and draw the initial gameboard.
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
    //and reset the board once game is done
    reset_gameboard();
  }
}

