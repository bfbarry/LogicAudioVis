#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), visualizer(p)
{
    // juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(visualizer);
    setSize (600, 400);
    // startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    visualizer.setBounds(getLocalBounds().reduced(10));
}

// void AudioPluginAudioProcessorEditor::timerCallback()
// {
//     repaint();
// }


AudioVisualizer::AudioVisualizer (AudioPluginAudioProcessor& p)
    : processorRef(p)
{
    startTimerHz(30);
}

void AudioVisualizer::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::green);

    // TODO use transformed copies instead
    juce::Array<float> localCopy;
    {
        const juce::ScopedLock sl(processorRef.bufferLock);
        localCopy = processorRef.currentBufferValues;
    }
    auto bounds = getLocalBounds().toFloat();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();

    juce::Path waveform;

    const int numPoints = processorRef.currentBufferValues.size();
    const float pathStep = width/numPoints;

    waveform.startNewSubPath(0, height*0.5f);

    for (int i = 0; i < numPoints; ++i) {
        float y = juce::jmap(localCopy[i], -1.0f, 1.0f, height, 0.0f);
        waveform.lineTo(i * pathStep, y);
    }

    g.strokePath(waveform, juce::PathStrokeType(2.0f));
}

void AudioVisualizer::timerCallback()
{
    if (processorRef.newDataAvailable.compareAndSetBool(false, true))
        repaint();
}