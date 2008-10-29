/*
  ==============================================================================

   Demonstration BouncingBallAudio application in JUCE
   Copyright 2004 by Julian Storer.
   Modified 2008 by Martin Robinson

  ==============================================================================
*/

#include <juce/juce.h>

// some defines
#define APPLICATION_NAME "Bouncing Ball"
#define SOUNDS_DIRECTORY "../../../sounds/"

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


//==============================================================================
class AudioDemo  :  public Component,
					public ButtonListener,
					public ChangeListener,
					public BouncingBallListener,
					public AudioIODeviceCallback
{
    //==============================================================================
    TextButton* audioSettingsButton;
	
    //==============================================================================
    // this wraps the actual audio device
    AudioDeviceManager audioDeviceManager;
	
    // this allows an audio source to be streamed to the IO device
    AudioSourcePlayer audioSourcePlayer;
	
	// this source is used to mix together the output from our synth source
    // and wave player source
    MixerAudioSource mixerSource;
	
	// the above objects are needed to manage the playback of audio
	// ...
	// we need one of each of the following objects to play a sound file
	
	//AudioFilePlayer file1, file2;
	OwnedArray<File> audioFiles;
	OwnedArray<AudioFilePlayer> filePlayers;
	Array<BouncingBallComponent*> balls;
		
public:
	//==============================================================================
	AudioDemo()
    {
		setName (T(APPLICATION_NAME));
	
		//==============================================================================
		
		addAndMakeVisible (audioSettingsButton = new TextButton (T("show audio settings..."),
																 T("click here to change the audio device settings")));
		audioSettingsButton->addButtonListener (this);
		
		//==============================================================================
		// and initialise the device manager with no settings so that it picks a
		// default device to use.
		const String error (audioDeviceManager.initialise (1, /* number of input channels */
														   2, /* number of output channels */
														   0, /* no XML settings.. */
														   true  /* select default device on failure */));
		
		if (error.isNotEmpty())
		{
			AlertWindow::showMessageBox (AlertWindow::WarningIcon,
										 T("JuceAudioTemplateApp"),
										 T("Couldn't open an output device!\n\n") + error);
		}
		else
		{
			// ..and connect the mixer to our source player.
			audioSourcePlayer.setSource (&mixerSource);
			
			// start the IO device pulling its data from our callback..
			audioDeviceManager.setAudioCallback (this);
			
			// find some directories
			File appDirectory = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory();
			File soundsDirectory = appDirectory.getChildFile(T(SOUNDS_DIRECTORY));
			
			// print them for debugging (in debug mode only) note the extra brackets for 
			// DBG_PRINTF i.e. DBG_PRINTF(()) rather than just DBG_PRINTF() 
			// ..this is required.
			DBG_PRINTF(( String("App Directory: ") << appDirectory.getFullPathName() ));
			DBG_PRINTF(( String("Sounds Directory: ") << soundsDirectory.getFullPathName() ));
			
			// find all the files in our sounds directory
			soundsDirectory.findChildFiles(audioFiles, File::findFiles, false, T("*.aif"));
			soundsDirectory.findChildFiles(audioFiles, File::findFiles, false, T("*.wav"));
			
			// iterate through our files, making a ball and a player for each
			for (int i = 0; i < audioFiles.size(); i++ )
			{
				String soundFilePath = audioFiles[i]->getFullPathName();
				DBG_PRINTF(( soundFilePath ));
				
				// a new instance of one of our player objects
				// initialised with the path of this sound file
				filePlayers.add (new AudioFilePlayer(soundFilePath));
				
				// listen for start/stop messages of the transport of each file
				filePlayers[i]->addChangeListener (this);
				
				// plug it in to our mixer
				mixerSource.addInputSource (filePlayers[i], false);
				
				BouncingBallComponent *ball;
				addAndMakeVisible (ball = new BouncingBallComponent());
				
				// listen for ball bouncing messages
				ball->addListener(this);
				
				// keep an array of the balls, in case we decide to add/remove them dynamically
				// not REQUIRED since the Component will delete them cleanly anyway when the app exits
				balls.add(ball);
			}
		}
	}
	
	~AudioDemo()
	{
		audioDeviceManager.setAudioCallback (0);
		
		// reset all transportSources before they get deleted	
		for (int i = 0; i < audioFiles.size(); i++ )
		{
			filePlayers[i]->removeChangeListener (this);
		}
		
		// (NB  "filePlayers" is an OwnedArray so it and its elements are deleted for us )
		
		audioSourcePlayer.setSource (0);
		
		deleteAllChildren();
	}
	
	
	//==============================================================================
	void audioDeviceIOCallback (const float** inputChannelData,
								int totalNumInputChannels,
								float** outputChannelData,
								int totalNumOutputChannels,
								int numSamples)
	{		
		// pass the audio callback on to our player source
		audioSourcePlayer.audioDeviceIOCallback (inputChannelData, totalNumInputChannels, outputChannelData, totalNumOutputChannels, numSamples);
	}
	
	void audioDeviceAboutToStart (AudioIODevice* device)
	{
		audioSourcePlayer.audioDeviceAboutToStart (device);
	}
	
	void audioDeviceStopped()
	{
		audioSourcePlayer.audioDeviceStopped();
	}
	
	//==============================================================================
	void paint (Graphics& g)
	{
		// nothing here for now, it's all done by the child components
	}
	
	void resized()
	{		
		audioSettingsButton->setBounds (10, 10, 200, 24);
		audioSettingsButton->changeWidthToFitText();
	}
	
	
	void buttonClicked (Button* button)
	{
		if (button == audioSettingsButton)
		{
			// Create an AudioDeviceSelectorComponent which contains the audio choice widgets...
			AudioDeviceSelectorComponent audioSettingsComp (audioDeviceManager,
															0, 1,
															2, 2,
															true,
															false,
															true,
															false);
			
			// ...and show it in a DialogWindow...
			audioSettingsComp.setSize (500, 400);
			
			DialogWindow::showModalDialog (T("Audio Settings"),
										   &audioSettingsComp,
										   this,
										   Colours::azure,
										   true);
		}
	}
		
	void changeListenerCallback (void* pointer)
	{
		// callback from the transport source to tell us that play has
		// started or stopped
				
	}
	
	void ballCollision (BouncingBallComponent* ball)
	{
		DBG_PRINTF(( T("ball %p bounced"), ball ));
		
		int i = balls.indexOf(ball);
//		filePlayers[i]->startFromZero();
	}
	
	
};


//==============================================================================
/** This is the top-level window that we'll pop up. Inside it, we'll create and
    show a AudioDemo component.
*/
class HelloWorldWindow  : public DocumentWindow
{
public:
    //==============================================================================
    HelloWorldWindow()
        : DocumentWindow (T(APPLICATION_NAME),
                          Colours::azure, 
                          DocumentWindow::allButtons, 
                          true)
    {
		setContentComponent (new AudioDemo());

        setVisible (true);
		setResizable(true, false);
		setTitleBarHeight(20);
		

        // centre the window on the desktop with this size
        centreWithSize (300, 300);
    }

    ~HelloWorldWindow()
    {
        // (the content component will be deleted automatically, so no need to do it here)
    }

    //==============================================================================
    void closeButtonPressed()
    {
        // When the user presses the close button, we'll tell the app to quit. This 
        // window will be deleted by the app object as it closes down.
        JUCEApplication::quit();
    }
};


//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
    the initialisation and shutdown of the whole application.
*/
class JUCEHelloWorldApplication : public JUCEApplication
{
    /* Important! NEVER embed objects directly inside your JUCEApplication class! Use
       ONLY pointers to objects, which you should create during the initialise() method
       (NOT in the constructor!) and delete in the shutdown() method (NOT in the
       destructor!)

       This is because the application object gets created before Juce has been properly
       initialised, so any embedded objects would also get constructed too soon.
   */
    HelloWorldWindow* helloWorldWindow;

public:
    //==============================================================================
    JUCEHelloWorldApplication()
        : helloWorldWindow (0)
    {
        // NEVER do anything in here that could involve any Juce function being called
        // - leave all your startup tasks until the initialise() method.
    }

    ~JUCEHelloWorldApplication()
    {
        // Your shutdown() method should already have done all the things necessary to
        // clean up this app object, so you should never need to put anything in
        // the destructor.

        // Making any Juce calls in here could be very dangerous...
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        // just create the main window...
        helloWorldWindow = new HelloWorldWindow();

        /*  ..and now return, which will fall into to the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().

            In this case, JUCEAppliction::quit() will be called by the
            hello world window being clicked.
        */
    }

    void shutdown()
    {
        // clear up..

        if (helloWorldWindow != 0)
            delete helloWorldWindow;
    }

    //==============================================================================
    const String getApplicationName()
    {
        return T(APPLICATION_NAME);
    }

    const String getApplicationVersion()
    {
        return T("1.0");
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
    }
};


//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION (JUCEHelloWorldApplication)
