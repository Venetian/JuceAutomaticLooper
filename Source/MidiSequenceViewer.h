//
//  MidiSequenceViewer.h
//  JuceAbletonMidiFilePlayer
//
//  Created by Andrew Robertson on 11/08/2014.
//
//

#ifndef __JuceAbletonMidiFilePlayer__MidiSequenceViewer__
#define __JuceAbletonMidiFilePlayer__MidiSequenceViewer__

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

class MidiSequenceViewer{
public:
    MidiSequenceViewer();
    ~MidiSequenceViewer();
    
    RelativeRectangle relativeViewerBox;
    Rectangle<float> viewerBox;
    
    MidiMessageSequence* sequencePtr;
    
    void setBounds(float x, float y, float w, float h);
    void draw(Graphics& g);
    void resized();
    
    void redrawMidiSequence(Graphics& g);
    void setSequence(MidiMessageSequence& sequence);
    
    Graphics* viewerGraphics;
    float getHeight(float pitch);
    
    float* loopMax;
    float* loopMin;
    
    int pitchMin;
    int pitchMax;
    
    //these are limits, so we will always show this much of the midi notes
    int pitchAbsoluteMin, pitchAbsoluteMax;
    
    void setPitchLimits();
    int getMinPitch();
    int getMaxPitch();
    
    //change this to uodate the viewer
    Value changedValue;
    
    Colour noteColor;
    
};
#endif /* defined(__JuceAbletonMidiFilePlayer__MidiSequenceViewer__) */
