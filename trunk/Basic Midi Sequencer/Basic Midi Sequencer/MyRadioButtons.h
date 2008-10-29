/*
 *  DavesRadioButtons.h
 *  haydxn_tutorial
 *
 *  Created by David Rowland on 15/10/2008.
 *  Copyright 2008 UWE. All rights reserved.
 *
 */

#ifndef _MyRadioButtons_H_
#define _MyRadioButtons_H_


/*
 Using <juce/juce.h> here assumes the juce source tree is accessible via one
 of the following paths (depending on the Xcode project settings)
 
 /Developer/SDKs/MacOSX10.5.sdk/usr/include/
 /Developer/SDKs/MacOSX10.4u.sdk/usr/include/
 /usr/include
 
 On other platforms just use juce.h making it available in the search path 
 */
#include <juce/juce.h>

class MyRadioButtons  :  public Component,
						 public ButtonListener
	
	// Here we specify any base classes that
	// give this type predefined characteristics.
	// Naturally, this is a Component, but we could
	// also inherit other qualities. For example, if
	// we want to respond to button presses, we can
	// inherit 'ButtonListener', by changing it thus:
	// e.g.
	// public Component,
	// public ButtonListener
	
	// (Notice that they're separated by a comma, and nothing comes after the
	//  final one in the list - i.e. no semicolon, because the next character
	//  must be the '{' denoting the start of the class body).
	
	// We can inherit many different classes from Juce (or classes we make
	// ourselves), but be aware that some base classes require you to define
	// some function bodies before it will allow your app to compile. These
	// functions are called 'pure virtual' functions - an example would be
	// 'buttonClicked' in ButtonListener. Some base classes will provide many
	// other virtual functions - not just pure virtual ones - which you can
	// define if you choose, but you do not have to do so.
	{
	private:
		
		//==============================================================================
				
		//TooltipWindow tooltipWindow;   // To add tooltips to an application, you

		
	public:
		
		Array<TextButton*> radioGroup;

		//==============================================================================
		MyRadioButtons (int numButtons);		
		~MyRadioButtons ();
		
		//==============================================================================
		void resized ();
		
		void paint (Graphics& g);
		
		// Pure Virtual function to listen to button clicks
		void buttonClicked (Button* button);
		
		//==============================================================================
	};

#endif//_MyRadioButtons_H_ 