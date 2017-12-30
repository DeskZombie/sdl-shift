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
const int SPEED_INCREASE_INTERVAL = 1; 
const SDL_Color COLOR_BASE = { 0, 0, 255, 255 }; // blue 
const SDL_Color COLOR_SHIFT = { 255, 255, 0, 255 }; // yellow 
const SDL_Color COLOR_COOLDOWN = { 0, 255, 0, 255 }; // green 
const SDL_Color COLOR_FAIL = { 255, 0, 0, 255 }; // red 
const SDL_Color COLOR_FONT = { 0, 0, 64, 255 }; // dark blue 
const Uint32 COOLDOWN_FAST = 100; 
const Uint32 COOLDOWN_SLOW = 500; 
const int HEALTH_MAX = 10; 
const int TARGET_EASY = SPEED_INCREASE_INTERVAL * 10; 
const int TIMER_EASY = TARGET_EASY + 5; 

struct GameText 
{
	TTF_Font* font = NULL; 
	LTexture fontTexture; 
	SDL_Color fontColor = { 127, 127, 127 }; 
	string text = "*SampleText"; 
	int number = 0; 
}; 

enum GameState 
{
	menu, 
	playing, 
	paused, 
	gameOver, 
	win
}; 

SDL_Window* window = NULL; 
SDL_Renderer* renderer = NULL; 
SDL_Event e; 

// game variables requiring load-time go here 
GameText score; 
GameText health; 
// GameText boost; 
GameText target; 
GameText burnOut; 
LTexture background[2]; 
GameText targetTime; 
LTimer targetTimer; 
GameText winner; 
GameText replay; 
SDL_Rect playerBox = { (WINDOW_WIDTH - PLAYER_SIZE)/2, (WINDOW_HEIGHT/2) + GOAL_HEIGHT - PLAYER_SIZE, 
		PLAYER_SIZE, PLAYER_SIZE }; 

bool init(); 
bool loadMedia(); 
void close(); 
// check collision between two rectangles 
bool checkCollision( SDL_Rect, SDL_Rect ); 
// reset game variables 
void resetGame( GameState & ); 

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
	
	SDL_Rect goalBox = { WINDOW_WIDTH, WINDOW_HEIGHT / 2, GOAL_WIDTH, GOAL_HEIGHT };  
	SDL_Point bgPos1 = { 0, 0 }; 
	SDL_Point bgPos2 = { WINDOW_WIDTH, 0 }; 
	int zCoolDownLength = COOLDOWN_FAST; 
	GameState gameState = playing; 
	
	bool previousGoalTaken = false; 
	bool previousSpeedTaken = false; 
	
	// reset variables 
	resetGame( gameState ); 
	
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
				
				switch( gameState ) 
				{
					case playing: 
						// if z is pressed & cooldown is over, change the player's color 
						if( e.key.keysym.sym == SDLK_z && !zTimer.isStarted() && !zCoolDown.isStarted() ) 
						{
							zTimer.start(); 
						} 
						break; 
					case gameOver: case win: 
						if( e.key.keysym.sym == SDLK_z ) 
						{
							// reset the game w/ z if the game ended 
							resetGame( gameState ); 
						}
						break; 
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
		
		
		
		switch( gameState ) 
		{
			case playing: 
				// display goal 
				SDL_SetRenderDrawColor( renderer, 0xff, 0x88, 0x00, 0x00 ); 
				SDL_RenderFillRect( renderer, &goalBox ); 
				// display rectangle 
				if( !zCoolDown.isStarted() ) 
				{
					if( zTimer.isStarted() ) 
					{
						// set color to the shift color 
						SDL_SetRenderDrawColor( renderer, COLOR_SHIFT.r, COLOR_SHIFT.g, COLOR_SHIFT.b, 0xff ); 
						
						// check if the goal collided with the player if z is open 
						if( checkCollision( goalBox, playerBox ) ) 	
						{
							// up the speed if z was pressed on the goal 
							if( !previousGoalTaken ) 
							{
								// increment score 
								score.text = "Shift: " + to_string( ++score.number ); 
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
						// set color to base color 
						SDL_SetRenderDrawColor( renderer, COLOR_BASE.r, COLOR_BASE.g, COLOR_BASE.b, 0xff ); 
					}
				}
				else
				{
					// if player doesn't catch goal before zTimer ends and 
					// speed hasn't been lowered and 
					// the previous goal wasn't taken 
					if( !previousSpeedTaken ) 
					{
						
						if( !previousGoalTaken ) 
						{
							// lower the health and update the health text 
							health.number -= score.number / SPEED_INCREASE_INTERVAL; 
							health.text = "Integrity: " + to_string( health.number ); 
							
							// check for gameOver condition 
							if( health.number <= 0 ) 
							{
								// set the number to zero to make sure the text doesn't show negative 
								health.number = 0; 
								health.text = "BURNED OUT"; 
								gameState = gameOver; 
														
							}
							
							// lower speed by current-speed-based division of speed increase interval 
							score.number -= score.number / SPEED_INCREASE_INTERVAL; 
							score.text = "Shift: " + to_string( score.number ); 
							previousSpeedTaken = true; 
							
							
							
						}
					}
					
					if( !previousSpeedTaken ) 
					{
						SDL_SetRenderDrawColor( renderer, COLOR_COOLDOWN.r, COLOR_COOLDOWN.g, COLOR_COOLDOWN.b, 0xff ); 
						zCoolDownLength = COOLDOWN_FAST; 
					}
					else
					{
						SDL_SetRenderDrawColor( renderer, COLOR_FAIL.r, COLOR_FAIL.g, COLOR_FAIL.b, 0xff ); 
						zCoolDownLength = COOLDOWN_SLOW; 
					}
					
					if( zCoolDown.getTicks() / zCoolDownLength ) 
					{
						zCoolDown.stop(); 
						previousGoalTaken = false; 
						previousSpeedTaken = false; 
					}
				}
				
				// if the targetTimer number reaches zero, check shift 
				if( targetTime.number == 0 ) 
				{
					// if the player hasn't reached shift target, game over 
					if( score.number < target.number ) 
					{
						gameState = gameOver; 
					}
					// otherwise, game win 
					else
					{
						gameState = win; 
					}
				}
				
				SDL_RenderFillRect( renderer, &playerBox ); 
				
				// update the score texture 
				score.fontTexture.loadFromRenderedText( renderer, score.text, score.fontColor, score.font ); 
				// render the score texture 
				score.fontTexture.render( (WINDOW_WIDTH - score.fontTexture.getWidth())/2, 
										  0, renderer ); 		
				
				health.fontTexture.loadFromRenderedText( renderer, health.text, health.fontColor, health.font ); 
				health.fontTexture.render( 0, 0, renderer ); 
				target.fontTexture.render( 100, 100, renderer ); 
				targetTime.number = TIMER_EASY - targetTimer.getTicks() / 1000; 
				targetTime.text = "Timer: " + to_string( targetTime.number ); 
				targetTime.fontTexture.loadFromRenderedText( renderer, targetTime.text, targetTime.fontColor, targetTime.font ); 
				targetTime.fontTexture.render( 100, 115, renderer ); 
				break; 
			case gameOver: 
				burnOut.fontTexture.render( (WINDOW_WIDTH - burnOut.fontTexture.getWidth())/2, 
											(WINDOW_HEIGHT - burnOut.fontTexture.getHeight())/2, renderer ); 
				break; 
			case win: 
				// render player exit motion until it leaves the screen 
				if( playerBox.y + playerBox.h > 0 ) 
				{
					playerBox.y -= 1; 
					SDL_SetRenderDrawColor( renderer, COLOR_BASE.r, COLOR_BASE.g, COLOR_BASE.b, COLOR_BASE.a ); 
					SDL_RenderFillRect( renderer, &playerBox ); 
				}
				else 
				{
					// render the replay cue below the win text 
					replay.fontTexture.render( ( WINDOW_WIDTH - replay.fontTexture.getWidth() )/2, ( WINDOW_HEIGHT - replay.fontTexture.getHeight() )/2 + 15, renderer ); 
				}
				
				// render win text 
				winner.fontTexture.render( ( WINDOW_WIDTH - winner.fontTexture.getWidth() ) / 2, ( WINDOW_HEIGHT - winner.fontTexture.getHeight() ) / 2, renderer ); 
				
				break; 
		}
		
		
		
		
		
		
		
		
		
		
		
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
	
	score.text = "Shift: " + to_string( score.number ); 
	score.fontColor = COLOR_FONT; 
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
	
	health.number = HEALTH_MAX; 
	health.text = "Integrity: " + to_string( health.number ); 
	health.font = TTF_OpenFont( "Fairfax.ttf", 24 ); 
	health.fontColor = COLOR_FONT; 
	if( health.font == NULL ) 
	{
		cout << "health font failed" << endl; 
		success = false;
	}
	if( !health.fontTexture.loadFromRenderedText( renderer, health.text, health.fontColor, health.font ) ) 
	{
		success = false;
		cout << "failed to render health text" << endl; 
	}
	
	burnOut.text = "Press z to try again"; 
	burnOut.font = TTF_OpenFont( "Fairfax.ttf", 48 ); 
	burnOut.fontColor = COLOR_FONT; 
	if( health.font == NULL ) 
	{
		cout << "burnOut font failed" << endl; 
		success = false; 
	}
	if( !burnOut.fontTexture.loadFromRenderedText( renderer, burnOut.text, burnOut.fontColor, burnOut.font ) ) 
	{
		cout << "failed to render burnOut text" << endl; 
		success = false; 
	}
	/*
	boost.number = 0; 
	boost.text = "Boost: " + to_string( boost.number ) + " "; 
	*/ 
	
	target.number = TARGET_EASY; 
	target.text = "Target: " + to_string( target.number ); 
	target.font = TTF_OpenFont( "Fairfax.ttf", 20 ); 
	target.fontColor = COLOR_FONT; 
	if( target.font == NULL ) 
	{
		cout << "target font failed" << endl; 
		success = false; 
	}
	if( !target.fontTexture.loadFromRenderedText( renderer, target.text, target.fontColor, target.font ) ) 
	{
		cout << "failed to render target text" << endl; 
		success = false; 
	}
	
	targetTime.text = "Timer: "; 
	targetTime.font = TTF_OpenFont( "Fairfax.ttf", 20 ); 
	targetTime.fontColor = COLOR_FONT; 
	if( targetTime.font == NULL ) 
	{
		cout << "targetTime font failed" << endl; 
		success = false; 
	}
	if( !targetTime.fontTexture.loadFromRenderedText( renderer, targetTime.text, targetTime.fontColor, targetTime.font ) ) 
	{
		cout << "failed to render targetTimer text" << endl; 
		success = false; 
	}
	
	winner.text = "Target shift reached. Returning to home base. "; 
	winner.font = TTF_OpenFont( "Fairfax.ttf", 18 ); 
	winner.fontColor = COLOR_FONT; 
	if( winner.font == NULL ) 
	{
		cout << "winner font failed" << endl; 
		success = false; 
	}
	if( !winner.fontTexture.loadFromRenderedText( renderer, winner.text, winner.fontColor, winner.font ) ) 
	{
		cout << "failed to render winner texture" << endl; 
		success = false; 
	}
	
	replay.font = TTF_OpenFont( "Fairfax.ttf", 24 ); 
	replay.text = "Press z to replay"; 
	replay.fontColor = COLOR_FONT; 
	if( replay.font == NULL ) 
	{
		cout << "replay font failed" << endl; 
		success = false; 
	}
	if( !replay.fontTexture.loadFromRenderedText( renderer, replay.text, replay.fontColor, replay.font ) ) 
	{
		cout << "failed to render replay texture" << endl; 
		success = false; 
	}
	
	background[ 0 ].loadFromFile( "parg-bg.png", renderer ); 
	background[ 1 ] = background[ 0 ]; 
	
	return success; 
	
}

void close() 
{
	TTF_CloseFont( score.font ); 
	TTF_CloseFont( health.font ); 
	// TTF_CloseFont( boost.font ); 
	TTF_CloseFont( burnOut.font ); 
	TTF_CloseFont( target.font ); 
	TTF_CloseFont( targetTime.font ); 
	TTF_CloseFont( winner.font ); 
	TTF_CloseFont( replay.font ); 
	
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

void resetGame( GameState &gameState ) 
{
	// reset targetTimer and target time 
	targetTimer.start(); 
	targetTime.number = TIMER_EASY; 
	
	// reset player position 
	playerBox.x = (WINDOW_WIDTH - PLAYER_SIZE)/2; 
	playerBox.y = (WINDOW_HEIGHT/2) + GOAL_HEIGHT - PLAYER_SIZE; 
	
	// reset number values 
	score.number = 0; 
	health.number = HEALTH_MAX; 
	
	// reset text values 
	score.text = "Shift: " + to_string( score.number ); 
	health.text = "Integrity: " + to_string( health.number ); 
	
	// re-render values 
	score.fontTexture.loadFromRenderedText( renderer, score.text, score.fontColor, score.font ); 
	health.fontTexture.loadFromRenderedText( renderer, health.text, health.fontColor, health.font ); 
	
	gameState = playing; 
}

