NAME := wsi-tvx
SRC_DIR := src
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
HDR_FILES := $(wildcard $(SRC_DIR)/*.h)
LDFLAGS := -lopenslide -lm -lev
CXXFLAGS := -Wall -Wextra -pedantic -std=c99 -O3

$(NAME): $(SRC_FILES) $(HDR_FILES) Makefile
	$(CC) $(SRC_FILES) -o $(BIN_DIR)/$(NAME) $(LDFLAGS) $(CXXFLAGS)

clean:
	rm $(BIN_DIR)/$(NAME)
