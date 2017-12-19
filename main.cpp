#include <iostream> 
#include <SDL.h>
#include <SDL_image.h> 
#include <SDL_ttf.h> 
#include <SDL_mixer.h> 
#include "../../LTimer.h" 
#include "../../LTexture.h" 
#define PI 3.14159265

using namespace std; 

const int WINDOW_WIDTH = 640; 
const int WINDOW_HEIGHT = 400; 
const int FRAME_RATE = 60; 
const int TICKS_PER_FRAME = 1000 / FRAME_RATE; 
const int PLAYER_SIZE = 50; 
const int GOAL_WIDTH = 25; 
const int GOAL_HEIGHT = 100; 
const int GOAL_VEL = -5; 
const int Z_TIMER_LENGTH = 250; 
const int Z_COOLDOWN = 100; 
const int SPEED_INCREASE_INTERVAL = 5; 

struct GameText 
{
	TTF_Font* font = NULL; 
	LTexture fontTexture; 
	SDL_Color fontColor = { 127, 127, 127 }; 
	string text = "*SampleText"; 
	int number = 0; 
}; 

SDL_Window* window = NULL; 
SDL_Renderer* renderer = NULL; 
SDL_Event e; 

// game variables 
GameText score; 
GameText health; 
LTexture background[2]; 

bool init(); 
bool loadMedia(); 
void close(); 
// check collision between two rectangles 
bool checkCollision( SDL_Rect, SDL_Rect ); 

int main( int argc, char* args[] ) 
{
	bool running = false; 
	if( !init() ) 
	{
		cout << "init failed" << endl; 
	}
	else 
	{
		if( !loadMedia() ) 
		{
			cout << "loadMedia failed" << endl; 
		} 
		else 
			running = true; 
	}
	
	// *******  game variables here ********** 
	LTimer frameTimer; 
	LTimer zTimer; 
	LTimer zCoolDown; 
	SDL_Rect playerBox = { (WINDOW_WIDTH - PLAYER_SIZE)/2, (WINDOW_HEIGHT/2) + GOAL_HEIGHT - PLAYER_SIZE, 
		PLAYER_SIZE, PLAYER_SIZE }; 
	SDL_Rect goalBox = { WINDOW_WIDTH, WINDOW_HEIGHT / 2, GOAL_WIDTH, GOAL_HEIGHT };  
	SDL_Point bgPos1 = { 0, 0 }; 
	SDL_Point bgPos2 = { WINDOW_WIDTH, 0 }; 
	
	
	
	bool previousGoalTaken = false; 
	bool previousSpeedTaken = false; 
	
	// game loop here 
	while( running ) 
	{
		frameTimer.start(); 
		
		// poll the event queue 
		while( SDL_PollEvent( &e ) != 0 ) 
		{
			SDL_FlushEvent( SDL_MOUSEMOTION ); 
			if( e.type == SDL_QUIT ) 
			{
				running = false; 
			} 
			else if( e.type == SDL_KEYDOWN ) 
			{
				// if z is pressed & cooldown is over, change the player's color 
				if( e.key.keysym.sym == SDLK_z && !zTimer.isStarted() && !zCoolDown.isStarted() ) 
				{
					zTimer.start(); 
				} 
			}
		}
		
		// move the goal from right to left 
		goalBox.x += GOAL_VEL + (-score.number / SPEED_INCREASE_INTERVAL); 
		if( goalBox.x + goalBox.w < 0 ) 
		{
			// reset the goal if it leaves the screen 
			goalBox.x = WINDOW_WIDTH; 
		}
		
		SDL_RenderClear( renderer ); 
		
		// render background 
		background[ 0 ].render( bgPos1.x, bgPos1.y, renderer ); 
		bgPos1.x += GOAL_VEL + (-score.number / SPEED_INCREASE_INTERVAL); 
		background[ 1 ].render( bgPos2.x, bgPos2.y, renderer ); 
		bgPos2.x += GOAL_VEL + (-score.number / SPEED_INCREASE_INTERVAL); 
		
		if( bgPos1.x + background[ 0 ].getWidth() < 0 ) 
		{
			bgPos1.x = bgPos2.x + background[ 1 ].getWidth(); 
		} 
		if( bgPos2.x + background[ 1 ].getWidth() < 0 ) 
		{
			bgPos2.x = bgPos1.x + background[ 0 ].getWidth(); 
		}
		
		// display goal 
		SDL_SetRenderDrawColor( renderer, 0xff, 0x88, 0x00, 0x00 ); 
		SDL_RenderFillRect( renderer, &goalBox ); 
		
		// display rectangle 
		if( !zCoolDown.isStarted() ) 
		{
			if( zTimer.isStarted() ) 
			{
				SDL_SetRenderDrawColor( renderer, 0x11, 0x11, 0xbb, 0x00 ); 
				
				// check if the goal collided with the player if z is open 
				if( checkCollision( goalBox, playerBox ) ) 	
				{
					// up the speed if z was pressed on the goal 
					if( !previousGoalTaken ) 
					{
						// increment score 
						score.text = "Speed: " + to_string( ++score.number ); 
						previousGoalTaken = true; 
					}
				}
				if( zTimer.getTicks() / Z_TIMER_LENGTH )
				{
					zTimer.stop(); 
					zCoolDown.start(); 
				}
			}
			else
			{
				SDL_SetRenderDrawColor( renderer, 0x33, 0x33, 0x77, 0x00 ); 
			}
		}
		else
		{
			// if player doesn't catch goal before zTimer ends and 
			// speed hasn't been lowered and 
			// the previous goal wasn't taken 
			if( !previousSpeedTaken && !previousGoalTaken ) 
			{
				// lower speed by current-speed-based division of speed increase interval 
				score.number -= score.number / SPEED_INCREASE_INTERVAL; 
				score.text = "Speed: " + to_string( score.number ); 
				previousSpeedTaken = true; 
			}
			SDL_SetRenderDrawColor( renderer, 0x33, 0x33, 0x77, 0x00 ); 
			if( zCoolDown.getTicks() / Z_COOLDOWN ) 
			{
				zCoolDown.stop(); 
				previousGoalTaken = false; 
				previousSpeedTaken = false; 
			}
		}
		SDL_RenderFillRect( renderer, &playerBox ); 
		
		// update the score texture 
		score.fontTexture.loadFromRenderedText( renderer, score.text, score.fontColor, score.font ); 
		// render the score texture 
		score.fontTexture.render( (WINDOW_WIDTH - score.fontTexture.getWidth())/2, 
								  0, renderer ); 		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		// check to see how many ticks this machine counted on this frame 
		Uint32 ticksThisFrame = frameTimer.getTicks(); 
		
		// if this frame's ticks is under the target ticks, delay the program for the remainder 
		if( ticksThisFrame < TICKS_PER_FRAME ) 
			SDL_Delay( TICKS_PER_FRAME - ticksThisFrame ); 
		
		SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 ); 
		SDL_RenderPresent( renderer ); 
		
		
	}
	
	close(); 
	
	return 0; 
}

bool init() 
{
	bool success = true; 
	
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		cout << "sdl failed to init" << endl; 
		success = false; 
	}
	else 
	{
		window = SDL_CreateWindow( "hop", 
			0, 0, 
			WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN ); 
		if( window == NULL ) 
		{
			cout << "CreateWindow failed" << endl; 
			success = false; 
		}
		else 
		{
			renderer = SDL_CreateRenderer( window, -1, 
				SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ); 
			if( renderer == NULL ) 
			{
				cout << "CreateRenderer failed" << endl; 
				success = false; 
			}
			else 
			{
				SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 ); 
				SDL_RenderPresent( renderer ); 
				
				if( !IMG_Init( IMG_INIT_PNG ) ) 
				{
					cout << "sdl_image failed to init" << endl; 
					success = false; 
				}
				if( TTF_Init() == -1 ) 
				{
					cout << "sdl_ttf failed to init" << endl; 
					success = false; 
				}
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					cout << "sdl_mixer failed to init" << endl; 
					success = false; 
				}
			}
		}
	}
	return success; 
}

bool loadMedia() 
{
	bool success = true; 
	// load assets and check for success 
	
	score.text = "Speed: " + to_string( score.number ); 
	score.fontColor = { 0, 0, 128 }; 
	score.font = TTF_OpenFont( "Fairfax.ttf", 64 ); 
	if( score.font == NULL ) 
	{
		cout << "couldn't load font" << endl; 
		success = false; 
	}
	if( !score.fontTexture.loadFromRenderedText( renderer, score.text, score.fontColor, score.font ) ) 
	{
		cout << "failed to render text" << endl; 
		success = false; 
	}
	
	background[ 0 ].loadFromFile( "parg-bg.png", renderer ); 
	background[ 1 ] = background[ 0 ]; 
	
	return success; 
	
}

void close() 
{
	TTF_CloseFont( score.font ); 
	
	SDL_DestroyRenderer( renderer ); 
	SDL_DestroyWindow( window ); 
	
	renderer = NULL; 
	window = NULL; 
	
	TTF_Quit(); 
	IMG_Quit(); 
	Mix_Quit(); 
	SDL_Quit(); 
}

// uses bounding box collider test to return collision result 
bool checkCollision( SDL_Rect a, SDL_Rect b )
{
	// declare all sides 
	int aTop, aBottom, aLeft, aRight; 
	int bTop, bBottom, bLeft, bRight; 
	
	// define all sides 
	aTop = a.y; 
	aBottom = a.y + a.h; 
	aLeft = a.x; 
	aRight = a.x + a.w; 
	
	bTop = b.y; 
	bBottom = b.y + b.h; 
	bLeft = b.x; 
	bRight = b.x + b.w; 
	
	// perform collision test by process of elimination 
	if( aTop > bBottom ) 
		return false; 
	else if( aBottom < bTop ) 
		return false; 
	else if( aLeft > bRight ) 
		return false; 
	else if( aRight < bLeft ) 
		return false; 
	
	// the axes are colliding, so there is a collision between the rects 
	return true; 
}
