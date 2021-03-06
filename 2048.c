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

//We can enable or disable the high score code and screens
// as we choose. (ie for hardware and emulators that don't support save)
// TODO: can we detect if we have the ability to save at runtime?
//#define ENABLE_HIGHSCORES

#define SCREEN_WIDTH  32
#define SCREEN_HEIGHT 30

//Import the pallet and startup screen
extern const byte game_title_pal[16];
extern const byte game_title_rle[];
extern const byte game_options_rle[];
extern const byte game_credits_rle[];
extern const byte board_bg_00[];

//some values to define win/loss/continue state
#define GAME_CONTINUE 0
#define GAME_LOSE 1
#define GAME_WIN 2

//Setup the initial state for all options:
//Win Score, max is based on we can only print 4 numers per tile
//TODO: can we surpass 2^13 with specially crafted number tiles to fit more?
// Short answer is yes, with 3 digits compressed to 2 tiles, but I have not made
//  a set that I like yet.
byte win_score = 11;	//2^11 = 2048
#define MAX_SCORE 13	//2^13 = 8192

//Board definitions
byte board_width = 4;
byte board_height = 4;
#define BOARD_MAX_WIDTH  8 //might be into the borders
#define BOARD_MIN_WIDTH  2 //does this make sense?
#define BOARD_MAX_HEIGHT 7 //might be into the borders
#define BOARD_MIN_HEIGHT 2 //does this make sense?
//the playboard array that stores the values(as 2^x)
byte board[BOARD_MAX_WIDTH][BOARD_MAX_HEIGHT];

//Visual settings
//not used yet
byte tile_style = 0;
byte pallette  = 0;
byte background = 0;


//a little debug function
void debug(char a) {
  char b[1];
  b[0] = a;
  vrambuf_put(NTADR_A(2,2),b,1);
  vrambuf_flush();
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
byte show_title_screen() {
  byte pad, i;
  #ifdef ENABLE_HIGHSCORES
    char buffer[11];
    #define MAIN_MENU_SIZE 4
  #else
    #define MAIN_MENU_SIZE 3
  #endif
  int selection = 0;
  int last_selection = 1;
  const byte addresses[MAIN_MENU_SIZE][2]= { 	//used to map selection to screen location
    {8,12}, 				// New Game
    {8,15}, 				// Options
    #ifdef ENABLE_HIGHSCORES
        {8,17}, 				// High Scores
    #endif
    {8,20} 				// Credits
  };	
  const char cursor[1] = { 0x16 };	// Card Diamond char
  const char blank[1]  = { 0x00 };	// Clear blank
  const byte debounce = 4;
  
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(game_title_pal);
  // unpack nametable into the VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(game_title_rle);
  
  //if we have enabled highscores, add the option to the page
  #ifdef ENABLE_HIGHSCORES
    sprintf(buffer, "High Scores");
    vrambuf_put(NTADR_A(10, 17), buffer, 11);
  #endif
  //set it to faded out before enabling
  pal_bright(0);
  // enable rendering
  ppu_on_all();
  // fade in from black
  fade_in();
  
  //draw first cursor
  vrambuf_put(NTADR_A(addresses[selection][0],addresses[selection][1]),cursor,1);
  vrambuf_flush();
  
  while (1) {
    ppu_wait_frame();
    pad = pad_poll(0);
    if (pad) {
      switch(pad) {
        case PAD_DOWN:
          last_selection = selection;
          selection++;
          if (selection > MAIN_MENU_SIZE - 1) selection = 0;
          break;
        case PAD_UP:
          last_selection = selection;
          selection--;
          if (selection < 0) selection = MAIN_MENU_SIZE - 1;
          break;
        case PAD_START:
        case PAD_A:
        case PAD_B:
          //delay a bit to eat the button press if going to another selection screen
          //debounce - delay a few frames
            for ( i = 0; i < debounce; i++ ) {
                  ppu_wait_frame();
            }
          #ifdef ENABLE_HIGHSCORES
            return (byte)selection;
          #else
            if(selection < 2) return (byte)selection;
            else return (byte)selection + 1;
          #endif
          break;
      }

      //update selection
      vrambuf_put(NTADR_A(addresses[selection][0],addresses[selection][1]),cursor,1);
      vrambuf_put(NTADR_A(addresses[last_selection][0],addresses[last_selection][1]),blank,1);
      vrambuf_flush();
      
      //debounce - delay a few frames
      for ( i = 0; i < debounce; i++ ) {
        ppu_wait_frame();
      }
    }
    //TODO: animate cursor? sprite or just screen udpate
  }
}

//bring up the options page and let uses select changes
//TODO: this does nothing right now
void show_options_page() {
  byte pad, i;
  char buffer[4];
  int selection = 0;
  int last_selection = 1;
  #define OPTIONS_MENU_SIZE 7
  const byte addresses[OPTIONS_MENU_SIZE][2]= { 	//used to map selection to screen location
    {20,4}, 				// Win Score
    {20,6}, 				// Board Width
    {20,7}, 				// Board Height
    {20,9}, 				// Tile Style
    {20,11},				// Tile Pallette
    {20,13},				// Background Selection
    {20,16}				// Return
  };	
  const char cursor[1] = { 0x16 };	// Card Diamond char
  const char blank[1]  = { 0x00 };	// Clear blank
  const byte debounce = 4;
  
  memset(buffer, 0, sizeof(buffer));
  
  ppu_off();
  pal_bg(game_title_pal);
  vram_adr(NAMETABLE_A);
  vram_unrle(game_options_rle);
  pal_bright(0);
  ppu_on_all();
  fade_in();
  
  //draw first cursor
  vrambuf_put(NTADR_A(addresses[selection][0],addresses[selection][1]),cursor,1);
  vrambuf_flush();
  
  while(1){
    ppu_wait_frame();
    pad = pad_poll(0);
    if (pad) {
      switch(pad) {
        case PAD_DOWN:
          last_selection = selection;
          selection++;
          if (selection > OPTIONS_MENU_SIZE - 1) selection = 0;
          break;
        case PAD_UP:
          last_selection = selection;
          selection--;
          if (selection < 0) selection = OPTIONS_MENU_SIZE - 1;
          break;
          
          
        case PAD_LEFT:
          switch (selection) {
            case 0:
              win_score--;
              if(win_score < 1) win_score = 1;
              break;
            case 1:
              board_width--;
              if(board_width < BOARD_MIN_WIDTH) board_width = BOARD_MIN_WIDTH;
              break;
            case 2:
              board_height--;
              if(board_height < BOARD_MIN_HEIGHT) board_height = BOARD_MIN_HEIGHT;
              break;
          }
          break;
        case PAD_RIGHT:
          switch (selection) {
            case 0:
              win_score++;
              if(win_score > MAX_SCORE) win_score = MAX_SCORE;
              break;
            case 1:
              board_width++;
              if(board_width > BOARD_MAX_WIDTH) board_width = BOARD_MAX_WIDTH;
              break;
            case 2:
              board_height++;
              if(board_height > BOARD_MAX_HEIGHT) board_height = BOARD_MAX_HEIGHT;
              break;
          }
          break;
          
          
        case PAD_START:
        case PAD_A:
        case PAD_B:
          if(selection == OPTIONS_MENU_SIZE - 1) {	//selected return
            return;
          }
          break;
        default:
          break;
      }
      
      //update selection
      vrambuf_put(NTADR_A(addresses[selection][0],addresses[selection][1]),cursor,1);
      vrambuf_put(NTADR_A(addresses[last_selection][0],addresses[last_selection][1]),blank,1);
      vrambuf_flush();
          
      //and update the text for the selected entry:
      switch(selection) {
        case 0:
          //update win score
          sprintf(buffer, "%d", 1 << win_score);
          vrambuf_put(NTADR_A(22,4),buffer,4);
          break;
        case 1:
          sprintf(buffer, "%d", board_width);
          vrambuf_put(NTADR_A(22,6),buffer,1);
          break;
        case 2:
          sprintf(buffer, "%d", board_height);
          vrambuf_put(NTADR_A(22,7),buffer,1);
          break;
        default:
          break;
      }
      
      //debounce - delay a few frames
      for ( i = 0; i < debounce; i++ ) {
            ppu_wait_frame();
      }
    }
  }
}

void show_credits() {
  byte pad, i;
  const byte debounce = 4;
  
  ppu_off();
  pal_bg(game_title_pal);
  vram_adr(NAMETABLE_A);
  vram_unrle(game_credits_rle);
  pal_bright(0);
  ppu_on_all();
  fade_in();
  
  while(1) {
    ppu_wait_frame();
    pad = pad_poll(0);
    //press any key to continue
    if (pad) {
      for ( i = 0; i < debounce; i++ ) {
            ppu_wait_frame();
      }
      return;
    }
  }
}

//add a block to the board
void add_block() {
  bool done = false;
  int v = rand() % 10 ? 1: 2;	//1 in 10 will be a 2^2 block
  
  while(!done) {
    int x = rand() % board_width;
    int y = rand() % board_height;
    
    if(board[x][y] == 0) {
      board[x][y] = v;
      done = true;
    }
  }
}

//init the board with some values
void init_gameboard() {
  //init random number generator, use a framecounter to make it more random
  memset(board, 0, sizeof(board));
  srand(nesclock());
  add_block();
  add_block();
}

// put the specified value in the x,y location requested on the playboard
void fill_tile(int x, int y, byte pow) {
  //calculate the offset for each direction
  byte nx = (x * 4) + (SCREEN_WIDTH  / 2) - (board_width  * 2) + 1;
  byte ny = (y * 4) + (SCREEN_HEIGHT / 2) - (board_height * 2) + 1;
  
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
      vals[0][0] = 0xB4;
      vals[0][1] = 0xB0;
      vals[1][0] = 0xB9;
      vals[1][1] = 0xB6;      
      break;
    case 13:		//8192
      vals[0][0] = 0xB8;
      vals[0][1] = 0xB1;
      vals[1][0] = 0xB9;
      vals[1][1] = 0xB2;      
      break;
    case 255:		//Debug - A
      vals[0][0] = 0x03;
      vals[0][1] = 0x03;
      vals[1][0] = 0x03;
      vals[1][1] = 0x41;      
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

void draw_blank_tile(byte screen_x, byte screen_y, byte tileset) {
  char buffer[4][4];
  byte y;
  //ignore tileset for now
  //TODO, store this as a set of bytes, maybe from an assembly file
  buffer[0][0] = 0xF8;	//upper left corner
  buffer[0][1] = 0xFF;	//top line
  buffer[0][2] = 0xFF;
  buffer[0][3] = 0xFA;	//upper right corner
  
  buffer[1][0] = 0xFC;	//left line
  buffer[1][1] = 0x03;	//filled blank
  buffer[1][2] = 0x03;
  buffer[1][3] = 0xFE;	//right line
  
  buffer[2][0] = 0xFC;	//left line
  buffer[2][1] = 0x03;	//filled blank
  buffer[2][2] = 0x03;
  buffer[2][3] = 0xFE;	//right line
  
  buffer[3][0] = 0xF9;	//lower left corner
  buffer[3][1] = 0xFD;	//bottom line
  buffer[3][2] = 0xFD;
  buffer[3][3] = 0xFB;	//lower right corner
  
  for( y = 0; y < 4; y++) {
    vrambuf_put(NTADR_A(screen_x,screen_y+y),buffer[y],4);
  }
  vrambuf_flush();
  buffer[0][0] = tileset;//TODO delete this, it's just to shut up the compiler for now
}

void draw_gameboard_bg() {
  byte x,y;
  byte start_x = SCREEN_WIDTH  / 2 - (board_width  * 2);
  byte start_y = SCREEN_HEIGHT / 2 - (board_height * 2);
  
  ppu_wait_frame();
  ppu_off();
  // unpack nametable into the VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(board_bg_00);
  pal_bright(0);
  
  ppu_on_all();
  
  //test
  draw_blank_tile((SCREEN_WIDTH / 2) - 2,(SCREEN_HEIGHT / 2) - 2,1);
  //draw_blank_tile(14,13,1);
  
  //draw the gameboard automatically
  for(   x = start_x; x < start_x + (board_width  * 4); x+=4 ){
    for( y = start_y; y < start_y + (board_height * 4); y+=4 ){
      draw_blank_tile(x,y,1);
    }
  }
  fade_in();
}

//take the current state and draw it
void draw_gameboard() {
  int x;
  int y;
  //char tmp[63];
  //We run this twice, as it is too many updates for the code otherwise
  //TODO: genericize this for larger boards.
  
  for( x = 0; x < board_width; x++) {
    for( y = 0; y < board_height; y++) {
      fill_tile(x, y, board[x][y]);
    }
    vrambuf_flush();
  }
  //debug code
  /*for( x = 0; x < board_width;  x++){
    for( y=0; y < board_height; y++){
      tmp[y*(board_width + 1) + x] = (char)(board[x][y] + 0x30);
    }
    tmp[x*(board_width + 1)+board_height]=0;
  }
  vrambuf_put(NTADR_A(1,1),tmp,63);
  vrambuf_flush();*/
}

//shift the numbers a direction, no merging of matched values
void move_shift(byte dir) {
  byte x = 0;
  byte y = 0;
  byte c = 0;
  
  switch (dir) {
    case PAD_UP: 
      //because we are using unsigned bytes here, can't use x >= 0 as it's always true
      //  instead we use x != 255 due to that being the wraparound
      for ( x = board_width - 1; x != 255; x--) {
        c = 0;
        for( y = 0; y < board_height ; y++) {	
          if (board[x][y] != 0) {
            board[x][c] = board[x][y];
            c++;
          }
        }
        for( y = c; y < board_height; y++) {
          board[x][y] = 0;
        }
      }
      break;
      
    case PAD_DOWN:
      for ( x = board_width - 1; x != 255; x--) {
        c = 0;
        for( y = board_height - 1; y != 255 ; y--) {	
          if (board[x][y] != 0) {
            board[x][board_height - 1 - c] = board[x][y];
            c++;
          }
        }
        for( y = board_height - 1 - c; y != 255; y--) {
          board[x][y] = 0;
        }
      }
      break;
      
    case PAD_LEFT:
      for( y = 0; y < board_height ; y++) {
        c = 0;
        for( x = 0; x < board_width ; x++) {
          if (board[x][y] != 0) {
            board[c][y] = board[x][y];
            c++;
          }
        }
        for( x = c; x < board_width; x++) {
          board[x][y] = 0;
        }
      }
      break;
      
    case PAD_RIGHT:
      for ( y = board_height - 1; y != 255; y--) {
        c = 0;
        for( x = board_width - 1; x != 255 ; x--) {	
          if (board[x][y] != 0) {
            board[board_width - 1 - c][y] = board[x][y];
            c++;
          }
        }
        for( x = board_width - 1 - c; x != 255; x--) {
          board[x][y] = 0;
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
  byte x = 0;
  byte y = 0;
  switch (dir) {
    case PAD_UP: 
      for ( x = board_width - 1; x != 255; x--) {
        for( y = 0; y < board_height - 1 ; y++) {
          if((board[x][y] == board[x][y + 1]) && (board[x][y] !=0)) {
            board[x][y]    += 1;
            board[x][y + 1] = 0;
          }
        }
      }
      break;
    case PAD_DOWN:
      for ( x = board_width - 1; x != 255; x--) {
        for( y = board_height - 1; y >= 1 ; y--) {
          if((board[x][y] == board[x][y - 1]) && (board[x][y] !=0)){
            board[x][y]    += 1;
            board[x][y - 1] = 0;
          }
        }
      }
      break;
      
    case PAD_LEFT:
      for( y = 0; y < board_height ; y++) {
        for( x = 0; x < board_width - 1; x++) {
          if((board[x][y] == board[x + 1][y]) && (board[x][y] !=0)){
            board[x][y]    += 1;
            board[x + 1][y] = 0;
          }
        }
      }
      break;
      
    case PAD_RIGHT:
      for( y = 0; y < board_height ; y++) {
        for( x = board_width - 1; x >= 1 ; x--) {	
          if((board[x][y] == board[x - 1][y]) && (board[x][y] !=0)){
            board[x][y]    += 1;
            board[x - 1][y] = 0;
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
  byte x = 0;
  byte y = 0;

  if (pad & PAD_UP) {
    for ( x = board_width - 1; x != 255; x--) {
      for( y = 0; y < board_height - 1 ; y++) {
        if(is_valid_pair(board[x][y], board[x][y+1])) return true;
      }
    }
  } else if (pad & PAD_DOWN) {
    for ( x = board_width - 1; x != 255; x--) {
      for( y = board_height - 1; y >= 1 ; y--) {
        if(is_valid_pair(board[x][y], board[x][y-1])) return true;
      }
    }
  } else if (pad & PAD_LEFT) {
    for( y = 0; y < board_height ; y++) {
      for( x = 0; x < board_width - 1; x++) {
        if(is_valid_pair(board[x][y], board[x+1][y])) return true;
      }
    }
  } else if (pad & PAD_RIGHT) {
    for( y = 0; y < board_height ; y++) {
      for( x = board_width - 1; x >= 1 ; x--) {	
        if(is_valid_pair(board[x][y], board[x-1][y])) return true;
      }
    }
  }
  return false; 
}

//check the status of the game
bool game_win_lose() {
  //brute force, just roll over all values and check for 2048 and 0s
  //TODO: allow one last move?
  byte x = 0;
  byte y = 0;
  bool win = false;
  bool lose = false;

  for( x = 0; x < board_width; x++) {
    for( y = 0; y < board_height; y++) {
      if(board[x][y] == win_score) return GAME_WIN;	//TODO make this changable for harder modes
    }
  }
  //we have to check for continue after win, otherwise it often will find a 0 first.
  for( x = 0; x < board_width; x++) {
    for( y = 0; y < board_height; y++) {
      if(board[x][y] == 0) return GAME_CONTINUE;
    }
  }
     
  //no 0s on the board, and no 2048, so player loses
  return GAME_LOSE;
}

//let the player know they won/lost
void draw_winscreen(int state) {
  byte pad;
  char buf[16];
  byte x;
   
  ppu_wait_frame();
  
  //we zero out the buffer
  memset(buf, 0, sizeof(buf));
   
  //and since we want to use pallet 0 to draw our winscreen, 
  // we just use the 0's in the buffer to update the attribute table
  vrambuf_put(NAMETABLE_A + 0x03DA, buf, 4);
  vrambuf_put(NAMETABLE_A + 0x03E2, buf, 4); 
  
  //now draw the box for the text:
  //top line
  memset(buf, 0xFF, sizeof(buf));
  buf[0]  = 0xF8;
  buf[15] = 0xFA;
  vrambuf_put(NTADR_A(8,12),buf,16);
  //sides of the box
  memset(buf, 0x00, sizeof(buf));
  buf[0]  = 0xFC;
  buf[15] = 0xFE;
  for( x = 13 ; x <= 15; x++ ) {
    vrambuf_put(NTADR_A(8,x),buf,16);
  }
  vrambuf_flush();	//buffer is full by now
  for( x = 16 ; x <= 18; x++ ) {
    vrambuf_put(NTADR_A(8,x),buf,16);
  }
  //bottom line
  memset(buf, 0xFD, sizeof(buf));
  buf[0]  = 0xF9;
  buf[15] = 0xFB;
  vrambuf_put(NTADR_A(8,19),buf,16);
  vrambuf_flush();	//buffer is full again
    
  memset(buf, 0, sizeof(buf));
  if(state == GAME_WIN)  sprintf(buf, "  WINNER !  ");
  if(state == GAME_LOSE) sprintf(buf, " GAME  OVER ");
  vrambuf_put(NTADR_A(10,14), buf, 12);
  sprintf(buf, "PRESS  START");
  vrambuf_put(NTADR_A(10,17), buf, 12);
  vrambuf_flush();
  
  while(1) {
    ppu_wait_frame();
    pad = pad_poll(0);
    //press any key to continue
    if (pad == PAD_START) {
      debug(0x62);
      return;
    }
  }
}

void reset_gameboard() {
  byte x, y;
  for( x = 0; x < board_width ; x++) {
    for( y = 0; y < board_height ; y++) {
      board[x][y] = 0;
    }
  }
}

void play_game() {
  byte pad;
  byte state;

  // initialize and draw the initial gameboard.
  init_gameboard();
  draw_gameboard_bg();
  draw_gameboard();

  while (1) {
    ppu_wait_frame();
    pad = pad_poll(0);	//TODO: on some emulators(FCEUX) this can sometimes act up.
    
    //TODO: make sure we can do one last move?
    state = game_win_lose();
    
    //TODO: fix this state logic block somehow....
    if(state != 0) {
      draw_winscreen(state);
    } else {
      if(is_valid_move(pad)) move_gameboard(pad);
      state = game_win_lose();
      if(state != 0) {
        draw_winscreen(state);
        return;
      }
    }
    while (!pad_trigger(0)) {
      ppu_wait_nmi();
    }
  }
}

void main(void)
{
  //byte pad;	// controller flags
  byte return_code;

  //run all the video setup routines
  setup_graphics();
    
  while(1) {
    return_code = 255;
    while(return_code != 0) {
      //show the title screen 
      return_code = show_title_screen();
      switch(return_code) {
        case 1:
          show_options_page();
          break;
          
        case 2:
          //high scores
          //not yet implemented
          break;
          
        case 3:
          show_credits();
          break;
          
        default:
          //something broke - maybe try going and playing
          return_code = 0;
          break;
      }
    }
    
    play_game();
    
    //and reset the board once game is done
    reset_gameboard();
  }
}

