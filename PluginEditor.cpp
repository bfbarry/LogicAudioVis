#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    juce::String bufferText = "values: ";
    {
        const juce::ScopedLock sl(processorRef.bufferLock);
        for (int i = 0; i < processorRef.currentBufferValues.size(); ++i) {
            bufferText += juce::String(processorRef.currentBufferValues[i], 4);
            if (i < processorRef.currentBufferValues.size() - 1)
                bufferText += ", ";
        }
    }
    // g.drawFittedText(bufferText, getLocalBounds(), juce::Justification::centred, 3);
    g.drawFittedText ("hello", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}