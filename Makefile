CC=lcc

avoider.gb: avoider.o map.o sprite_data.o bg_data.o
	$(CC) avoider.o map.o sprite_data.o bg_data.o -o avoider.gb

avoider.o: avoider.c map.h bg_data.h sprite_data.h
	$(CC) -c avoider.c

map.o: map.c
	$(CC) -c map.c

sprite_data.o: sprite_data.c
	$(CC) -c sprite_data.c

bg_data.o: bg_data.c
	$(CC) -c bg_data.c

.PHONY: run

run:
	gambatte-qt avoider.gb
