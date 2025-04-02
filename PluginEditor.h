#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioVisualizer : public juce::Component, public juce::Timer
{
public:
    AudioVisualizer (AudioPluginAudioProcessor&);
    void paint (juce::Graphics&) override;
    void timerCallback() override;
private:
    AudioPluginAudioProcessor& processorRef;
};

//==============================================================================

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor//, public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    // void timerCallback() override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)

    //custom
    AudioVisualizer visualizer;
};