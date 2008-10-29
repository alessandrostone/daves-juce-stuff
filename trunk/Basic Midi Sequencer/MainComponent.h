#ifndef _MAINCOMPONENT_H_
#define _MAINCOMPONENT_H_

#include <juce/juce.h>
#include "MyRadioButtons.h"

// uint32 seems to be defined in multiple places, this is a hack for now..
#define uint32 JUCE_NAMESPACE::uint32

class MainComponent  :	public Component,
						public ButtonListener,
						public ComboBoxListener,
						public SliderListener,
						public LabelListener,
						public Thread
	{
	private:
		StringArray midiDevices;
		MidiOutput* midiOutput;
		ComboBox* midiOutputSelector;
		TextButton* play;
		TextButton* stop;
		Array<Label*> noteVals;
		Array<Slider*> notes;
		TextButton* synthSelection;
		TextButton* stepSelection;
		Slider* rateSlider;
		Slider* synthVol;
		Slider* stepVol;
		Label* text;
		Label* volumeLabel;
		Array<int> noteSeq;
		int	index;
		int rate;
		int i;
		
		Array<MyRadioButtons*> stepSequencer;
		
		
	public:
		//==============================================================================
		juce_UseDebuggingNewOperator // fixes some window compiler issue
		
		/** Create our main component which does all of the app's work.
		 */
		MainComponent () 
			:	Thread(T("Sequencer")),
				midiOutput(0),
				index(0),
				rate(125)
		{		
			
			// simple sequencing example,  using a thread
			
			// get a list of our MIDI output devices
			midiDevices = MidiOutput::getDevices();
			
			// create a combo box (menu) component
			addAndMakeVisible(midiOutputSelector = new ComboBox("MIDI Output Selector"));
			midiOutputSelector->setBounds(10, 10, 270, 20);
			midiOutputSelector->addListener(this);
			
			// populate the menu with our MIDI device names
			for(i = 0; i < midiDevices.size(); i++)
			{
				midiOutputSelector->addItem(midiDevices[i], i+1);
			}
			
			// choose the first item 1 (note that the first item is not 0 in this case for combo boxes)
			midiOutputSelector->setSelectedId(1, false);
			
			// play button
			addAndMakeVisible(play = new TextButton(T("Play")));
			play->setBounds(10, 40, 135, 20);
			play->setConnectedEdges(2);
			play->addButtonListener(this);
			
			// stop button
			addAndMakeVisible(stop = new TextButton(T("Stop")));
			stop->setBounds(145, 40, 135, 20);
			stop->setConnectedEdges(1);
			stop->addButtonListener(this);
			
//			// Create the array for the note values
//			for(i = 0; i < 8; i++)
//			{
//				noteVals.add(new Label(T("Note Val"),T("?")));
//				//addAndMakeVisible(noteVals[i]);
//				noteVals[i]->setBounds(15 + 35*i, 70, 20, 20);
//				noteVals[i]->setEditable(true);
//				noteVals[i]->setText(String(noteSeq[i]), false);
//				noteVals[i]->addListener(this);
//			}
			
			for(i = 0; i < 8; i++)
			{
				notes.add(new Slider(T("Note Val")));
				addAndMakeVisible(notes[i]);
				notes[i]->setSliderStyle(Slider::IncDecButtons);
				notes[i]->setIncDecButtonsMode(Slider::incDecButtonsDraggable_AutoDirection);
				notes[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 25, 30);
				notes[i]->setRange(0, 127, 1);
				notes[i]->setBounds(10 + 35*i, 65, 25, 30);
				notes[i]->addListener(this);
			}
			
			// Synth and step selections
			addAndMakeVisible(synthSelection = new TextButton(T("Synthesizer On")));
			synthSelection->setBounds(10, 220, 135, 20);
			synthSelection->setConnectedEdges(2);
			synthSelection->setClickingTogglesState(true);
			synthSelection->setToggleState(true, false);
			synthSelection->addButtonListener(this);
			
			addAndMakeVisible(stepSelection = new TextButton(T("Step Sequencer On")));
			stepSelection->setBounds(145, 220, 135, 20);
			stepSelection->setConnectedEdges(1);
			stepSelection->setClickingTogglesState(true);
			stepSelection->setToggleState(true, false);
			stepSelection->addButtonListener(this);
			
			// Volume label
			addAndMakeVisible(volumeLabel = new Label(T("Synth and Step Volumes"),T("Volumes:")));
			volumeLabel->setBounds(120, 250, 50, 10);
			
			// Volume Sliders
			addAndMakeVisible(synthVol = new Slider(T("Synthesizer Volume")));
			synthVol->setBounds(10, 270, 135, 10);
			synthVol->setTextBoxStyle(Slider::NoTextBox,false, 0, 0);
			synthVol->setRange(0, 1, 0.001);
			synthVol->setSkewFactor(0.9);
			synthVol->setValue(1);
			
			addAndMakeVisible(stepVol = new Slider(T("Step Sequencer Volume")));
			stepVol->setBounds(145, 270, 135, 10);
			stepVol->setTextBoxStyle(Slider::NoTextBox,false, 0, 0);
			stepVol->setRange(0, 1, 0.001);
			stepVol->setSkewFactor(0.9);
			stepVol->setValue(1);
			
			// Rate slider
			addAndMakeVisible(rateSlider = new Slider(T("Rate Slider")));
			rateSlider->setBounds(10, 290, 270, 20);
			rateSlider->setRange(60, 180, 1);
			rateSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 30, 20);
			rateSlider->setValue(180);
			
			// some text for our status
			addAndMakeVisible(text = new Label(T("Text"),T("Ready...")));
			text->setBounds(10, 100, 270, 20);
			
			// Step Sequencer buttons
			for(i = 0; i < 4; i++)
			{
				stepSequencer.add(new MyRadioButtons(16));
				addAndMakeVisible(stepSequencer[i]);
			}
		
			
			// add some notes to the sequence, noteSeq is an Array<int>
			// Note sequence is: 60, 62, 64, 65, 67, 65, 64, 62
//			noteSeq.add(60);
//			noteSeq.add(62);
//			noteSeq.add(64);
//			noteSeq.add(65);
//			noteSeq.add(67);
//			noteSeq.add(65);
//			noteSeq.add(64);
//			noteSeq.add(62);
//			noteSeq.add(60);
//			noteSeq.add(62);
//			noteSeq.add(64);
//			noteSeq.add(65);
//			noteSeq.add(67);
//			noteSeq.add(65);
//			noteSeq.add(64);
//			noteSeq.add(62);
			
			// This is a bit of a hack as only every odd note gets played
			// This means that the drum sequencer has twice the resolution of the 
			// synth
			
			noteSeq.add(60);
			noteSeq.add(0);
			noteSeq.add(62);
			noteSeq.add(0);
			noteSeq.add(64);
			noteSeq.add(0);
			noteSeq.add(65);
			noteSeq.add(0);
			noteSeq.add(67);
			noteSeq.add(0);
			noteSeq.add(65);
			noteSeq.add(0);
			noteSeq.add(64);
			noteSeq.add(0);
			noteSeq.add(62);
			
			
			// Fill sliders with note values
			for(i = 0; i < 8; i++)
			{
				notes[i]->setValue(noteSeq[(i*2)]);
			}
		}
		
		~MainComponent ()
		{
			deleteAllChildren();
		}
		
		//==============================================================================
		
		void resized ()
		{			
			for(i = 0; i < stepSequencer.size(); i++)
			{
				stepSequencer[i]->setBounds(10, (text->getY()+30) + 20*i, getWidth()-20, 20);
			}
		}
		
		void buttonClicked(Button* button)
		{
			if(button == play)
			{
				text->setText(T("Playing"), false);
				
				// startThread starts our thread which ultimately causes our run() method to be called
				// 10 = highest priority
				startThread(10); 
			}
			else if(button == stop)
			{
				text->setText(T("Stopped"), false);
				index = 0;		// Reset index so sequence starts from the beggining
				
				// stop (and timeout after 3 secs, if this fails and then force if necessary)
				stopThread(3000); 
			}
			else if(button == synthSelection)
			{
				if(synthSelection->getToggleState())
					synthSelection->setButtonText(T("Synthsizer On"));
				if(! synthSelection->getToggleState()){
					synthSelection->setButtonText(T("Synthsizer Off"));
					midiOutput->sendMessageNow(MidiMessage::allNotesOff(2));}
			}
			else if(button == stepSelection)
			{
				if(stepSelection->getToggleState())
					stepSelection->setButtonText(T("Step Sequencer On"));
				if(! stepSelection->getToggleState()){
					stepSelection->setButtonText(T("Step Sequencer Off"));
					midiOutput->sendMessageNow(MidiMessage::allNotesOff(1));}
			}
				
		}
		
		void sliderValueChanged (Slider* slider)
		{
			if(slider = rateSlider)
				rate = ((30/rateSlider->getValue())*1000);
			
			// Update note sequence with values from inc/dec sliders
			for(i = 0; i < 8; i++)
			{
				noteSeq.set((i*2), int(notes[i]->getValue()));
				
			}
			midiOutput->sendMessageNow(MidiMessage::allNotesOff(2));

			//****************************************************************<<DR>>
			// I'm not sure why this wouldn't work, I thought I could compare the
			// input to the callback with the note array index as they are all 
			// addresses but I must be doing something wrong
//			//****************************************************************<<DR>>
//			So had to loop through all the notes as above
//			if(slider == notes[0]){
//				DBG(String(int(notes[0]->getValue())));
//				noteSeq.set(0, int(notes[0]->getValue()));}
//			if(slider == notes[1])
//				noteSeq.set(2, notes[1]->getValue());
//			if(slider == notes[2])
//				noteSeq.set(4, notes[2]->getValue());
//			if(slider == notes[3])
//				noteSeq.set(6, notes[3]->getValue());
//			if(slider == notes[4])
//				noteSeq.set(8, notes[4]->getValue());
//			if(slider == notes[5])
//				noteSeq.set(10, notes[5]->getValue());
//			if(slider == notes[6])
//				noteSeq.set(12, notes[6]->getValue());
//			if(slider == notes[7])
//				noteSeq.set(14, notes[7]->getValue());
		}
		
		void labelTextChanged (Label* changedLabel)
		{
//			if(changedLabel == noteVals[0])
//				noteSeq.set(0, noteVals[0]->getText().getIntValue());
//			if(changedLabel == noteVals[1])
//				noteSeq.set(2, noteVals[1]->getText().getIntValue());
//			if(changedLabel == noteVals[2])
//				noteSeq.set(4, noteVals[2]->getText().getIntValue());
//			if(changedLabel == noteVals[3])
//				noteSeq.set(6, noteVals[3]->getText().getIntValue());
//			if(changedLabel == noteVals[4])
//				noteSeq.set(8, noteVals[4]->getText().getIntValue());
//			if(changedLabel == noteVals[5])
//				noteSeq.set(10, noteVals[5]->getText().getIntValue());
//			if(changedLabel == noteVals[6])
//				noteSeq.set(12, noteVals[6]->getText().getIntValue());
//			if(changedLabel == noteVals[7])
//				noteSeq.set(14, noteVals[7]->getText().getIntValue());

		}
		
		void comboBoxChanged(ComboBox* comboBox)
		{
			if(comboBox == midiOutputSelector)
			{
				// open the midi output selected via the menu
				midiOutput = MidiOutput::openDevice(midiOutputSelector->getSelectedItemIndex());
				
				// this would be called near the start of the program since we do..
				//  midiOutputSelector->setSelectedId(1, false);
				// ..shortly after populating our combo box menu in the constructor
			}
		}
		
		
		void run()
		{
			// this is where our Thread runs after startThread() is called when the play button is clicked
			
			while( ! threadShouldExit() )
			{
				// get the current time in ms, do this first so it is as accurate as possible
				const uint32 now = Time::getMillisecondCounter();
				
				// .. even bfeore checking our midi output is valid (i.e., not 0)
				if(midiOutput != 0)
				{
					// the '%' is the modulo operator, it gives the remainder after a division
					// so although index keeps getting larger.. doing index % noteSeq.size() always keeps
					// the index into the array betwen 0 and (noteSeq.size()-1)
					// e.g. if noteSeq.size() is 16 the return of index % noteSeq.size()
					// will always be between 0...15
					// Therefor (index - 1) % noteSeq.size() should get the previous note in the sequence
					
					// Set up current and previous notes
					int previousNoteIndex = (index - 1) % stepSequencer[0]->radioGroup.size();
					int currentNoteIndex = index % stepSequencer[0]->radioGroup.size();
					int previousSynthNoteIndex = (index - 2) % stepSequencer[0]->radioGroup.size();
					
					
					//********************************************<<DR>>
					// I think it is this that is causing a few problems
					// The last button doesn't get repainted after being 
					// turned white initially
					//********************************************<<DR>>
					
					// Move the transport hint
					for(i = 0; i < 4; i++)
					{
						stepSequencer[i]->radioGroup[currentNoteIndex]->setColour(TextButton::buttonColourId, Colours::white);
						if(currentNoteIndex > 0)
							stepSequencer[i]->radioGroup[previousNoteIndex]->setColour(TextButton::buttonColourId, Colours::lightblue);
						else if (currentNoteIndex = 0)
							stepSequencer[i]->radioGroup[15]->setColour(TextButton::buttonColourId, Colours::lightblue);
					}

					
					
					// If the synth selection button is on play the synth
					if(synthSelection->getToggleState() && (currentNoteIndex % 2 == 0))
					{
						// note off for the previous note..
						midiOutput->sendMessageNow(MidiMessage::noteOff(2, noteSeq[previousSynthNoteIndex]));
						// note on for new note..
						midiOutput->sendMessageNow(MidiMessage::noteOn(2, noteSeq[currentNoteIndex], float(synthVol->getValue())));
					}
					
					float stepVolume = float(stepVol->getValue());
					
					if(stepSelection->getToggleState())
					{
						// Step sequencer section
						if (stepSequencer[0]->radioGroup[currentNoteIndex]->getToggleState())		//Kick drum
						{
							midiOutput->sendMessageNow(MidiMessage::noteOff(1, 35));
							midiOutput->sendMessageNow(MidiMessage::noteOn(1, 35, stepVolume));
						}
						if (stepSequencer[1]->radioGroup[currentNoteIndex]->getToggleState())		//Snare Drum
						{
							midiOutput->sendMessageNow(MidiMessage::noteOff(1, 38));
							midiOutput->sendMessageNow(MidiMessage::noteOn(1, 38, stepVolume));
						}
						if (stepSequencer[2]->radioGroup[currentNoteIndex]->getToggleState())		//Closed hat
						{
							midiOutput->sendMessageNow(MidiMessage::noteOff(1, 42));
							midiOutput->sendMessageNow(MidiMessage::noteOn(1, 42, stepVolume));
						}
						if (stepSequencer[3]->radioGroup[currentNoteIndex]->getToggleState())		//Open Hat
						{
							midiOutput->sendMessageNow(MidiMessage::noteOff(1, 46));
							midiOutput->sendMessageNow(MidiMessage::noteOn(1, 46, stepVolume));
						}
					}					
				}
			
				index++;
				
				
				//***********************************************************************<<DR>>
				// I am accessing the rate slider here to get the BPM of the transport
				// I did try updating the 'rate' variable in the sliderValueChanged callback
				// but got some horrible results
				//***********************************************************************<<DR>>
				
				// pause until the next event is due i.e. the time it was at the top of the loop
				// plus 'rate' which is the time we want between events (defaulted to 125ms)
				Time::waitForMillisecondCounter(now + ((15/rateSlider->getValue())*1000));
			}
			
			// we break out of the while loop when the Thread is told to stop via stopThread()
			// here we send an all notes off message to prevent hangin notes (which haven't had a real note off)
			if(midiOutput != 0) 
			{
				midiOutput->sendMessageNow(MidiMessage::allNotesOff(1));
				midiOutput->sendMessageNow(MidiMessage::allNotesOff(2));
			}
		}
		
	};

#endif//_MAINCOMPONENT_H_ 