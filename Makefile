CC=lcc

avoider.gb: avoider.o sprite_data.o bg_data.o map_tiles.o title_tiles.o
	$(CC) avoider.o sprite_data.o bg_data.o map_tiles.o title_tiles.o -o avoider.gb

avoider.o: avoider.c map_tiles.h bg_data.h sprite_data.h
	$(CC) -c avoider.c

sprite_data.o: sprite_data.c
	$(CC) -c sprite_data.c

bg_data.o: bg_data.c
	$(CC) -c bg_data.c

map_tiles.o: map_tiles.c
	$(CC) -c map_tiles.c

title_tiles.o: title_tiles.c
	$(CC) -c title_tiles.c

.PHONY: run

run:
	gambatte-qt avoider.gb
