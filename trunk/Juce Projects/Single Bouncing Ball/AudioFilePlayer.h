/*
 *  AudioFilePlayer.h
 *  BouncingBallAudio
 *
 *  Created by David Rowland on 04/11/2008.
 *  Copyright 2008 UWE. All rights reserved.
 *
 */

#include <juce/juce.h>

#define SOUNDS_DIRECTORY "../../../sounds/"


/** 
 A wrapper class for playing audio files.
 
 Being an AudioTransportSource this controls the playback of a positionable audio stream, 
 handling the starting/stopping and sample-rate conversion.
 */
class AudioFilePlayer : public AudioTransportSource
	{
	public:
		
		/** Constuct an empty AudioFilePlayer.
		 */
		AudioFilePlayer()
		{
			currentAudioFileSource = 0;
		}
		
		/** Constuct a new AudioFilePlayer with a given path.
		 */
		AudioFilePlayer(const String& path)
		{
			currentAudioFileSource = 0;
			setFile(path);
		}
		
		~AudioFilePlayer()
		{
			setSource (0);
			deleteAndZero (currentAudioFileSource);
		}
		
		/** Play the audio file from the start.
		 */
		void startFromZero()
		{
			if(currentAudioFileSource == 0) return;
			
			setPosition (0.0);
			start();
		}
		
		/** Open and get ready to play a given audio file path.
		 */
		bool setFile(const String& path)
		{
			// should really delete/reset any exisiting data in case this method is callled more than once
			// (which it isn't in this example)
			stop();
			setSource (0);
			deleteAndZero (currentAudioFileSource);
			
			// OK now let's add the new file
			AudioFormatReader* reader = audioFormatReaderFromFile(path);
			
			if (reader != 0)
			{										
				// we SHOULD let the AudioFormatReaderSource delete the reader for us..
				currentAudioFileSource = new AudioFormatReaderSource (reader, true);
				
				// ..and plug it into our transport source
				setSource (currentAudioFileSource,
						   32768, // tells it to buffer this many samples ahead
						   reader->sampleRate);
				
				return true;
			}
			
			return false;
		}
		
	private:	
		AudioFormatReader* audioFormatReaderFromFile(const String& path)
		{
			File audioFile (path);
			
			// get a format manager and set it up with the basic types (wav and aiff).
			AudioFormatManager formatManager;
			formatManager.registerBasicFormats();
			
			return formatManager.createReaderFor (audioFile);
		}
		
		// this is the actual stream that's going to read from the audio file.
		AudioFormatReaderSource* currentAudioFileSource;
		
	};