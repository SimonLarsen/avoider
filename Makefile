CC=lcc

avoider.gb: avoider.o sprite_data.o title_data.o game_data.o map_tiles.o title_tiles.o
	$(CC) avoider.o sprite_data.o title_data.o game_data.o map_tiles.o title_tiles.o -o avoider.gb

avoider.o: avoider.c sprite_data.h title_data.h game_data.h map_tiles.h title_tiles.h
	$(CC) -c avoider.c

sprite_data.o: sprite_data.c
	$(CC) -c sprite_data.c

title_data.o: title_data.c
	$(CC) -c title_data.c

game_data.o: game_data.c
	$(CC) -c game_data.c

map_tiles.o: map_tiles.c
	$(CC) -c map_tiles.c

title_tiles.o: title_tiles.c
	$(CC) -c title_tiles.c

.PHONY: run

run:
	gambatte-qt avoider.gb
