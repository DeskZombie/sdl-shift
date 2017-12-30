#ifndef LTIMER_H
#define LTIMER_H

class LTimer
{
	public: 
		// initializes variables 
		LTimer(); 
		
		// the various clock action 
		void start(); 
		void stop(); 
		void pause(); 
		void unpause(); 
		
		// gets the timer's time 
		Uint32 getTicks(); 
		
		// checks the status of the timer 
		bool isStarted(); 
		bool isPaused(); 
		
	private: 
		// the clock time when the timer started 
		Uint32 mStartTicks; 
		
		// the ticks stored when the timer was paused 
		Uint32 mPausedTicks; 
		
		// the timer status 
		bool mPaused; 
		bool mStarted; 
		
}; 

#endif 
