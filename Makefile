# SteadyFeet Makefile
# type 'make' to compile
# 'make clean' to remove compilation results (ie. for packaging)

# Directories where to install stuff
# (notice that Installing is not necessary!)
PREFIX=/usr

DATA_PREFIX=$(PREFIX)/share/steadyfeet
BIN_PREFIX=$(PREFIX)/bin
DOCS_PREFIX=$(DATA_PREFIX)/docs
ICON_PREFIX=$(PREFIX)/share/icons
APPS_PREFIX=$(PREFIX)/applications

CC = g++
CXXFLAGS = -Wall -g `sdl-config --cflags` -DDATA_PREFIX=\"$(DATA_PREFIX)\"
LIBS  = `sdl-config --libs`
SRC = src/main.cpp src/gameloop.cpp src/board.cpp src/player.cpp src/background.cpp \
      src/game_session.cpp src/leveleditor.cpp src/title.cpp src/highscore.cpp \
      src/lib/surface.cpp src/lib/text.cpp src/lib/timer.cpp src/lib/screen.cpp \
      src/lib/file_access.cpp src/lib/menu.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = steadyfeet

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJ) core* *~ */*~ */*/*~

install:
	mkdir -p $(DATA_PREFIX)
	mkdir -p $(DATA_PREFIX)/graphics
	mkdir -p $(DATA_PREFIX)/levels
	mkdir -p $(DOCS_PREFIX)
	install ./data/graphics/* $(DATA_PREFIX)/graphics
	install ./data/levels/* $(DATA_PREFIX)/levels
	install COPYING INSTALL README $(DOCS_PREFIX)
	install steadyfeet $(BIN_PREFIX)
	install data/graphics/icon.xpm $(ICON_PREFIX)/steadyfeet.xpm
	install steadyfeet.desktop $(APPS_PREFIX)

uninstall:
	rm -rf $(DATA_PREFIX)
	rm -f  $(BIN_PREFIX)/steadyfeet
	rm -rf $(DOCS_PREFIX)
	rm -f  $(ICON_PREFIX)/steadyfeet.xpm
	rm -f  $(APPS_PREFIX)/steadyfeet.desktop
