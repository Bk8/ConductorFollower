/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINEDITOR_H_D516F0C3__
#define __PLUGINEDITOR_H_D516F0C3__

#include <boost/fusion/include/for_each.hpp>

#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/FollowerOptions.h"

#include "MotionTracker/EventQueue.h"

#include "Visualizer/Data.h"
#include "Visualizer/Visualizer.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/JucePluginCharacteristics.h"
#include "PluginProcessor.h"

#include "Components/StatusWidget.h"
#include "Components/OptionWidget.h"

using namespace cf::ScoreFollower;

//==============================================================================
/**
*/
class CfpluginAudioProcessorEditor
	: public AudioProcessorEditor
	, public ChangeListener
{
public:
    CfpluginAudioProcessorEditor (CfpluginAudioProcessor* ownerFilter);
    ~CfpluginAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);

public:
	void changeListenerCallback(ChangeBroadcaster *source);

private:
	void BuildUI();

private:
	CfpluginAudioProcessor* ownerFilter;

	typedef Status::FollowerStatus::transformed<StatusWidget>::type FollowerStatusWidgets;
	FollowerStatusWidgets statusWidgets;

	typedef Options::FollowerOptions::transformed<StatusWidget>::type FollowerOptionWidgets;
	FollowerOptionWidgets optionWidgets;

	cf::Visualizer::Visualizer::Ptr visualizer_;

private: // Event stuff
	void ConsumeEvent(cf::MotionTracker::Event const & e);

	boost::shared_ptr<cf::MotionTracker::EventQueue> eventQueue_;
};


#endif  // __PLUGINEDITOR_H_D516F0C3__
