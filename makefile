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
	./tools/bin/png2spr dev/bitmaps/nomask.png output/NOMASK.SPR 16 16
	./tools/bin/png2spr dev/bitmaps/font.png output/FONT.SPR 8 8 -mask

.PHONY: tilemaps
tilemaps:
	mkdir -p output
	./tools/bin/tmx2bin output/LEVELS.DAT dev/maps/test.tmx 

assets: bitmaps tilemaps
