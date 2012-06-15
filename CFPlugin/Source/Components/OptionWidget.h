#pragma once

// JUCE headers last, as usual...
#include "../JuceLibraryCode/JuceHeader.h"

template<typename KeyType, typename ValueType>
class OptionWidget : public Component, public Slider::Listener
{
public:
	~OptionWidget()
	{
		deleteAllChildren();
	}

	void Initialize(ValueType & value)
	{
		value_ = &value;
		addAndMakeVisible(description_ = new Label("desc", String(KeyType::description().c_str())));

		ValueType::value_type val = value.value();
		addAndMakeVisible(slider_ = new Slider("slider"));
		slider_->setSliderStyle(Slider::LinearHorizontal);
		slider_->setRange(ValueType::min_value, ValueType::max_value, 1.0);
		slider_->setValue(val);
		slider_->addListener(this);

		layout_.setItemLayout(0, -0.2, -0.8, -0.5);
		layout_.setItemLayout(1, -0.2, -0.8, -0.5);
	}

public: // Slider::Listener implementation
	void sliderValueChanged (Slider *slider)
	{
		value_->setValue(slider->getValue());
	}

public: // GUI stuff
	void resized()
    {
        Component* comps[] = { description_, slider_ };
        layout_.layOutComponents (comps, 2,
                                    0, 0, getWidth(), getHeight(),
                                    true, true);
    }

private:
	StretchableLayoutManager layout_;
	Label * description_;
	Slider * slider_;
	ValueType * value_;
};
