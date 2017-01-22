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
#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include "SDL.h"
#include <string>
#include "item.h"

class Font;
class Items;

enum INFOSCREEN_CHOICES {
  INFO_LIGHT_FIGHTER,
  INFO_HEAVY_FIGHTER,
  INFO_PRIMARY_WEAPON,
  INFO_MACHINE_GUN,
  INFO_DUMBFIRE_DOUBLE,
  INFO_KICK_ASS_ROCKET,
  INFO_HELLFIRE,
  INFO_HEATSEEKER,
  INFO_NUKE,
  INFO_DEFLECTOR,
  INFO_ENERGY_BEAM,
  INFO_LASER,
  INFO_SHIELD_UP,
  INFO_CREDITS,
  INFO_BACK_TO_MAIN_MENU
};
const std::string SET_INFOSCREEN_CHOICES[] = {
  "Light Fighter",
  "Heavy Fighter",
  "Primary Weapon",
  "Machine Gun",
  "Double Dumbfire",
  "Kick-Ass Rocket",
  "Hellfire Rocket",
  "Heatseaker Rocket",
  "The Great Nuke",
  "Deflector",
  "Energy Beam",
  "Heavy Laser",
  "Health Powerup",
  "Credits",
  "Back To Main Menu"
};
  
const int NR_INFOSCREEN_CHOICES = 15;

class Infoscreen {
  private:
  SDL_Renderer *screen;
  SDL_Texture *activeChoiceSprite;
  SDL_Rect activeChoiceSpriteR;
  SDL_Texture *lightFighterIcon1;
  SDL_Rect lightFighterIcon1R;
  SDL_Texture *lightFighterIcon2;
  SDL_Rect lightFighterIcon2R;
  SDL_Texture *heavyFighterIcon1;
  SDL_Rect heavyFighterIcon1R;
  SDL_Texture *heavyFighterIcon2;
  SDL_Rect heavyFighterIcon2R;
  
  Font *font;
  Font *fontHighlighted;
  Item *newItem;
  
  int activeChoice;
  bool quitInfoscreen;

  // sounds
  int choose, confirm;

  public:
  Infoscreen( SDL_Renderer *scr );
  ~Infoscreen();
  void run();

  private:
  void handleEvents();
  void draw();
  void putBitmapAtPosition( int x, int y, SDL_Texture* bitmap );
};

#endif
