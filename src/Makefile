# game name
GAME_NAME=alienBlaster

# the compiler to use
COMPILER=g++

# include path
INCLUDE_PATH=-I.

INSTALL_DIR = install -d
INSTALL_FILES = install -D

#OPTIMIZATION=-g -pg -fprofile-arcs
#OPTIMIZATION=-O3
OPTIMIZATION=-g
#OPTIMIZATION=
DATADIR ?= "/usr/local/share/alienblaster"

# SDL library
SDL_LIBS=$(shell sdl-config --libs)
SDL_FLAGS=$(shell sdl-config --cflags)

# game flags
GAME_FLAGS=-D_GNU_SOURCE -Wall -Winline -finline-functions $(SDL_FLAGS) \
	 $(OPTIMIZATION) -DDATA_PREFIX='$(DATADIR)'

GAME_LIBS=-lSDL_mixer $(SDL_LIBS) $(OPTIMIZATION)

# all objectfiles
OBJECT_FILES=main.o surfaceDB.o soundDB.o options.o geometry.o video.o game.o \
	racer.o racers.o shots.o shot.o boundingBox.o items.o item.o font.o \
	explosion.o explosions.o mixer.o enemys.o enemy.o wrecks.o wreck.o \
	settings.o intro.o setDifficulty.o global.o formation.o infoscreen.o \
	menuArcadeMode.o sonic.o banners.o banner.o smokePuff.o smokePuffs.o \
	shieldGlow.o background.o input.o

.PHONY: all game clean realclean rebuild tgz

all: depend $(GAME_NAME)

clean:
	rm -f *.o *.da

realclean:
	rm -f *.o *.da *~ Makefile.dep

rebuild: realclean game

.SUFFIXES: .cc

# How to compile a c++ programm
$(GAME_NAME): $(OBJECT_FILES)
	@echo ""
	@echo ""
	@echo "Linking $@"
	@$(COMPILER) -o $(GAME_NAME) $(OBJECT_FILES) $(GAME_LIBS)
	mv $(GAME_NAME) ../

%.o: %.cc
	@echo ""
	@echo ""
	@echo "Compiling $<"
	$(COMPILER) $(GAME_FLAGS) $(INCLUDE_PATH) -c $< -o $@ 

depend:  dep

dep:
	-touch Makefile.dep
	-makedepend $(INCLUDE_PATH) -Y -f Makefile.dep *.cc 2> /dev/null
	-rm -f Makefile.dep.bak

install: all
	@echo ""
	@echo ""
	@echo "Installing game's data..."
	@$(INSTALL_DIR) $(DATADIR)
	@$(INSTALL_DIR) $(DATADIR)/sound
	@$(INSTALL_DIR) $(DATADIR)/images
	@$(INSTALL_DIR) $(DATADIR)/cfg
	@$(INSTALL_FILES) ../sound/* $(DATADIR)/sound/
	@$(INSTALL_FILES) ../images/* $(DATADIR)/images/
	@$(INSTALL_FILES) ../cfg/* $(DATADIR)/cfg/
	@echo ""
	@echo "Installing $(GAME_NAME)..."
	@$(INSTALL_FILES) -m 755 ../$(GAME_NAME) /usr/local/games/$(GAME_NAME)
	@echo "Done."


-include Makefile.dep
