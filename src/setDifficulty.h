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
#ifndef SET_DIFFICULTY_H
#define SET_DIFFICULTY_H

#include "SDL.h"
#include "game.h"
#include <string>

class Font;

enum DIFFICULTY_CHOICES { DIFFICULTY_0, DIFFICULTY_1, DIFFICULTY_2, DIFFICULTY_3, 
			  BACK_TO_MAIN_MENU };
const std::string SET_DIFFICULTY_CHOICES[] = 
{ "Easy Going", "Let's Rock",
  "Kick Some Ass", "Argh...", 
  "Back to Main Menu" };
const int NR_DIFFICULTY_CHOICES = 5;

class SetDifficulty {
  private:
  SDL_Renderer *screen;
  SDL_Texture *introSprite;
  SDL_Rect introSpriteR;
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
  
  int activeChoice;
  bool onePlayerGame;
  bool playerOneLightFighter;
  bool playerTwoLightFighter;

  // sounds
  int confirm;
  int choose;

  public:
  SetDifficulty( SDL_Renderer *scr );
  ~SetDifficulty();
  void run( GameStates &gameState, bool onePlayerGame );
  bool getPlayerOneLightFighter();
  bool getPlayerTwoLightFighter();

  private:
  void handleEvents( GameStates &gameState );
  void draw();
};

#endif
