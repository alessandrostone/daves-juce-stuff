/*
 *  MyRadioButtons.cpp
 *  haydxn_tutorial
 *
 *  Created by David Rowland on 15/10/2008.
 *  Copyright 2008 UWE. All rights reserved.
 *
 */


#include "MyRadioButtons.h"

MyRadioButtons::MyRadioButtons(int numButtons) 
{	
	String buttonPrefix("Button ");
	
		for (int i = 0; i < numButtons; i++)
		{
			String number(i+1);
			radioGroup.add(new TextButton(number, buttonPrefix + number));	// Create the new TextButton objects
					
			addAndMakeVisible(radioGroup[i]);					// Add buttons to the component
			radioGroup[i]->setClickingTogglesState (true);		// Make button behaviour toggle
			radioGroup[i]->addButtonListener(this);				// Add button listners
			radioGroup[i]->setConnectedEdges(15);
		}
}

MyRadioButtons::~MyRadioButtons ()
{
	deleteAllChildren();   // This will remove all the contained components, and delete
	// the objects for you.
}

void MyRadioButtons::resized ()
{
	// Set positions of the buttons
	for (int i = 0; i < radioGroup.size(); i++) // <<MR>>
	{
		// no need for a border, you can leave that to the parent component
		radioGroup[i]->setBounds (17*i, 0, 15, 15);
		
	}
}

void MyRadioButtons::paint (Graphics& g)
{
	
}

void MyRadioButtons::buttonClicked (Button* button)
{
	
}