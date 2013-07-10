#include <gb/gb.h>
#include <rand.h>
#include "binconst.h"
#include "bg_data.h"
#include "sprite_data.h"
#include "map_tiles.h"
#include "title_tiles.h"

#define MAPW 8
#define MAPH 6

#define PMINY 18
#define PMAXY 111
#define PMINX 20
#define PMAXX 140

#define DIR_UP 	  0
#define DIR_RIGHT 1
#define DIR_DOWN  2
#define DIR_LEFT  3 

#define HOLE_DELAY 50
#define WHITE_TIME 50
#define BLACK_TIME 100

UBYTE time;

UBYTE pposx, pposy;
UBYTE pframe, pdir;

UBYTE map[MAPW*MAPH];

UBYTE nextHole;

unsigned char normal_cell_tiles[] = { 32, 34, 33, 35 };
unsigned char white_cell_tiles[] = { 36, 38, 37, 39 };
unsigned char black_cell_tiles[] = { 40, 42, 41, 43 };

/**
 * Initializes game.
 * Loads tile data for BG and OBJ layers.
 */
void init() {
	disable_interrupts();
	DISPLAY_OFF;

	/*
	 * LCD off
	 * Window tile map = 9600
	 * Window display off
	 * BG & window data 8800
	 * BG tile map 9800
	 * OBJ size = 8x8
	 * OBJ display on
	 * BG display on
	 */
	LCDC_REG = B8(01000011);

	BGP_REG = OBP1_REG = B8(11100100);
	OBP0_REG = B8(11100000);

	// Load tile data sets
	set_bkg_data(0, bg_dataLen, bg_data);
	set_sprite_data(0, sprite_dataLen, sprite_data);

	DISPLAY_ON;
	enable_interrupts();
}

void showTitle() {
	UWORD seed;

	// Load titlescreen and seed RNG
	DISPLAY_OFF;
	set_bkg_tiles(0, 0, 20, 18, title_tiles);
	DISPLAY_ON;

	waitpad(J_START);
	seed = DIV_REG;

	waitpadup();
	seed |= (UWORD)DIV_REG << 8;
	initarand(seed);
}

/**
 * Clears game map zeroing all cells.
 */
void clearMap() {
	UBYTE i = 0;
	for(i = 0; i < MAPW*MAPH; ++i) {
		map[i] = 0;
	}
}

/**
 * Updates map and triggers events when appropriate.
 */
void updateMap() {
	UBYTE x, y;

	nextHole++;
	// Add white cell
	if(nextHole >= HOLE_DELAY) {
		nextHole = 0;
		x = (UBYTE)rand() % MAPW;
		y = (UBYTE)rand() % MAPH;
		map[x + y*MAPW] = 1;
		set_bkg_tiles(2+x*2, 2+y*2, 2, 2, white_cell_tiles);
	}

	for(y = 0; y < MAPH; ++y) {
		for(x = 0; x < MAPW; ++x) {
			if(map[x + y*MAPW] > 0) {
				map[x + y*MAPW]++;
				if(map[x + y*MAPW] > BLACK_TIME) {
					map[x + y*MAPW] = 0;
					set_bkg_tiles(2+x*2, 2+y*2, 2, 2, normal_cell_tiles);
				} else if(map[x + y*MAPW] > WHITE_TIME) {
					set_bkg_tiles(2+x*2, 2+y*2, 2, 2, black_cell_tiles);
				}
			}
		}
	}
}

/**
 * Updates player movement and animation.
 */
void updatePlayer() {
	UBYTE i, moved;

	moved = 0;
	// Get joypad input
	i = joypad();
	if(i & J_RIGHT) {
		pposx += 1;
		pdir = DIR_RIGHT;
		moved = 1;
	}
	if(i & J_LEFT) {
		pposx -= 1;
		pdir = DIR_LEFT;
		moved = 1;
	}
	if(i & J_UP) {
		pposy -= 1;
		pdir = DIR_UP;
		moved = 1;
	}
	if(i & J_DOWN) {
		pposy += 1;
		pdir = DIR_DOWN;
		moved = 1;
	}

	// Keep player within bounds
	if(pposx < PMINX) pposx = PMINX;
	else if(pposx > PMAXX) pposx = PMAXX;
	if(pposy < PMINY) pposy = PMINY;
	else if(pposy > PMAXY) pposy = PMAXY;

	// Set player position
	move_sprite(0, pposx,   pposy);
	move_sprite(1, pposx,   pposy+8);
	move_sprite(2, pposx+8, pposy);
	move_sprite(3, pposx+8, pposy+8);

	// Set player sprite
	if(moved) {
		if(time % 8 == 0) {
			pframe = (pframe + 1) % 4;
		}
	} else {
		pframe = 1;
	}

	i = (pdir << 4) + (pframe << 2);
	set_sprite_tile(0, i);
	set_sprite_tile(1, i+1);
	set_sprite_tile(2, i+2);
	set_sprite_tile(3, i+3);
}

void main() {
	init();

	showTitle();

	// Init player
	pposx = 80;
	pposy = 64;
	pdir = DIR_DOWN;

	nextHole = 0;

	// Load game map BG tiles
	DISPLAY_OFF;
	set_bkg_tiles(0, 0, 20, 18, map_tiles);
	DISPLAY_ON;

	// Game loop
	while(1) {
		wait_vbl_done();
		time++;
		updateMap();
		updatePlayer();
	}
}
