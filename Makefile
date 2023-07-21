NAME := wsi-tvx
SRC_DIR := src
BIN_DIR := bin
TEST_DIR := tests
TEST_BIN_DIR := tests/bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
HDR_FILES := $(wildcard $(SRC_DIR)/*.h)
TEST_FILES := $(wildcard $(TEST_DIR)/*.c)
LDFLAGS := -lopenslide -lm -lev
CXXFLAGS := -Wall -Wextra -pedantic -std=c99 -O3 -pg

$(NAME): main.c $(SRC_FILES) $(HDR_FILES)
	$(CC) main.c $(SRC_FILES) -o $(BIN_DIR)/$(NAME) $(LDFLAGS) $(CXXFLAGS)

test: $(TEST_FILES) $(SRC_FILES) $(HDR_FILES)
	$(CC) $(TEST_DIR)/test-base64.c $(SRC_FILES) -o $(TEST_BIN_DIR)/test-base64 $(LDFLAGS) $(CXXFLAGS)
	# $(CC) $(TEST_DIR)/test-tiles.c $(SRC_FILES) -o $(TEST_BIN_DIR)/test-tiles $(LDFLAGS) $(CXXFLAGS)
	# $(CC) $(TEST_DIR)/test-slide.c $(SRC_FILES) -o $(TEST_BIN_DIR)/test-slide $(LDFLAGS) $(CXXFLAGS)
	# $(CC) $(TEST_DIR)/test-term.c $(SRC_FILES) -o $(TEST_BIN_DIR)/test-term $(LDFLAGS) $(CXXFLAGS)

clean:
	rm $(BIN_DIR)/$(NAME)
	rm $(TEST_BIN_DIR)/test-*
