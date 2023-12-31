SRC_FILES := $(wildcard src/*.c)
HDR_FILES := $(wildcard src/*.h)
LDFLAGS := -lopenslide -lm -lpthread
TESTLDFLAGS := -lopenslide -lm -lev -lunistring -lpthread
CXXFLAGS := -Wall -Wextra -pedantic -std=c99 -O3 -pg

# --- wsi-tvx ---

bin/wsi-tvx: $(SRC_FILES) $(HDR_FILES)
	$(CC) $(SRC_FILES) -o bin/wsi-tvx $(LDFLAGS) $(CXXFLAGS)

# --- tests ---

tests/bin/test-kitty: tests/test-kitty.c $(SRC_FILES) $(HDR_FILES)
	$(CC) tests/test-kitty.c $(SRC_FILES) -o tests/bin/test-kitty $(TESTLDFLAGS) $(CXXFLAGS)

tests/bin/test-base64: tests/test-base64.c $(SRC_FILES) $(HDR_FILES)
	$(CC) tests/test-base64.c $(SRC_FILES) -o tests/bin/test-base64 $(TESTLDFLAGS) $(CXXFLAGS)

tests/bin/test-tiles: tests/test-tiles.c $(SRC_FILES) $(HDR_FILES)
	$(CC) tests/test-tiles.c $(SRC_FILES) -o tests/bin/test-tiles $(TESTLDFLAGS) $(CXXFLAGS)

tests/bin/test-slide: tests/test-slide.c $(SRC_FILES) $(HDR_FILES)
	$(CC) tests/test-slide.c $(SRC_FILES) -o tests/bin/test-slide $(TESTLDFLAGS) $(CXXFLAGS)

tests/bin/test-term: tests/test-term.c $(SRC_FILES) $(HDR_FILES)
	$(CC) tests/test-term.c $(SRC_FILES) -o tests/bin/test-term $(TESTLDFLAGS) $(CXXFLAGS)

test:
		make tests/bin/test-kitty
		make tests/bin/test-base64
		make tests/bin/test-tiles
		make tests/bin/test-slide
		make tests/bin/test-term

install:
		cp ./bin/wsi-tvx $(HOME)/.local/bin/wsi-tvx


# --- cleanup ---

clean:
	rm bin/wsi-tvx
	rm tests/bin/test-*
