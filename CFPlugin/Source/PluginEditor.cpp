/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Components/WidgetHelpers.h"

//==============================================================================
CfpluginAudioProcessorEditor::CfpluginAudioProcessorEditor (CfpluginAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
	, ownerFilter(ownerFilter)
	//, playButton(0)
	//, speedLabel(nullptr)
	//, runningLabel(nullptr)
{
    // This is where our plugin's editor size is set.
    setSize (400, 300);

	using namespace cf::ScoreFollower::Status;
	FollowerStatus & status = ownerFilter->followerStatus();
	WidgetInitializer<FollowerStatusWidgets> initializer(statusWidgets);
	boost::fusion::for_each(status.map(), initializer);

	std::vector<Component *> components;
	WidgetCollector<std::vector<Component *> > collector(components);
	boost::fusion::for_each(statusWidgets, collector);

	int yPos = 0;
	int const height = 30;
	for(auto it = components.begin(); it != components.end(); ++it) {
		addAndMakeVisible(*it);
		(*it)->setBounds(0, yPos, 150, height);
		yPos += height;
	}

	ownerFilter->changeBroadcaster.addChangeListener(this);
}

CfpluginAudioProcessorEditor::~CfpluginAudioProcessorEditor()
{
	ownerFilter->changeBroadcaster.removeChangeListener(this);
	// TODO make a sane system for this
	//deleteAllChildren();
}

//==============================================================================
void CfpluginAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll (Colours::white);

	/*
    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!",
                      0, 0, getWidth(), getHeight(),
                      Justification::centred, 1);
	*/

	
}

void
CfpluginAudioProcessorEditor::buttonClicked(Button * button)
{
	/*
	if (button == playButton) {
		ownerFilter->shouldRun.store(!ownerFilter->shouldRun.load());
	}
	*/
}

void
CfpluginAudioProcessorEditor::buttonStateChanged(Button * button)
{

}


void
CfpluginAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	using namespace cf::ScoreFollower::Status;
	FollowerStatus & status = ownerFilter->followerStatus();
	WidgetUpdater<FollowerStatusWidgets> updater(statusWidgets);
	boost::fusion::for_each(status.map(), updater);
}

