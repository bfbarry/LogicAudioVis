#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <Eigen/Dense>
#include <Eigen/SVD>

static Eigen::MatrixXf _pcaFitTransform(Eigen::MatrixXf& data) {
    // Center the data
    Eigen::VectorXf mean = data.colwise().mean();
    Eigen::MatrixXf centered = data.rowwise() - mean.transpose();

    // Compute full SVD
    Eigen::BDCSVD<Eigen::MatrixXf> svd(centered, Eigen::ComputeThinU | Eigen::ComputeThinV);

    // Determine how many components to keep
    int k = std::min(data.rows(), data.cols());

    // Get the top-k principal components
    Eigen::MatrixXf components = svd.matrixV().leftCols(k);

    // Optional: flip signs for consistency with sklearn
    for (int i = 0; i < components.cols(); ++i) {
        if (components(0, i) < 0)
            components.col(i) *= -1;
    }

    // Project data
    Eigen::MatrixXf transformed = centered * components;

    return transformed;
}

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

    Eigen::MatrixXf pcaProj;
    {
        const juce::ScopedLock sl(processorRef.bufferLock);
        pcaProj = _pcaFitTransform(processorRef.windowMat);

    }
    auto bounds = getLocalBounds().toFloat();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();

    juce::Path waveform;

    Eigen::RowVectorXf X = pcaProj.col(0);
    Eigen::RowVectorXf Y = pcaProj.col(1);
    const int numPoints = X.size();
    const float pathStep = width/numPoints;

    waveform.startNewSubPath(0, height*0.5f);

    for (int i = 0; i < numPoints; ++i) {
        float x = juce::jmap(X[i], -1.0f, 1.0f, width, 0.0f);
        float y = juce::jmap(Y[i], -1.0f, 1.0f, height, 0.0f);
        waveform.lineTo(x, y);
    }

    g.strokePath(waveform, juce::PathStrokeType(2.0f));
}

void AudioVisualizer::timerCallback()
{
    if (processorRef.newDataAvailable.compareAndSetBool(false, true))
        repaint();
}