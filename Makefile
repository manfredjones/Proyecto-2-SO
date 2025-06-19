CC = gcc
CFLAGS = -Wall `pkg-config fuse --cflags`
LDFLAGS = `pkg-config fuse --libs`

SRC = src
BIN = bin

all: | $(BIN) $(BIN)/mkfs.bwfs $(BIN)/fsck.bwfs $(BIN)/mount.bwfs

$(BIN):
	mkdir -p $(BIN)

$(BIN)/mkfs.bwfs: $(SRC)/mkfs.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN)/fsck.bwfs: $(SRC)/fsck.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN)/mount.bwfs: $(SRC)/mount.c $(SRC)/bwfs.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BIN)/*
