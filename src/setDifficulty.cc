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
using namespace std;

#include "setDifficulty.h"
#include "global.h"
#include "surfaceDB.h"
#include "mixer.h"
#include "video.h"
#include "font.h"
#include "settings.h"
#include "racer.h"
#include "racers.h"

SetDifficulty::SetDifficulty( SDL_Renderer *scr ) {
  screen = scr;
  introSprite = surfaceDB.loadSurface( FN_ALIENBLASTER_INTRO );
  SDL_QueryTexture (introSprite, NULL, NULL, &introSpriteR.w, &introSpriteR.h);
  activeChoiceSprite = surfaceDB.loadSurface( FN_INTRO_SHOW_CHOICE );
  SDL_QueryTexture (activeChoiceSprite, NULL, NULL, &activeChoiceSpriteR.w, &activeChoiceSpriteR.h);

  font = new Font( FN_FONT_INTRO );
  fontHighlighted = new Font( FN_FONT_INTRO_HIGHLIGHTED );
  lightFighterIcon1 = surfaceDB.loadSurface( FN_LIGHT_FIGHTER_1_ICON );
  SDL_QueryTexture (lightFighterIcon1, NULL, NULL, &lightFighterIcon1R.w, &lightFighterIcon1R.h);
  lightFighterIcon2 = surfaceDB.loadSurface( FN_LIGHT_FIGHTER_2_ICON );
  SDL_QueryTexture (lightFighterIcon2, NULL, NULL, &lightFighterIcon2R.w, &lightFighterIcon2R.h);
  heavyFighterIcon1 = surfaceDB.loadSurface( FN_HEAVY_FIGHTER_1_ICON );
  SDL_QueryTexture (heavyFighterIcon1, NULL, NULL, &heavyFighterIcon1R.w, &heavyFighterIcon1R.h);
  heavyFighterIcon2 = surfaceDB.loadSurface( FN_HEAVY_FIGHTER_2_ICON );
  SDL_QueryTexture (heavyFighterIcon2, NULL, NULL, &heavyFighterIcon2R.w, &heavyFighterIcon2R.h);
  choose = mixer.loadSample( FN_SOUND_INTRO_CHOOSE, 100 );
  confirm = mixer.loadSample( FN_SOUND_INTRO_CONFIRM, 100 );

  activeChoice = 0;
  playerOneLightFighter = true;
  playerTwoLightFighter = false;
}

SetDifficulty::~SetDifficulty() {}

void SetDifficulty::run( GameStates &gameState, bool onePlayerGame ) {
  activeChoice = 0;
  SetDifficulty::onePlayerGame = onePlayerGame;
  draw();
  while ( gameState == GS_SET_DIFFICULTY ) {
    handleEvents( gameState );
    draw();
    SDL_Delay( 50 );
  }
}

bool SetDifficulty::getPlayerOneLightFighter() {
  return playerOneLightFighter;
}

bool SetDifficulty::getPlayerTwoLightFighter() {
  return playerTwoLightFighter;
}
 
void SetDifficulty::draw() {
  videoserver->clearScreen();
  SDL_Rect r;
  r.x = SCREEN_WIDTH / 2 - introSpriteR.w / 2;
  r.y = 0;
  r.w = introSpriteR.w;
  r.h = introSpriteR.h;  
  //SDL_BlitSurface( introSprite, 0, screen, &r );
  SDL_RenderCopy(screen, introSprite, 0, &r );

  for ( int i = 0; i < NR_DIFFICULTY_CHOICES; i++ ) {
    if( activeChoice == i ) {
      r.x = 230 - activeChoiceSpriteR.w - 8;
      r.y = 258 + i * 40;
      r.w = activeChoiceSpriteR.w;
      r.h = activeChoiceSpriteR.h;
      //SDL_BlitSurface( activeChoiceSprite, 0, screen, &r );
      SDL_RenderCopy(screen, activeChoiceSprite, 0, &r );
      fontHighlighted->drawStr( screen, 230, 260 + i * 40, SET_DIFFICULTY_CHOICES[ i ] );
    } else {
      font->drawStr( screen, 230, 260 + i * 40, SET_DIFFICULTY_CHOICES[ i ] );
    }
    if( onePlayerGame ) {
      font->drawStr( screen, 20, 20, "1-Player" );
      font->drawStr( screen, 100, 270, "Player 1", FONT_ALIGN_CENTERED );
      if ( playerOneLightFighter ) {
        r.x = 100 - lightFighterIcon1R.w / 2;
        r.y = 340 - lightFighterIcon1R.h / 2;
        r.w = lightFighterIcon1R.w;
        r.h = lightFighterIcon1R.h;
        //SDL_BlitSurface( lightFighterIcon1, 0, screen, &r );
        SDL_RenderCopy(screen, lightFighterIcon1, 0, &r );
      } else {
        r.x = 100 - heavyFighterIcon1R.w / 2;
        r.y = 340 - heavyFighterIcon1R.h / 2;
        r.w = heavyFighterIcon1R.w;
        r.h = heavyFighterIcon1R.h;
        //SDL_BlitSurface( heavyFighterIcon1, 0, screen, &r );
        SDL_RenderCopy(screen, heavyFighterIcon1, 0, &r );
      }
      fontHighlighted->drawStr( screen, 100, 400, "Press \"1\"", FONT_ALIGN_CENTERED );
      fontHighlighted->drawStr( screen, 100, 430, "To Change", FONT_ALIGN_CENTERED );
    }
    else {
      font->drawStr( screen, 20, 20, "2-Player" );
      font->drawStr( screen, 100, 270, "Player 1", FONT_ALIGN_CENTERED );
      if ( playerOneLightFighter ) {
        r.x = 100 - lightFighterIcon1R.w / 2;
        r.y = 340 - lightFighterIcon1R.h / 2;
        r.w = lightFighterIcon1R.w;
        r.h = lightFighterIcon1R.h;
        //SDL_BlitSurface( lightFighterIcon1, 0, screen, &r );
        SDL_RenderCopy(screen, lightFighterIcon1, 0, &r );
      } else {
        r.x = 100 - heavyFighterIcon1R.w / 2;
        r.y = 340 - heavyFighterIcon1R.h / 2;
        r.w = heavyFighterIcon1R.w;
        r.h = heavyFighterIcon1R.h;
        //SDL_BlitSurface( heavyFighterIcon1, 0, screen, &r );
        SDL_RenderCopy(screen, heavyFighterIcon1, 0, &r );
      }
      fontHighlighted->drawStr( screen, 100, 400, "Press \"1\"", FONT_ALIGN_CENTERED );
      fontHighlighted->drawStr( screen, 100, 430, "To Change", FONT_ALIGN_CENTERED );
      font->drawStr( screen, 560, 270, "Player 2", FONT_ALIGN_CENTERED );
      if ( playerTwoLightFighter ) {
        r.x = 560 - lightFighterIcon1R.w / 2;
        r.y = 340 - lightFighterIcon1R.h / 2;
        r.w = lightFighterIcon2R.w;
        r.h = lightFighterIcon2R.h;
        //SDL_BlitSurface( lightFighterIcon2, 0, screen, &r );
        SDL_RenderCopy(screen, lightFighterIcon2, 0, &r );
      } else {
        r.x = 560 - heavyFighterIcon1R.w / 2;
        r.y = 340 - heavyFighterIcon1R.h / 2;
        r.w = heavyFighterIcon2R.w;
        r.h = heavyFighterIcon2R.h;
        //SDL_BlitSurface( heavyFighterIcon2, 0, screen, &r );
        SDL_RenderCopy(screen, heavyFighterIcon2, 0, &r );
      }
      fontHighlighted->drawStr( screen, 560, 400, "Press \"2\"", FONT_ALIGN_CENTERED );
      fontHighlighted->drawStr( screen, 560, 430, "To Change", FONT_ALIGN_CENTERED );
    }
  }
  //SDL_Flip( screen );
  SDL_RenderPresent( screen );
}

void SetDifficulty::handleEvents( GameStates &gameState ) {
  SDL_Event event;
  
  while ( SDL_PollEvent(&event) ) {
    switch(event.type) {
      case SDL_KEYDOWN: {
        switch ( event.key.keysym.sym ) {
          case SDLK_1: {
            playerOneLightFighter = !playerOneLightFighter;
            break;
          }
          case SDLK_2: {
            playerTwoLightFighter = !playerTwoLightFighter;
            break;
          }
          case SDLK_F5: {
            printf("Toggle fullscreen\n");
            //SDL_WM_ToggleFullScreen( screen );
            break;
          }
          case SDLK_F7: {
            if ( playMusicOn ) {
              playMusicOn = false;
              mixer.stopMusic();
            } else {
              playMusicOn = true;
              mixer.playMusic( MUSIC_INTRO, -1, 1000 );
            }
            break;
          }
          case SDLK_UP: {
            mixer.playSample( choose, 0 );
            activeChoice--;
            if ( activeChoice < 0 ) activeChoice = NR_DIFFICULTY_CHOICES - 1;
            break;
          }
          case SDLK_DOWN: {
            mixer.playSample( choose, 0 );
            activeChoice = (activeChoice + 1) % NR_DIFFICULTY_CHOICES;
            break;
          }
          case SDLK_ESCAPE: {
            gameState = GS_INTRO;
            break;
          }
          case SDLK_RETURN: {
            mixer.playSample( confirm, 0 );
            switch (activeChoice) {
              case DIFFICULTY_0: {
                difficultyLevel = 0;
	              gameState = GS_PLAYON;
                break;
              }
              case DIFFICULTY_1: {
                difficultyLevel = 1;
                gameState = GS_PLAYON;
                break;
	            }
              case DIFFICULTY_2: {
                difficultyLevel = 2;
                gameState = GS_PLAYON;
                break;
              }
              case DIFFICULTY_3: {
                difficultyLevel = 3;
                gameState = GS_PLAYON;
                break;
              }
              case BACK_TO_MAIN_MENU: {
                gameState = GS_INTRO;
                break;
              }
   	        }
            break;
  	      }
          default: break;
  	    }
        break;
      }
      case SDL_QUIT: {
        gameState = GS_QUIT;
         break;
      }
      default: break;
    }
  }
}

