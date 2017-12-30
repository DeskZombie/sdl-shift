#include <SDL.h> 
#include "LTimer.h" 

LTimer::LTimer()
{
	// initialize the variables 
	mStartTicks = 0; 
	mPausedTicks = 0; 
	
	mPaused = false; 
	mStarted = false; 
}

void LTimer::start()
{
	// start the timer 
	mStarted = true; 
	
	// unpause the timer 
	mPaused = false; 
	
	// get the current clock time 
	mStartTicks = SDL_GetTicks(); 
	mPausedTicks = 0; 
	
}


void LTimer::stop() 
{
	// stop the timer 
	mStarted = false; 
	
	// unpause the timer 
	mPaused = false; 
	
	// clear the tick variables 
	mStartTicks = 0; 
	mPausedTicks = 0; 
	
}

void LTimer::pause()
{
	// if the timer is running and isn't already paused 
	if( mStarted && !mPaused )
	{
		// unpause the timer 
		mPaused = true; 
		
		// Calculate the paused ticks 
		mPausedTicks = SDL_GetTicks() - mStartTicks; 
		
		// reset the paused ticks 
		mStartTicks = 0; 
		
	}
}

void LTimer::unpause()
{
	// if the timer is running and paused 
	if( mStarted && mPaused )
	{
		// unpause the timer 
		mPaused = false; 
		
		// reset the starting ticks 
		mStartTicks = SDL_GetTicks() - mPausedTicks; 
		
		// reset the paused ticks 
		mPausedTicks = 0; 
	}
}

Uint32 LTimer::getTicks()
{
	// the actual timer time 
	Uint32 time = 0; 
	// if the timer is running 
	if( mStarted )
	{
		// if the timer is paused 
		if( mPaused )
		{
			// return the number of ticks when the timer was paused 
			time = mPausedTicks; 
		}
		else
		{
			// return the current time minus the start time 
			time = SDL_GetTicks() - mStartTicks; 
		}
	}
	
	return time; 
}

bool LTimer::isStarted()
{
	return mStarted; 
}

bool LTimer::isPaused()
{
	return mPaused; 
}
