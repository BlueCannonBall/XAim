CC = gcc
CFLAGS = -Wall -s -O2 -lX11 -lXfixes -flto
TARGET = xaim
PREFIX = /usr/local

$(TARGET): main.c config.h
	$(CC) $< $(CFLAGS) -o $@

.PHONY: clean install

clean:
	$(RM) $(TARGET)

install:
	cp $(TARGET) $(PREFIX)/bin/
