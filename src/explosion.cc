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

#include "explosion.h"
#include "surfaceDB.h"
#include "mixer.h"
#include "global.h"

Explosion::Explosion(string fn, const Vector2D &position, 
		     const Vector2D &velocity, const ExplosionTypes &explosionType) {
  sprite = surfaceDB.loadSurface(fn);
  SDL_QueryTexture(sprite, NULL, NULL, &spriteR.w, &spriteR.h);

  nrAnimStages = spriteR.w / spriteR.h;
  expired = false;

  sndExplosion = mixer.loadSample( FN_SOUND_EXPLOSION_NORMAL );  
  mixer.playSample( sndExplosion, 0 );

  this->explosionType = explosionType;
  pos = position;
  vel = velocity;
  
  switch (explosionType) {
  case EXPLOSION_NORMAL_AIR: 
  case EXPLOSION_NORMAL_GROUND:
    {
      timePerStage = LIFETIME_EXPL_NORMAL / nrAnimStages;
      break;
    }
  default: 
    {
      timePerStage = 0;
      actAnimStage = nrAnimStages;
      break;
    }
  }

  actAnimStage = 0;
  timeLived = 0;
  timeNextAnimStage = timePerStage;
}

Explosion::~Explosion() {}

void Explosion::update( int dT ) {
  if ( scrollingOn ) pos += vel * dT / 1000.0;
  timeLived += dT;
  if ( timeLived > timeNextAnimStage ) {
    timeNextAnimStage += timePerStage;
    actAnimStage++;
    if (actAnimStage == nrAnimStages) expired = true;
  }
}

void Explosion::drawAirExplosion(SDL_Renderer *screen) {
  if (expired) return;
  if ( ! ( explosionType == EXPLOSION_NORMAL_AIR ) ) return;

  SDL_Rect dest;
  dest.x = lroundf(pos.getX()) - spriteR.w / (2*nrAnimStages);
  dest.y = lroundf(pos.getY()) - spriteR.h / 2;
  dest.w = spriteR.w / nrAnimStages;
  dest.h = spriteR.h;

  SDL_Rect src;
  src.x = actAnimStage * spriteR.w / nrAnimStages;
  src.y = 0;
  src.w = spriteR.w / nrAnimStages;
  src.h = spriteR.h;

  //SDL_BlitSurface( sprite, &src, screen, &dest );
  SDL_RenderCopy(screen, sprite, &src, &dest );
}

void Explosion::drawGroundExplosion(SDL_Renderer *screen) {
  if (expired) return;
  if ( ! ( explosionType == EXPLOSION_NORMAL_GROUND ) ) return;

  SDL_Rect dest;
  dest.x = lroundf(pos.getX()) - spriteR.w / (2*nrAnimStages);
  dest.y = lroundf(pos.getY()) - spriteR.h / 2;
  dest.w = spriteR.w / nrAnimStages;
  dest.h = spriteR.h;

  SDL_Rect src;
  src.x = actAnimStage * spriteR.w / nrAnimStages;
  src.y = 0;
  src.w = spriteR.w / nrAnimStages;
  src.h = spriteR.h;

  //SDL_BlitSurface( sprite, &src, screen, &dest );
  SDL_RenderCopy(screen, sprite, &src, &dest );
}
