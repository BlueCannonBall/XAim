CC = gcc
CFLAGS = -s -Ofast -lX11 -lXfixes
TARGET = xaim
PREFIX = /usr/local

$(TARGET): main.c config.h
	$(CC) $< $(CFLAGS) -o $@

.PHONY: clean install

clean:
	$(RM) $(TARGET)

install:
	cp $(TARGET) $(PREFIX)/bin/