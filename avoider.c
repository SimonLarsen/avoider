#include <gb/gb.h>
#include <rand.h>
#include "binconst.h"
#include "title_data.h"
#include "game_data.h"
#include "sprite_data.h"
#include "map_tiles.h"
#include "title_tiles.h"

#include "avoider.h"

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

	DISPLAY_ON;
	enable_interrupts();
}

/**
 * Draws the title screen
 * and halts until START is pushed.
 */
void showTitle() {
	UWORD seed;

	// Load titlescreen and seed RNG
	DISPLAY_OFF;
	LCDC_REG = B8(01000001);
	set_bkg_data(0, title_dataLen, title_data);
	set_bkg_tiles(0, 0, 20, 18, title_tiles);
	DISPLAY_ON;

	waitpad(J_START);
	seed = DIV_REG;

	waitpadup();
	seed |= (UWORD)DIV_REG << 8;
	initarand(seed);
}

/**
 * Clears game map by zeroing all cells.
 */
void clearMap() {
	UBYTE i = 0;
	for(i = 0; i < MAPW*MAPH; ++i) {
		map[i] = 0;
	}
}

/**
 * Updates the score counter sprites
 * in the HUD
 */
void updateScore() {
	set_sprite_tile(6, 65 + score / 10);
	set_sprite_tile(7, 65 + score % 10);
}

/**
 * Moves the box to a random position.
 */
void addBox() {
	boxcx = (UBYTE)rand() % MAPW;
	boxcy = (UBYTE)rand() % MAPH;
	boxx = boxcx * 16 + 24;
	boxy = boxcy * 16 + 28;

	move_sprite(4, boxx+4, boxy+8);
}

/**
 * Adds a row or column of white cells.
 */
void spawnCells() {
	BYTE x, y, ix, iy;
	// Add white cell
	// Random event
	x = (UBYTE)rand() % 2;
	// Horizontal line
	if(x == 0) {
		y = (UBYTE)rand() % MAPH;
		ix = 2;
		iy = 2 + (y << 1);
		for(x = 0; x < MAPW; ++x) {
			map[x + (y << 3)] = CELL_WHITE; //map[x + y*MAPW] = 1;
			set_bkg_tiles(ix, iy, 2, 2, white_cell_tiles);
			ix += 2;
		}
	}
	// Vertical line
	else {
		x = (UBYTE)rand() % MAPW;
		ix = 2 + (x << 1);
		iy = 2;
		for(y = 0; y < MAPH; ++y) {
			map[x + (y << 3)] = CELL_WHITE; //map[x + y*MAPW] = 1;
			set_bkg_tiles(ix, iy, 2, 2, white_cell_tiles);
			iy += 2;
		}
	}
}

/**
 * Updates all white cells to be black and removes
 * black cells.
 */
void updateMap() {
	UBYTE x, y, i, ix, iy, black_exp;

	black_exp = switch_delay << 1;
	// Check cell timers
	iy = 2;
	for(y = 0; y < MAPH; ++y) {
		ix = 2;
		for(x = 0; x < MAPW; ++x) {
			i = x + (y<<3); // x + (y*MAPW)
			// Switch white cells
			if(map[i] == CELL_WHITE) {
				map[i] = CELL_BLACK;
				set_bkg_tiles(ix, iy, 2, 2, black_cell_tiles);
			}
			else if(map[i] == CELL_BLACK) {
				map[i] = CELL_NORMAL;
				set_bkg_tiles(ix, iy, 2, 2, normal_cell_tiles);
			}
			ix += 2;
		}
		iy += 2;
	}
}

/**
 * Updates player movement and animation.
 */
void updatePlayer() {
	UBYTE i, moved;

	// Get joypad input
	moved = 0;
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
	set_sprite_tile(0, i++);
	set_sprite_tile(1, i++);
	set_sprite_tile(2, i++);
	set_sprite_tile(3, i);
}

/**
 * Main ingame loop.
 */
void gameLoop() {
	BYTE xdist, ydist;
	UBYTE cx, cy;

	// Init player
	pposx = 80;
	pposy = 64;
	pdir = DIR_DOWN;
	alive = 1;

	// Set game variables
	score = 0;
	nextUpdate = 0;
	switch_delay = START_DELAY;

	// Load game map BG tiles
	DISPLAY_OFF;
	// Load tile data sets
	LCDC_REG = B8(01000011);
	set_sprite_data(0, sprite_dataLen, sprite_data);
	set_bkg_data(0, game_dataLen, game_data);
	set_bkg_tiles(0, 0, 20, 18, map_tiles);

	// Score counter
	set_sprite_tile(5, 64);
	move_sprite(5, 15, 148);

	set_sprite_tile(6, 65);
	set_sprite_tile(7, 65);
	move_sprite(6, 26, 148);
	move_sprite(7, 35, 148);

	set_sprite_tile(4, 64);

	clearMap();
	addBox();

	DISPLAY_ON;


	// Game loop
	while(alive) {
		wait_vbl_done();
		time++;
		nextUpdate++;
		if(nextUpdate >= switch_delay) {
			nextUpdate = 0;
			updateMap();
			spawnCells();
		}
		updatePlayer();

		// Check if player has picked up box
		xdist = pposx - boxx;
		ydist = pposy - boxy;
		if(xdist > -COLDISTX && xdist < COLDISTX
		&& ydist > -COLDISTY && ydist < COLDISTY) {
			score++;
			updateScore();
			addBox();
			switch_delay--;
		}

		// Check if player is on black hole
		cx = pposx/16 - 1;
		cy = pposy/16 - 1;
		if(map[cx + cy*MAPW] >= CELL_BLACK) {
			alive = 0;
		}
	}
}

void main() {
	init();

	while(1) {
		showTitle();
		gameLoop();
	}
}
