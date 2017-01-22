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

#include <cassert>
#include <string>
#include <map>
#include "SDL.h"
#include "options.h"
#include "settings.h"
#include "global.h"
#include "surfaceDB.h"
#include "asstring.h"
#include "font.h"
#include "video.h"
#include "mixer.h"
#include "input.h"

Settings *settings;

Settings::Settings() {
  opfile = NULL;

  introSprite = surfaceDB.loadSurface( FN_ALIENBLASTER_INTRO );
  SDL_QueryTexture( introSprite, NULL, NULL, &introSpriteR.w, &introSpriteR.w);
  activeChoiceSprite = surfaceDB.loadSurface( FN_INTRO_SHOW_CHOICE );
  SDL_QueryTexture( activeChoiceSprite, NULL, NULL, &activeChoiceSpriteR.w, &activeChoiceSpriteR.w);
  bluePlain = surfaceDB.loadSurface( FN_SETTINGS_BLUE, true );
  whitePlain = surfaceDB.loadSurface( FN_SETTINGS_WHITE, false );

  fontMenu = new Font ( FN_FONT_SETTINGS );
  fontMenuHighlighted = new Font ( FN_FONT_SETTINGS_HIGHLIGHTED );
  fontNormal = new Font( FN_FONT_SETTINGS_SMALL );
  fontKey = new Font ( FN_FONT_SETTINGS_SMALL_BLUE );
  fontHighlighted = new Font( FN_FONT_SETTINGS_SMALL_HIGHLIGHTED );

  playerEventNames[ PE_UNKNOWN ] = "UNKNOWN"; 
  playerEventNames[ PE_UP ]    = "UP"; 
  playerEventNames[ PE_DOWN ]  = "DOWN"; 
  playerEventNames[ PE_LEFT ]  = "LEFT"; 
  playerEventNames[ PE_RIGHT ] = "RIGHT";
  playerEventNames[ PE_FIRE_WEAPONS ]  = "FIRE"; 
  playerEventNames[ PE_FIRE_SPECIALS ] = "FIRE-SPECIAL";
  playerEventNames[ PE_CHOOSE_WEAPON_SECONDARY ] = "CHOOSE-WEAPON"; 
  playerEventNames[ PE_CHOOSE_WEAPON_SPECIALS ]  = "CHOOSE-SPECIALS"; 
  
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_UP ] ] = SDLK_UP;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_DOWN ] ] = SDLK_DOWN;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_LEFT ] ] = SDLK_LEFT;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_RIGHT ] ] = SDLK_RIGHT;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_CHOOSE_WEAPON_SECONDARY ] ] = SDLK_RSHIFT;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_CHOOSE_WEAPON_SPECIALS ] ] = SDLK_PERIOD;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_FIRE_WEAPONS ] ] = SDLK_RCTRL;
  defaultSettings[ string("PLAYER0-") + playerEventNames[ PE_FIRE_SPECIALS ] ] = SDLK_MODE;
  
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_UP ] ] = SDLK_e;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_DOWN ] ] = SDLK_d;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_LEFT ] ] = SDLK_s;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_RIGHT ] ] = SDLK_f;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_CHOOSE_WEAPON_SECONDARY ] ] = SDLK_LSHIFT;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_CHOOSE_WEAPON_SPECIALS ] ] = SDLK_y;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_FIRE_WEAPONS ] ] = SDLK_LCTRL;
  defaultSettings[ string("PLAYER1-") + playerEventNames[ PE_FIRE_SPECIALS ] ] = SDLK_LALT;

  setKeyNames();

  loadSettings();
}


Settings::~Settings() {
  if (fontKey) delete fontKey;
  if (fontHighlighted) delete fontHighlighted;
  if (fontNormal) delete fontNormal;
  if (fontMenu) delete fontMenu;
  if (opfile) delete opfile;
}


void Settings::loadSettings() {
  bool restoredSettings = false;
  if (opfile) {
    delete opfile;
  }
  opfile = new Options( FN_SETTINGS );
  playerKeys.clear();
  for(int i=0; i < MAX_PLAYER_CNT; ++i) {
    PlayerEventKeys pk;
    for(int t=1; t < PlayerEventCnt; ++t) {
      int key;
      string keyname = string("PLAYER") + asString(i) + "-" + playerEventNames[(PlayerEvent)t];
      if (!opfile->getInt( keyname , key)) {
	key = defaultSettings[ keyname ];
	restoredSettings = true;
      }
      pk[ (PlayerEvent)t ] = (SDL_Keycode)key;
    }
    playerKeys.push_back(pk);
  }
  if (restoredSettings) {
    saveSettings();
  }
}

void Settings::saveSettings() {
  for(int i=0; i < MAX_PLAYER_CNT; ++i) {
    string name;
    for(int t=1; t < PlayerEventCnt; ++t) {
      opfile->setInt( (int)playerKeys[i][ (PlayerEvent)t ], 
		      string("PLAYER") + asString(i) + "-" + playerEventNames[(PlayerEvent)t] );
    }
  }
  if (opfile) {
    opfile->saveFile( FN_SETTINGS );
  }
}


void Settings::draw( SDL_Renderer *screen, bool getNewKey ) {
  videoserver->clearScreen();
  SDL_Rect r;

  //r.x = SCREEN_WIDTH / 2 - introSpriteR.w / 2;
  r.x = SCREEN_WIDTH / 2 - introSpriteR.w / 2;
  r.y = 0;
  r.w = introSpriteR.w;
  r.h = introSpriteR.h;  
  //SDL_BlitSurface( introSprite, 0, screen, &r );
  SDL_RenderCopy(screen, introSprite, 0, &r );
  showSpecialKeys( screen );
  showSettings( screen, getNewKey );
  showMenu( screen );
  //SDL_Flip( screen );
  SDL_RenderPresent( screen );
}


void Settings::showSpecialKeys( SDL_Renderer *screen ) {
  int screen_w = SCREEN_WIDTH;
  int screen_h = SCREEN_HEIGHT;

  fontNormal->drawStr(screen, screen_w/2, screen_h - 2*fontNormal->getCharHeight() - 10,
		      "F1: Configure Keys   F5: Fullscreen", FONT_ALIGN_CENTERED );
  fontNormal->drawStr(screen, screen_w/2, screen_h - fontNormal->getCharHeight() - 5,
		      "F6: Display Enemy Stats   F7: Toggle Music   F10: Pause Game",
		      FONT_ALIGN_CENTERED );
}


void Settings::showSettings( SDL_Renderer *screen, bool getNewKey ) {
  int playerOfActiveItem = -1;
  if ( actChoice <= SC_FIRE_SPEC_1 ) playerOfActiveItem = 0;
  else if ( actChoice <= SC_FIRE_SPEC_2 ) playerOfActiveItem = 1;
  int activePlayerEvent = actChoice - (8*playerOfActiveItem) + 1;
  SDL_Rect r;
  SDL_Rect srcDesc;
  srcDesc.x = 0;
  srcDesc.y = 0;
  srcDesc.w = 155;
  srcDesc.h = 14;
  SDL_Rect srcKey;
  srcKey.x = 0;
  srcKey.y = 0;
  srcKey.w = 100;
  srcKey.h = 14;

  int i = 150;
  fontHighlighted->drawStr( screen, 15, i - fontHighlighted->getCharHeight() - 5, "Player 1" );
  fontHighlighted->drawStr( screen, SCREEN_WIDTH/2 + 5, i - fontHighlighted->getCharHeight() - 5, 
		       "Player 2" );

  std::map< PlayerEvent, SDL_Keycode >::const_iterator pkiter;
  for ( pkiter = playerKeys[0].begin(); pkiter != playerKeys[0].end(); ++pkiter ) {
    
    if ( !(playerOfActiveItem == 0 && pkiter->first == activePlayerEvent) ) {
      r.x = 15;
      r.y = i - 1;
      //SDL_BlitSurface(bluePlain, &srcDesc, screen, &r );
      SDL_RenderCopy(screen, bluePlain, &srcDesc, &r );
      fontNormal->drawStr( screen,  20, i, playerEventNames[ pkiter->first ] + ":" );
      
      r.x = 175;
      r.y = i-1;
      //SDL_BlitSurface(bluePlain, &srcKey, screen, &r );
      SDL_RenderCopy(screen, bluePlain, &srcKey, &r );
      fontNormal->drawStr( screen, 270, i, keyName[ pkiter->second ], FONT_ALIGN_RIGHT );
    }
    if ( !(playerOfActiveItem == 1 && pkiter->first == activePlayerEvent) ) {
      r.x = SCREEN_WIDTH/2 + 5;
      r.y = i - 1;
      //SDL_BlitSurface(bluePlain, &srcDesc, screen, &r );
      SDL_RenderCopy(screen, bluePlain, &srcDesc, &r );
      
      fontNormal->drawStr( screen, SCREEN_WIDTH/2 + 10,  i, 
			   playerEventNames[ pkiter->first ] + ":" );

      r.x = SCREEN_WIDTH/2 + 165;
      r.y = i-1;
      //SDL_BlitSurface(bluePlain, &srcKey, screen, &r );
      SDL_RenderCopy(screen, bluePlain, &srcKey, &r );
      fontNormal->drawStr( screen, SCREEN_WIDTH/2 + 260, i, 
			   keyName[ playerKeys[1][pkiter->first] ], FONT_ALIGN_RIGHT );
    }
    
    if ( playerOfActiveItem == 0 && pkiter->first == activePlayerEvent ) {
      r.x = 15;
      r.y = i - 1;
      //SDL_BlitSurface(bluePlain, &srcDesc, screen, &r );
      SDL_RenderCopy(screen, bluePlain, &srcDesc, &r );
      fontNormal->drawStr( screen, 20, i, playerEventNames[ pkiter->first ] + ":" );

      r.x = 175;
      r.y = i-1;
      //SDL_BlitSurface(whitePlain, &srcKey, screen, &r );
      SDL_RenderCopy(screen, whitePlain, &srcKey, &r );
      if (!getNewKey) {
	fontHighlighted->drawStr( screen, 270, i, keyName[ pkiter->second ], FONT_ALIGN_RIGHT );
      }
    }
    if ( playerOfActiveItem == 1 && pkiter->first == activePlayerEvent ) {
      r.x = SCREEN_WIDTH/2 + 5;
      r.y = i - 1;
      //SDL_BlitSurface(bluePlain, &srcDesc, screen, &r );
      SDL_RenderCopy(screen, bluePlain, &srcDesc, &r );
      
      fontNormal->drawStr( screen, SCREEN_WIDTH/2 + 10,  i, 
			   playerEventNames[ pkiter->first ] + ":" );
      r.x = SCREEN_WIDTH/2 + 165;
      r.y = i-1;
      //SDL_BlitSurface(whitePlain, &srcKey, screen, &r );
      SDL_RenderCopy(screen, whitePlain, &srcKey, &r );
      if (!getNewKey) {
	fontHighlighted->drawStr( screen, SCREEN_WIDTH/2 + 260, i, 
				  keyName[ playerKeys[1][pkiter->first] ], FONT_ALIGN_RIGHT );
      }
    }
    
    i = i + fontNormal->getCharHeight() + 5;
  }
}

void Settings::showMenu( SDL_Renderer *screen ) {
  SDL_Rect r;
  r.x = 230 - activeChoiceSpriteR.w - 8;
  r.w = activeChoiceSpriteR.w;
  r.h = activeChoiceSpriteR.h;
  
  if ( actChoice == SC_DEFAULTS ) {
    r.y = 328;
    //SDL_BlitSurface(activeChoiceSprite, 0, screen, &r );
    SDL_RenderCopy(screen, activeChoiceSprite, 0, &r );
    fontMenuHighlighted->drawStr( screen, 230, 330, "Reset Defaults" );
  } else fontMenu->drawStr( screen, 230, 330, "Reset Defaults" );

  if ( actChoice == SC_CANCEL ) {
    r.y = 363;
    //SDL_BlitSurface(activeChoiceSprite, 0, screen, &r );
    SDL_RenderCopy(screen, activeChoiceSprite, 0, &r );
    fontMenuHighlighted->drawStr( screen, 230, 365, "Cancel" );
  } else fontMenu->drawStr( screen, 230, 365, "Cancel" );

  if ( actChoice == SC_FINISH ) {
    r.y = 398;
    //SDL_BlitSurface(activeChoiceSprite, 0, screen, &r );
    SDL_RenderCopy(screen, activeChoiceSprite, 0, &r );
    fontMenuHighlighted->drawStr( screen, 230, 400, "Finish" );
  } else fontMenu->drawStr( screen, 230, 400, "Finish" );
}

void Settings::settingsDialog( SDL_Renderer *screen ) {
  bool run = true;
  actChoice = SC_FINISH;
  wasLeftColumn = true;
  draw( screen );
  while(run) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_KEYDOWN: {
  	      switch(event.key.keysym.sym) {
            case SDLK_UP: {
              if ( actChoice == SC_DEFAULTS ) {
                if ( wasLeftColumn ) actChoice = SC_FIRE_SPEC_1;
                else actChoice = SC_FIRE_SPEC_2;
	          } else if ( actChoice == SC_UP2 || actChoice == SC_UP1 ) {
		        actChoice = SC_FINISH;
	          } else {
		        actChoice = (SettingsChoices)(actChoice - 1);
	          }
	          break;
	        }
	        case SDLK_DOWN: {
	          if ( actChoice == SC_FINISH ) {
		        if ( wasLeftColumn ) actChoice = SC_UP1;
		        else actChoice = SC_UP2;
	          } else if ( actChoice == SC_FIRE_SPEC_1 ) {
		        actChoice = SC_DEFAULTS;
	          } else {
		        actChoice = (SettingsChoices)(actChoice + 1);
	          }
	          break;
	        }
	        case SDLK_RIGHT: {
	          if (actChoice <= SC_FIRE_SPEC_1) {
		        actChoice = (SettingsChoices)(actChoice + 8);
		        wasLeftColumn = false;
	          }
	          break;
	        }
	        case SDLK_LEFT: {
	          if ( SC_UP2 <= actChoice && actChoice <= SC_FIRE_SPEC_2 ) {
		        actChoice = (SettingsChoices)(actChoice - 8);
		        wasLeftColumn = true;
	          }
	          break;
	        }
	        case SDLK_TAB: {
	          if ( SC_UP1 <= actChoice && actChoice <= SC_FIRE_SPEC_1 ) {
		        actChoice = (SettingsChoices)(actChoice + 8);
		        wasLeftColumn = false;
	          } else {
		        if ( SC_UP2 <= actChoice && actChoice <= SC_FIRE_SPEC_2 ) {
		          actChoice = (SettingsChoices)(actChoice - 8);
		          wasLeftColumn = true;
		        }
	          }
	          break;
	        }
	        case SDLK_RETURN:
	        case SDLK_SPACE: {
              if ( actChoice == SC_DEFAULTS ) {
		        loadDefaultSettings();
	          } else if ( actChoice == SC_CANCEL ) {
		        run = false;
		        loadSettings();
	          } else if ( actChoice == SC_FINISH ) {
		        run = false;
		        saveSettings();
	          } else {
		        draw( screen, true );
		        getNewKeyFromUser();
	          }
	          break;
	        }
            case SDLK_F5: {
	          videoserver->toggleFullscreen();
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
	        case SDLK_ESCAPE: {
              run = false;
              loadDefaultSettings();
              break;
            }
            default: break;
          }
        }
      }
      draw( screen );
      SDL_Delay( 50 ); // save cpu-power
    }
  }
}

void Settings::loadDefaultSettings() {
  playerKeys.clear();
  for(int i=0; i < MAX_PLAYER_CNT; ++i) {
    PlayerEventKeys pk;   
    for(int t=1; t < PlayerEventCnt; ++t) {
      int key;
      string keyname = string("PLAYER") + asString(i) + "-" + playerEventNames[(PlayerEvent)t];
      key = defaultSettings[ keyname ];
      pk[ (PlayerEvent)t ] = (SDL_Keycode)key;
    }
    playerKeys.push_back(pk);
  }
}

void Settings::getNewKeyFromUser() {
  SDL_Event event;
  bool keypressed = false;
  while( !keypressed ) {
    while (!SDL_PollEvent(&event)) {}
    switch(event.type) {
    case SDL_KEYDOWN:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYAXISMOTION: {
      if (input.isPressed(event)) {
        int player = (actChoice >= SC_UP2) ? 1 : 0;
        playerKeys[player][ (PlayerEvent)(actChoice - (player * 8) + 1) ] = 
	      input.translate(event);
	    keypressed = true;
      }
	  break;
    }
    }    
  }
}

const PlayerKeys Settings::getPlayerKeys(unsigned int player) const {
  assert( player < playerKeys.size() );
  PlayerKeys pk;
  printf("settings -> getPlayerKeys");
  //for(int i=SDLK_FIRST; i <= SDLK_LAST; ++i) {
  //  pk[ (SDL_Keycode)i ] = PE_UNKNOWN;
  //}
  std::map< PlayerEvent, SDL_Keycode >::const_iterator pkiter;
  for(pkiter = playerKeys[player].begin(); pkiter != playerKeys[player].end(); ++pkiter) {
    pk[ pkiter->second ] = pkiter->first;
  }
  return pk;
}

void Settings::setKeyNames() {
  keyName[ (SDL_Keycode)0 ] = "UNKNOWN";
  keyName[ (SDL_Keycode)8 ] = "BACKSPACE";
  keyName[ (SDL_Keycode)9 ] = "TAB";
  keyName[ (SDL_Keycode)12 ] = "CLEAR";
  keyName[ (SDL_Keycode)13 ] = "RETURN";
  keyName[ (SDL_Keycode)19 ] = "PAUSE";
  keyName[ (SDL_Keycode)27 ] = "ESCAPE";
  keyName[ (SDL_Keycode)32 ] = "SPACE";
  keyName[ (SDL_Keycode)33 ] = "EXCLAIM";
  keyName[ (SDL_Keycode)34 ] = "QUOTEDBL";
  keyName[ (SDL_Keycode)35 ] = "HASH";
  keyName[ (SDL_Keycode)36 ] = "DOLLAR";
  keyName[ (SDL_Keycode)38 ] = "AMPERSAND";
  keyName[ (SDL_Keycode)39 ] = "QUOTE";
  keyName[ (SDL_Keycode)40 ] = "LEFTPAREN";
  keyName[ (SDL_Keycode)41 ] = "RIGHTPAREN";
  keyName[ (SDL_Keycode)42 ] = "ASTERISK";
  keyName[ (SDL_Keycode)43 ] = "PLUS";
  keyName[ (SDL_Keycode)44 ] = "COMMA";
  keyName[ (SDL_Keycode)45 ] = "MINUS";
  keyName[ (SDL_Keycode)46 ] = "PERIOD";
  keyName[ (SDL_Keycode)47 ] = "SLASH";
  keyName[ (SDL_Keycode)48 ] = "0";
  keyName[ (SDL_Keycode)49 ] = "1";
  keyName[ (SDL_Keycode)50 ] = "2";
  keyName[ (SDL_Keycode)51 ] = "3";
  keyName[ (SDL_Keycode)52 ] = "4";
  keyName[ (SDL_Keycode)53 ] = "5";
  keyName[ (SDL_Keycode)54 ] = "6";
  keyName[ (SDL_Keycode)55 ] = "7";
  keyName[ (SDL_Keycode)56 ] = "8";
  keyName[ (SDL_Keycode)57 ] = "9";
  keyName[ (SDL_Keycode)58 ] = "COLON";
  keyName[ (SDL_Keycode)59 ] = "SEMICOLON";
  keyName[ (SDL_Keycode)60 ] = "LESS";
  keyName[ (SDL_Keycode)61 ] = "EQUALS";
  keyName[ (SDL_Keycode)62 ] = "GREATER";
  keyName[ (SDL_Keycode)63 ] = "QUESTION";
  keyName[ (SDL_Keycode)64 ] = "AT";
  keyName[ (SDL_Keycode)91 ] = "LEFTBRACKET";
  keyName[ (SDL_Keycode)92 ] = "BACKSLASH";
  keyName[ (SDL_Keycode)93 ] = "RIGHTBRACKET";
  keyName[ (SDL_Keycode)94 ] = "CARET";
  keyName[ (SDL_Keycode)95 ] = "UNDERSCORE";
  keyName[ (SDL_Keycode)96 ] = "BACKQUOTE";
  keyName[ (SDL_Keycode)97 ] = "a";
  keyName[ (SDL_Keycode)98 ] = "b";
  keyName[ (SDL_Keycode)99 ] = "c";
  keyName[ (SDL_Keycode)100 ] = "d";
  keyName[ (SDL_Keycode)101 ] = "e";
  keyName[ (SDL_Keycode)102 ] = "f";
  keyName[ (SDL_Keycode)103 ] = "g";
  keyName[ (SDL_Keycode)104 ] = "h";
  keyName[ (SDL_Keycode)105 ] = "i";
  keyName[ (SDL_Keycode)106 ] = "j";
  keyName[ (SDL_Keycode)107 ] = "k";
  keyName[ (SDL_Keycode)108 ] = "l";
  keyName[ (SDL_Keycode)109 ] = "m";
  keyName[ (SDL_Keycode)110 ] = "n";
  keyName[ (SDL_Keycode)111 ] = "o";
  keyName[ (SDL_Keycode)112 ] = "p";
  keyName[ (SDL_Keycode)113 ] = "q";
  keyName[ (SDL_Keycode)114 ] = "r";
  keyName[ (SDL_Keycode)115 ] = "s";
  keyName[ (SDL_Keycode)116 ] = "t";
  keyName[ (SDL_Keycode)117 ] = "u";
  keyName[ (SDL_Keycode)118 ] = "v";
  keyName[ (SDL_Keycode)119 ] = "w";
  keyName[ (SDL_Keycode)120 ] = "x";
  keyName[ (SDL_Keycode)121 ] = "y";
  keyName[ (SDL_Keycode)122 ] = "z";
  keyName[ (SDL_Keycode)127 ] = "DELETE";
  keyName[ (SDL_Keycode)160 ] = "WORLD_0";
  keyName[ (SDL_Keycode)161 ] = "WORLD_1";
  keyName[ (SDL_Keycode)162 ] = "WORLD_2";
  keyName[ (SDL_Keycode)163 ] = "WORLD_3";
  keyName[ (SDL_Keycode)164 ] = "WORLD_4";
  keyName[ (SDL_Keycode)165 ] = "WORLD_5";
  keyName[ (SDL_Keycode)166 ] = "WORLD_6";
  keyName[ (SDL_Keycode)167 ] = "WORLD_7";
  keyName[ (SDL_Keycode)168 ] = "WORLD_8";
  keyName[ (SDL_Keycode)169 ] = "WORLD_9";
  keyName[ (SDL_Keycode)170 ] = "WORLD_10";
  keyName[ (SDL_Keycode)171 ] = "WORLD_11";
  keyName[ (SDL_Keycode)172 ] = "WORLD_12";
  keyName[ (SDL_Keycode)173 ] = "WORLD_13";
  keyName[ (SDL_Keycode)174 ] = "WORLD_14";
  keyName[ (SDL_Keycode)175 ] = "WORLD_15";
  keyName[ (SDL_Keycode)176 ] = "WORLD_16";
  keyName[ (SDL_Keycode)177 ] = "WORLD_17";
  keyName[ (SDL_Keycode)178 ] = "WORLD_18";
  keyName[ (SDL_Keycode)179 ] = "WORLD_19";
  keyName[ (SDL_Keycode)180 ] = "WORLD_20";
  keyName[ (SDL_Keycode)181 ] = "WORLD_21";
  keyName[ (SDL_Keycode)182 ] = "WORLD_22";
  keyName[ (SDL_Keycode)183 ] = "WORLD_23";
  keyName[ (SDL_Keycode)184 ] = "WORLD_24";
  keyName[ (SDL_Keycode)185 ] = "WORLD_25";
  keyName[ (SDL_Keycode)186 ] = "WORLD_26";
  keyName[ (SDL_Keycode)187 ] = "WORLD_27";
  keyName[ (SDL_Keycode)188 ] = "WORLD_28";
  keyName[ (SDL_Keycode)189 ] = "WORLD_29";
  keyName[ (SDL_Keycode)190 ] = "WORLD_30";
  keyName[ (SDL_Keycode)191 ] = "WORLD_31";
  keyName[ (SDL_Keycode)192 ] = "WORLD_32";
  keyName[ (SDL_Keycode)193 ] = "WORLD_33";
  keyName[ (SDL_Keycode)194 ] = "WORLD_34";
  keyName[ (SDL_Keycode)195 ] = "WORLD_35";
  keyName[ (SDL_Keycode)196 ] = "WORLD_36";
  keyName[ (SDL_Keycode)197 ] = "WORLD_37";
  keyName[ (SDL_Keycode)198 ] = "WORLD_38";
  keyName[ (SDL_Keycode)199 ] = "WORLD_39";
  keyName[ (SDL_Keycode)200 ] = "WORLD_40";
  keyName[ (SDL_Keycode)201 ] = "WORLD_41";
  keyName[ (SDL_Keycode)202 ] = "WORLD_42";
  keyName[ (SDL_Keycode)203 ] = "WORLD_43";
  keyName[ (SDL_Keycode)204 ] = "WORLD_44";
  keyName[ (SDL_Keycode)205 ] = "WORLD_45";
  keyName[ (SDL_Keycode)206 ] = "WORLD_46";
  keyName[ (SDL_Keycode)207 ] = "WORLD_47";
  keyName[ (SDL_Keycode)208 ] = "WORLD_48";
  keyName[ (SDL_Keycode)209 ] = "WORLD_49";
  keyName[ (SDL_Keycode)210 ] = "WORLD_50";
  keyName[ (SDL_Keycode)211 ] = "WORLD_51";
  keyName[ (SDL_Keycode)212 ] = "WORLD_52";
  keyName[ (SDL_Keycode)213 ] = "WORLD_53";
  keyName[ (SDL_Keycode)214 ] = "WORLD_54";
  keyName[ (SDL_Keycode)215 ] = "WORLD_55";
  keyName[ (SDL_Keycode)216 ] = "WORLD_56";
  keyName[ (SDL_Keycode)217 ] = "WORLD_57";
  keyName[ (SDL_Keycode)218 ] = "WORLD_58";
  keyName[ (SDL_Keycode)219 ] = "WORLD_59";
  keyName[ (SDL_Keycode)220 ] = "WORLD_60";
  keyName[ (SDL_Keycode)221 ] = "WORLD_61";
  keyName[ (SDL_Keycode)222 ] = "WORLD_62";
  keyName[ (SDL_Keycode)223 ] = "WORLD_63";
  keyName[ (SDL_Keycode)224 ] = "WORLD_64";
  keyName[ (SDL_Keycode)225 ] = "WORLD_65";
  keyName[ (SDL_Keycode)226 ] = "WORLD_66";
  keyName[ (SDL_Keycode)227 ] = "WORLD_67";
  keyName[ (SDL_Keycode)228 ] = "WORLD_68";
  keyName[ (SDL_Keycode)229 ] = "WORLD_69";
  keyName[ (SDL_Keycode)230 ] = "WORLD_70";
  keyName[ (SDL_Keycode)231 ] = "WORLD_71";
  keyName[ (SDL_Keycode)232 ] = "WORLD_72";
  keyName[ (SDL_Keycode)233 ] = "WORLD_73";
  keyName[ (SDL_Keycode)234 ] = "WORLD_74";
  keyName[ (SDL_Keycode)235 ] = "WORLD_75";
  keyName[ (SDL_Keycode)236 ] = "WORLD_76";
  keyName[ (SDL_Keycode)237 ] = "WORLD_77";
  keyName[ (SDL_Keycode)238 ] = "WORLD_78";
  keyName[ (SDL_Keycode)239 ] = "WORLD_79";
  keyName[ (SDL_Keycode)240 ] = "WORLD_80";
  keyName[ (SDL_Keycode)241 ] = "WORLD_81";
  keyName[ (SDL_Keycode)242 ] = "WORLD_82";
  keyName[ (SDL_Keycode)243 ] = "WORLD_83";
  keyName[ (SDL_Keycode)244 ] = "WORLD_84";
  keyName[ (SDL_Keycode)245 ] = "WORLD_85";
  keyName[ (SDL_Keycode)246 ] = "WORLD_86";
  keyName[ (SDL_Keycode)247 ] = "WORLD_87";
  keyName[ (SDL_Keycode)248 ] = "WORLD_88";
  keyName[ (SDL_Keycode)249 ] = "WORLD_89";
  keyName[ (SDL_Keycode)250 ] = "WORLD_90";
  keyName[ (SDL_Keycode)251 ] = "WORLD_91";
  keyName[ (SDL_Keycode)252 ] = "WORLD_92";
  keyName[ (SDL_Keycode)253 ] = "WORLD_93";
  keyName[ (SDL_Keycode)254 ] = "WORLD_94";
  keyName[ (SDL_Keycode)255 ] = "WORLD_95";
  keyName[ (SDL_Keycode)256 ] = "KP0";
  keyName[ (SDL_Keycode)257 ] = "KP1";
  keyName[ (SDL_Keycode)258 ] = "KP2";
  keyName[ (SDL_Keycode)259 ] = "KP3";
  keyName[ (SDL_Keycode)260 ] = "KP4";
  keyName[ (SDL_Keycode)261 ] = "KP5";
  keyName[ (SDL_Keycode)262 ] = "KP6";
  keyName[ (SDL_Keycode)263 ] = "KP7";
  keyName[ (SDL_Keycode)264 ] = "KP8";
  keyName[ (SDL_Keycode)265 ] = "KP9";
  keyName[ (SDL_Keycode)266 ] = "KP_PERIOD";
  keyName[ (SDL_Keycode)267 ] = "KP_DIVIDE";
  keyName[ (SDL_Keycode)268 ] = "KP_MULTIPLY";
  keyName[ (SDL_Keycode)269 ] = "KP_MINUS";
  keyName[ (SDL_Keycode)270 ] = "KP_PLUS";
  keyName[ (SDL_Keycode)271 ] = "KP_ENTER";
  keyName[ (SDL_Keycode)272 ] = "KP_EQUALS";
  keyName[ (SDL_Keycode)273 ] = "UP";
  keyName[ (SDL_Keycode)274 ] = "DOWN";
  keyName[ (SDL_Keycode)275 ] = "RIGHT";
  keyName[ (SDL_Keycode)276 ] = "LEFT";
  keyName[ (SDL_Keycode)277 ] = "INSERT";
  keyName[ (SDL_Keycode)278 ] = "HOME";
  keyName[ (SDL_Keycode)279 ] = "END";
  keyName[ (SDL_Keycode)280 ] = "PAGEUP";
  keyName[ (SDL_Keycode)281 ] = "PAGEDOWN";
  keyName[ (SDL_Keycode)282 ] = "F1";
  keyName[ (SDL_Keycode)283 ] = "F2";
  keyName[ (SDL_Keycode)284 ] = "F3";
  keyName[ (SDL_Keycode)285 ] = "F4";
  keyName[ (SDL_Keycode)286 ] = "F5";
  keyName[ (SDL_Keycode)287 ] = "F6";
  keyName[ (SDL_Keycode)288 ] = "F7";
  keyName[ (SDL_Keycode)289 ] = "F8";
  keyName[ (SDL_Keycode)290 ] = "F9";
  keyName[ (SDL_Keycode)291 ] = "F10";
  keyName[ (SDL_Keycode)292 ] = "F11";
  keyName[ (SDL_Keycode)293 ] = "F12";
  keyName[ (SDL_Keycode)294 ] = "F13";
  keyName[ (SDL_Keycode)295 ] = "F14";
  keyName[ (SDL_Keycode)296 ] = "F15";
  keyName[ (SDL_Keycode)300 ] = "NUMLOCK";
  keyName[ (SDL_Keycode)301 ] = "CAPSLOCK";
  keyName[ (SDL_Keycode)302 ] = "SCROLLOCK";
  keyName[ (SDL_Keycode)303 ] = "RSHIFT";
  keyName[ (SDL_Keycode)304 ] = "LSHIFT";
  keyName[ (SDL_Keycode)305 ] = "RCTRL";
  keyName[ (SDL_Keycode)306 ] = "LCTRL";
  keyName[ (SDL_Keycode)307 ] = "RALT";
  keyName[ (SDL_Keycode)308 ] = "LALT";
  keyName[ (SDL_Keycode)309 ] = "RMETA";
  keyName[ (SDL_Keycode)310 ] = "LMETA";
  keyName[ (SDL_Keycode)311 ] = "LSUPER";
  keyName[ (SDL_Keycode)312 ] = "RSUPER";
  keyName[ (SDL_Keycode)313 ] = "MODE";
  keyName[ (SDL_Keycode)314 ] = "COMPOSE";
  keyName[ (SDL_Keycode)315 ] = "HELP";
  keyName[ (SDL_Keycode)316 ] = "PRINT";
  keyName[ (SDL_Keycode)317 ] = "SYSREQ";
  keyName[ (SDL_Keycode)318 ] = "BREAK";
  keyName[ (SDL_Keycode)319 ] = "MENU";
  keyName[ (SDL_Keycode)320 ] = "POWER";
  keyName[ (SDL_Keycode)321 ] = "EURO";
  keyName[ (SDL_Keycode)322 ] = "UNDO";
  // the following entries are chosen by us... just for the mapping
  keyName[ (SDL_Keycode)1024 ] = "GPD1_L";
  keyName[ (SDL_Keycode)1025 ] = "GPD1_R";
  keyName[ (SDL_Keycode)1026 ] = "GPD1_U";
  keyName[ (SDL_Keycode)1027 ] = "GPD1_D";
  keyName[ (SDL_Keycode)1028 ] = "GPD2_L";
  keyName[ (SDL_Keycode)1029 ] = "GPD2_R";
  keyName[ (SDL_Keycode)1030 ] = "GPD2_U";
  keyName[ (SDL_Keycode)1031 ] = "GPD2_D";
  keyName[ (SDL_Keycode)1032 ] = "GPD3_L";
  keyName[ (SDL_Keycode)1033 ] = "GPD3_R";
  keyName[ (SDL_Keycode)1034 ] = "GPD3_U";
  keyName[ (SDL_Keycode)1035 ] = "GPD3_D";
  keyName[ (SDL_Keycode)1036 ] = "GPD4_L";
  keyName[ (SDL_Keycode)1037 ] = "GPD4_R";
  keyName[ (SDL_Keycode)1038 ] = "GPD4_U";
  keyName[ (SDL_Keycode)1039 ] = "GPD4_D";
  keyName[ (SDL_Keycode)1040 ] = "GPB_0";
  keyName[ (SDL_Keycode)1041 ] = "GPB_1";
  keyName[ (SDL_Keycode)1042 ] = "GPB_2";
  keyName[ (SDL_Keycode)1043 ] = "GPB_3";
  keyName[ (SDL_Keycode)1044 ] = "GPB_4";
  keyName[ (SDL_Keycode)1045 ] = "GPB_5";
  keyName[ (SDL_Keycode)1046 ] = "GPB_6";
  keyName[ (SDL_Keycode)1047 ] = "GPB_7";
  keyName[ (SDL_Keycode)1048 ] = "GPB_8";
  keyName[ (SDL_Keycode)1049 ] = "GPB_9";
  keyName[ (SDL_Keycode)1050 ] = "GPB_10";
  keyName[ (SDL_Keycode)1051 ] = "GPB_11";
  keyName[ (SDL_Keycode)1052 ] = "GPB_12";
  keyName[ (SDL_Keycode)1053 ] = "GPB_13";
  keyName[ (SDL_Keycode)1054 ] = "GPB_14";
  keyName[ (SDL_Keycode)1055 ] = "GPB_15";
}
