.PHONY: png2spr
png2spr:
	mkdir -p ./tools/bin
	gcc ./tools/src/$@.c -o ./tools/bin/$@ -lm

.PHONY: tmx2bin
tmx2bin:
	mkdir -p ./tools/bin
	gcc ./tools/src/$@.c -o ./tools/bin/$@ -lm

tools: png2spr tmx2bin

.PHONY: bitmaps
bitmaps:
	mkdir -p output
	./tools/bin/png2spr dev/bitmaps/sprites.png output/SPRITES.SPR 16 16 -mask
	./tools/bin/png2spr dev/bitmaps/tileset.png output/TILESET.SPR 16 16
	./tools/bin/png2spr dev/bitmaps/font.png output/FONT.SPR 8 8 -mask

.PHONY: tilemaps
tilemaps:
	mkdir -p output
	./tools/bin/tmx2bin output/LEVELS1.DAT dev/maps/pack1/*.tmx 
	./tools/bin/tmx2bin output/LEVELS2.DAT dev/maps/pack2/*.tmx 
	./tools/bin/tmx2bin output/LEVELS3.DAT dev/maps/pack3/*.tmx 

assets: bitmaps tilemaps

run:
	(cd output; dosbox game.exe)
