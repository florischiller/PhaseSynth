#
# Kurzanleitung
# =============
#
# make		-- Baut den Kernel.
#
# make install	-- Baut den Kernel und transferiert ihn auf den Server.
# 		   Das Board holt sich diesen Kernel beim nächsten Reset.
#
# make clean	-- Löscht alle erzeugten Dateien.
#

#
# Quellen
#
LSCRIPT = kernel.lds
OBJ = src/synth.o
OBJ += src/utils/util.o
OBJ += src/oscillator/osc.o
OBJ += src/audio/audio.o
OBJ += src/control/control.o

#
# Konfiguration
CC = gcc

INCLUDE = -I/usr/local/include/ -L/usr/local/lib

CFLAGS = -Wall -Wextra

LIBS = -lrt -lm -lasound -lpthread -lportaudio

GTKFLAGS = `pkg-config --cflags --libs gtk+-2.0`


#
# Regeln
#

%.o: %.c
	$(CC) $(CFLAGS) $(GTKFLAGS) -c $< -o $@

.PHONY: install
install: $(OBJ) $(LIB)
	$(CC) $(INCLUDE) $(CFLAGS) -o main $(OBJ) $(GTKFLAGS) $(LIB) $(LIBS)

.PHONY: debug
debug: $(OBJ) $(LIB)
	$(CC) $(INCLUDE) $(CFLAGS) -g -o main $(OBJ) $(LIB) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ)
	rm -f $(LIB)
