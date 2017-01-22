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
#include "banner.h"
#include "surfaceDB.h"
#include <iostream>

Banner::Banner( BannerTexts text, BannerModes mode, BannerBoni bonus ) {
  sprite = surfaceDB.loadSurface( FN_BANNER_TEXTS[ text ], true );
  SDL_QueryTexture(sprite, NULL, NULL, &spriteR.w, &spriteR.h);

  this->mode = mode;
  if ( this->mode == BANNER_MODE_RANDOM ) {
    this->mode = (BannerModes)(rand() % NR_BANNER_MODES);
  }
  this->bonus = bonus;
  if ( this->bonus != BANNER_BONUS_NONE ) {
    spriteBonus = surfaceDB.loadSurface( FN_BANNER_BONUS[ bonus ], true );
    SDL_QueryTexture(spriteBonus, NULL, NULL, &spriteBonusR.w, &spriteBonusR.h);
  }
  
  pos = Vector2D( -1000, -1000 );

  if ( mode == BANNER_MODE_ITEM_COLLECTED_SINGLE_PLAYER ) {
    pos = Vector2D( 10, SCREEN_HEIGHT - 20 - spriteR.h );
  } else if ( mode == BANNER_MODE_ITEM_COLLECTED_PLAYER_ONE ) {
    pos = Vector2D( 10, SCREEN_HEIGHT - 20 - spriteR.h );
  } else if ( mode == BANNER_MODE_ITEM_COLLECTED_PLAYER_TWO ) {
    pos = Vector2D( SCREEN_WIDTH - spriteR.w - 10,
		    SCREEN_HEIGHT - 20 - spriteR.h );
  }
  timeLived = 0;
}


Banner::~Banner() {}

bool Banner::isExpired() {
  return timeLived > BANNER_MODE_LIFETIME[ mode ];
}


void Banner::update( int dT ) {
  timeLived += dT;
  
  switch ( mode ) {
  case BANNER_MODE_FLY_FROM_LEFT:
    {
      if ( timeLived < 1000 ) {
	pos = Vector2D( -(spriteR.w) + ((SCREEN_WIDTH + spriteR.w)/ 2) * (timeLived / 1000.0), 
			200 - spriteR.h / 2 );
      } else if ( 1000 < timeLived && timeLived < 3000 ) {
	pos = Vector2D( ( SCREEN_WIDTH - spriteR.w ) / 2, 
			200 - spriteR.h / 2 );
      } else {
	pos = Vector2D( ((SCREEN_WIDTH - spriteR.w)/2) + 
			((SCREEN_WIDTH + spriteR.w)/2)*((timeLived-3000)/2000.0), 
			200 - spriteR.h / 2 );
      }
      break;
    }
  case BANNER_MODE_FROM_TOP:
    {
      if ( timeLived < 1000 ) {
	pos = Vector2D( (SCREEN_WIDTH - spriteR.w)/2,
			-(spriteR.h) + (200 + spriteR.h/2) * (timeLived / 1000.0) );
      } else if ( 1000 < timeLived && timeLived < 3000 ) {
	pos = Vector2D( (SCREEN_WIDTH - spriteR.w)/2,
			200 - (spriteR.h / 2 ) );
      } else {
	pos = Vector2D( (SCREEN_WIDTH - spriteR.w)/2,
			200 - (spriteR.h / 2) + 
			(200 + (spriteR.h / 2))*((timeLived-3000)/2000.0) );
      }
      break;
    }
  case BANNER_MODE_ITEM_COLLECTED_SINGLE_PLAYER:
  case BANNER_MODE_ITEM_COLLECTED_PLAYER_ONE:
  case BANNER_MODE_ITEM_COLLECTED_PLAYER_TWO:
    {
      break;
    }
  default:
    {
      break;
    }
  }
}
    
bool Banner::movingAway() {
  return ( 3000 <= timeLived );
}

void Banner::draw(SDL_Renderer *screen) {
  SDL_Rect r;
  r.x = lroundf(pos.getX());
  r.y = lroundf(pos.getY());
  r.w = spriteR.w;
  r.h = spriteR.h;
  //SDL_BlitSurface( sprite, 0, screen, &r );
  SDL_RenderCopy(screen, sprite, 0, &r);

  if ( bonus != BANNER_BONUS_NONE &&
       1000 < timeLived && timeLived < 3000 ) {
    r.x = SCREEN_WIDTH / 2 - spriteBonusR.w / 2;
    r.y = 250;
    r.w = spriteBonusR.w;
    r.h = spriteBonusR.h;
    //SDL_BlitSurface( spriteBonus, 0, screen, &r );
    SDL_RenderCopy(screen,
                   spriteBonus,
                   0,
                   &r);
  }
}
