#ifndef LTEXTURE_H
#define LTEXTURE_H
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

using namespace std;

// texture wrapper class 
class LTexture
{
	public:
		// initializes variables 
		LTexture(); 
		
		// deallocates memory 
		~LTexture(); 
		
		// loads image at specified path 
		bool loadFromFile( string filePath, SDL_Renderer* gRenderer ); 
		
		// creates image from font string 
		bool loadFromRenderedText( SDL_Renderer* gRenderer, 
			string textureText, SDL_Color textColor, TTF_Font* gFont ); 
		
		// deallocates texture 
		void free();
		
		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		
		// set blending (?)
		void setBlendMode( SDL_BlendMode blending ); 
		
		void setAlpha( Uint8 alpha ); 
		
		// render an image at a given point 
		void render( int x, int y, SDL_Renderer* gRenderer, 
			SDL_Rect* clip = NULL, double angle = 0.0, 
			SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE ); 
/*		
		// renders texture at given point, inside a rectangle 
		void render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip ); 
		
		// renders texture with capability of rotating and flipping on a center point 
		void render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip = NULL, 
			double angle = 0.0, SDL_Point* center = NULL, 
			SDL_RendererFlip flip = SDL_FLIP_NONE );
*/		
		// gets image dimensions 
		int getWidth(); 
		int getHeight(); 
	private:
		// the actual hardware texture 
		SDL_Texture* mTexture; 
		
		// image dimensions 
		int mWidth; 
		int mHeight; 
}; 

#endif
