#include "LTexture.h"
#include <iostream>

using namespace std; 

LTexture::LTexture()
{
	// initialize 
	mTexture = NULL; 
	mWidth = 0; 
	mHeight = 0; 
}

LTexture::~LTexture()
{
	// deallocate
	free(); 
}

bool LTexture::loadFromFile( string filePath, SDL_Renderer* gRenderer )
{
	// get rid of preexisting texture 
	free(); 
	
	SDL_Texture* newTexture = NULL; 
	
	SDL_Surface* loadedSurface = IMG_Load( filePath.c_str() ); 
	if( loadedSurface == NULL )
	{
		cout << "unable to load surface" << endl; 
	} else
	{
		// color key the image with cyan 
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xff, 0xff ) ); 
		
		newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface ); 
		if( newTexture == NULL )
		{
			cout << "unable to load texture from surface " << endl; 
		} else 
		{
			mWidth = loadedSurface->w; 
			mHeight = loadedSurface->h; 
		}
		
		SDL_FreeSurface( loadedSurface ); 
	}
	
	// return success 
	mTexture = newTexture; 
	return mTexture != NULL; 
}


bool LTexture::loadFromRenderedText( SDL_Renderer* gRenderer, string textureText, SDL_Color textColor, TTF_Font* gFont )
{
	// get rid of preexisting texture
	free(); 
	
	// render text surface 
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor ); 
	if( textSurface == NULL )
	{
		cout << "Unable to render text surface" << endl; 
	}
	else
	{
		// create texture from surface pixels 
		mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface ); 
		if( mTexture == NULL )
		{
			cout << "Unable to create texture from rendered text" << endl; 
		}
		else
		{
			mWidth = textSurface->w; 
			mHeight = textSurface->h; 
		}
		// free the old surface 
		SDL_FreeSurface( textSurface ); 
	}
	
	// return success 
	return mTexture != NULL; 
}

void LTexture::free()
{
	// free texture if it exists 
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture ); 
		mTexture = NULL; 
		mWidth = 0; 
		mHeight = 0; 
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	// modulate texture 
	SDL_SetTextureColorMod( mTexture, red, green, blue ); 
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	// set blending function 
	SDL_SetTextureBlendMode( mTexture, blending ); 
}

void LTexture::setAlpha( Uint8 alpha )
{
	// modulate alpha 
	SDL_SetTextureAlphaMod( mTexture, alpha ); 
}

void LTexture::render( int x, int y, SDL_Renderer* gRenderer, 
			SDL_Rect* clip, double angle, 
			SDL_Point* center, SDL_RendererFlip flip )
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight }; 
	if( clip != NULL )
	{
		renderQuad.w = clip->w; 
		renderQuad.h = clip->h; 
	}
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip ); 
}
/*
void LTexture::render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip )
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight }; 
	// render to screen 
	if( clip != NULL )
	{
		renderQuad.w = clip->w; 
		renderQuad.h = clip->h; 
	}
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad ); 
}

void LTexture::render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip = NULL, 
			double angle = 0.0, SDL_Point* center = NULL, 
			SDL_RendererFlip flip = SDL_FLIP_NONE )
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight }; 
	if( clip != NULL )
	{
		renderQuad.w = clip->w; 
		renderQuad.h = clip->h; 
	}
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip ); 
}
*/
int LTexture::getWidth()
	{ return mWidth; }

int LTexture::getHeight()
	{ return mHeight; }





