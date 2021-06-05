.PHONY: png2spr
png2spr:
	mkdir -p ./tools/bin
	gcc ./tools/src/$@.c -o ./tools/bin/$@ -lm

.PHONY: bitmaps
bitmaps:
	mkdir -p output
	./tools/bin/png2spr dev/bitmaps/test1.png output/TEST1.SPR 16 16 -mask
