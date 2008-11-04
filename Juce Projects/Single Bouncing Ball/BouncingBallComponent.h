/*
 *  BouncingBallComponent.h
 *  BouncingBallAudio
 *
 *  Created by David Rowland on 04/11/2008.
 *  Copyright 2008 UWE. All rights reserved.
 *
 */

#include <juce/juce.h>

/** This listens for the balls hitting the edge of the window.
 */
class BouncingBallListener
	{
	public:
		virtual void ballCollision (class BouncingBallComponent* ball) = 0;
	};


//==============================================================================

class BouncingBallComponent :	public Component,
								public Timer
{
    Colour colour;
    float x, y, dx, dy;
	SortedSet <void*> ballListeners;
	
public:
	
    BouncingBallComponent()
    {		
        // Initial positions on screen
		x = Random::getSystemRandom().nextFloat() * 100.0f;
        y = Random::getSystemRandom().nextFloat() * 100.0f;
		
		// Speeds
        dx = Random::getSystemRandom().nextFloat() * 4.0f - 2.0f;
        dy = Random::getSystemRandom().nextFloat() * 4.0f - 2.0f;
		
		// Colour and size
        colour = Colour (Random::getSystemRandom().nextInt())
		.withAlpha (0.5f)
		.withBrightness (0.7f);
		
        int size = 10 + Random::getSystemRandom().nextInt (30);
        setSize (size, size);
		
        startTimer (40);
    }
	
    ~BouncingBallComponent()
    {
    }
	
    void paint (Graphics& g)
    {
        g.setColour (colour);
        g.fillEllipse (x - getX(), y - getY(), getWidth() - 2.0f, getHeight() - 2.0f);
    }
	
    void timerCallback()
    {				
        bool bounced = false;
		
		// Set new co-ordinates
		x += dx;
        y += dy;
		
		// Ball hits left side of window
        if (x < 0)
		{
			// Make x movement positive
            dx = fabsf (dx);
			bounced = true;
		}
		
		// Ball hits right side of window
        if (x > (getParentWidth()-getWidth()) )
		{
            // Make x movement negative
			dx = -fabsf (dx);
			bounced = true;
		}
		
		// Ball hits top of window
        if (y < 0)
		{
            // Make y movement positive
			dy = fabsf (dy);
			bounced = true;
		}
		
		// Ball hits bottom of window
        if (y > (getParentHeight()-getHeight()) )
		{
            // Make y movement negative
			dy = -fabsf (dy);
			bounced = true;
		}
		
		// Move ball to new co-ordinates
        setTopLeftPosition ((int) x, (int) y);
		
		// If the ball has hit a side:
		if (bounced) 
			sendCollisionMessage();
    }
	
    bool hitTest (int x, int y)
    {
        return false;
    }
	
	void addListener (BouncingBallListener* const newListener) throw()
	{		
		if (newListener != 0)
			ballListeners.add (newListener);
	}
	
	void removeListener (BouncingBallListener* const listener) throw()
	{		
		ballListeners.removeValue (listener);
	}
	
	void sendCollisionMessage()
	{
		for (int i = ballListeners.size(); --i >= 0;)
        {
            BouncingBallListener* const bl = (BouncingBallListener*) ballListeners[i];
			
            if (bl != 0)
            {
                bl->ballCollision(this);
			}
        }
	}
};
