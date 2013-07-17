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

#define COLDISTX 8 // Player-Box collision extent
#define COLDISTY 7 

#define CELL_NORMAL 0
#define CELL_WHITE  1
#define CELL_BLACK  2
#define CELL_CBLACK 3

#define START_DELAY 70
#define MIN_DELAY 20

BYTE time;
BYTE nextUpdate;
BYTE score;

UBYTE pposx, pposy;
UBYTE pframe, pdir;
BYTE switch_delay;
UBYTE alive;

UBYTE boxx, boxy;
UBYTE boxcx, boxcy;

UBYTE map[MAPW*MAPH];

unsigned char normal_cell_tiles[] = { 32, 34, 33, 35 };
unsigned char white_cell_tiles[] = { 36, 38, 37, 39 };
unsigned char black_cell_tiles[] = { 40, 42, 41, 43 };

void init();
void showTitle();
void clearMap();
void updateScore();
void addBox();
void spawnCells();
void updateMap();
void updatePlayer();
void gameLoop();
