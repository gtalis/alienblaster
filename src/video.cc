/*************************************************************************** 
  alienBlaster 
  Copyright (C) 2004 
  Paul Grathwohl, Arne Hormann, Daniel Kuehn, Soenke Schwardt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
***************************************************************************/
#include "video.h"
#include "SDL.h"
#include <stdlib.h>
#include "global.h"

using namespace std;

Video *videoserver;

Video::Video(){
  screen = 0;
}

Video::~Video(){
  // kill something
}

SDL_Renderer *Video::init() {
  // --------------------------------------------------
  // SDL initialisation
  // -----------------------------------------------------
  fullscreen = false;
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    printf("Couldn't initialize SDL video subsystem: %s\n", SDL_GetError());
    exit(1);
  }
  
  game_window = NULL;
  game_window = SDL_CreateWindow("AlienBlaster", SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH,
    SCREEN_HEIGHT, 0);
  screen = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED);

  if (!screen) {
    printf("Couldn't create renderer!!!\n");
    exit(2);
  }
  
  return screen;
}


void Video::clearScreen() {
    SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
    SDL_RenderClear(screen);
    SDL_RenderPresent(screen);
}

void Video::toggleFullscreen() {

  if (fullscreen)
    SDL_SetWindowFullscreen(game_window, 0);
  else
    SDL_SetWindowFullscreen(game_window, SDL_WINDOW_FULLSCREEN);

  fullscreen = !fullscreen;
}
