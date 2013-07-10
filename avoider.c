#include <gb/gb.h>
#include "binconst.h"
#include "map.h"
#include "bg_data.h"
#include "sprite_data.h"

#define PMINY 18
#define PMAXY 127
#define PMINX 20
#define PMAXX 140

#define DIR_UP 	  0
#define DIR_RIGHT 1
#define DIR_DOWN  2
#define DIR_LEFT  3 

UBYTE time;

UBYTE pposx, pposy;
UBYTE pframe, pdir;

void updatePlayer() {
	UBYTE i;

	// Get joypad input
	i = joypad();
	if(i & J_RIGHT) {
		pposx += 1;
		pdir = DIR_RIGHT;
	}
	if(i & J_LEFT) {
		pposx -= 1;
		pdir = DIR_LEFT;
	}
	if(i & J_UP) {
		pposy -= 1;
		pdir = DIR_UP;
	}
	if(i & J_DOWN) {
		pposy += 1;
		pdir = DIR_DOWN;
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
	if(time % 8 == 0)
		pframe = (pframe + 1) % 4;

	i = pdir*16 + pframe*4;
	set_sprite_tile(0, i);
	set_sprite_tile(1, i+1);
	set_sprite_tile(2, i+2);
	set_sprite_tile(3, i+3);
}

void main() {
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
	set_bkg_data(0, 44, bg_data);
	set_sprite_data(0, 65, sprite_data);

	// Load game map BG tiles
	set_bkg_tiles(0, 0, 20, 18, map);

	// Init player
	pposx = 24;
	pposy = 24;

	DISPLAY_ON;
	enable_interrupts();

	// Game loop
	while(1) {
		wait_vbl_done();
		time++;
		updatePlayer();
	}
}
