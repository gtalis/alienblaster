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

#include "surfaceDB.h"
#include <fstream>
#include <iostream>

SurfaceDB surfaceDB;

SurfaceDB::SurfaceDB( Uint8 transparentR,
		      Uint8 transparentG,
		      Uint8 transparentB ) {
  transR = transparentR;
  transG = transparentG;
  transB = transparentB;
  mRenderer = NULL;
}

SurfaceDB::~SurfaceDB() {
  StringTextureMap::iterator pos;
  // free all surfaces
  for ( pos = surfaceDB.begin(); pos != surfaceDB.end(); ++pos ) {
    SDL_DestroyTexture( pos->second );
  }
}

void SurfaceDB::setRenderer(SDL_Renderer *renderer)
{
    this->mRenderer = renderer;
}

SDL_Texture *SurfaceDB::loadSurface( string fn, bool alpha ) {
  SDL_Texture *searchResult = getSurface( fn );
  if ( searchResult ) {
    return searchResult;
  }

  // open the file for reading
  ifstream inputFile ( fn.c_str(), ios::in);
  if (!inputFile.good()) {
    cout << "ERROR: file " << fn << " does not exist!" << endl;
    exit(1);
  }
  
  SDL_Surface *newSurface = SDL_LoadBMP( fn.c_str() );
  SDL_SetColorKey( newSurface, SDL_TRUE, 
		   SDL_MapRGB(newSurface->format, transR, transG, transB) );


  SDL_Texture *newTexture = SDL_CreateTextureFromSurface(mRenderer, newSurface);
  if ( alpha ) {
    SDL_SetTextureAlphaMod( newTexture, 128 );
  }
  
  SDL_FreeSurface(newSurface);

  surfaceDB[ fn ] = newTexture;
  return newTexture;
}

SDL_Texture *SurfaceDB::getSurface( string fn ) {
  if ( surfaceDB.empty() ) {
    return 0;
  } else {
    StringTextureMap::iterator pos = surfaceDB.find( fn );
    if ( pos == surfaceDB.end() ) {
      return 0;
    } else {
      return pos->second;
    }
  }
}

