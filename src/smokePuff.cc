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

#include "smokePuff.h"
#include "surfaceDB.h"
#include "global.h"

SmokePuff::SmokePuff( Vector2D position, Vector2D velocity, SmokePuffTypes whichType ) {
  
  smokePuffType = whichType;
  pos = position;
  vel = velocity;

  sprite = surfaceDB.loadSurface( FN_SMOKE_PUFF[ smokePuffType ],
				  SMOKE_PUFF_ALPHA_BLENDING );

  SDL_QueryTexture( sprite, NULL, NULL, &spriteR.w, &spriteR.h);

  expired = false;
  nrAnimStages = spriteR.w / spriteR.h;
  timePerStage = LIFETIME_SMOKE_PUFF[ smokePuffType ] / nrAnimStages;
  actAnimStage = 0;
  timeLived = 0;
  timeNextAnimStage = timePerStage;
}

SmokePuff::~SmokePuff() {}

void SmokePuff::update( int dT ) {
  pos += vel * dT / 1000.0;
  timeLived += dT;
  if ( timeLived > timeNextAnimStage ) {
    timeNextAnimStage += timePerStage;
    actAnimStage++;
    if (actAnimStage == nrAnimStages) expired = true;
  }
}

void SmokePuff::drawSmokePuff( SDL_Renderer *screen ) {
  if (expired) return;

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
