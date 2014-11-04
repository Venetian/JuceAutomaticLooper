//
//  JucePatternSequencer.h
//  JuceAbletonMidiFilePlayer
//
//  Created by Andrew Robertson on 12/08/2014.
//
//

#ifndef __JuceAbletonMidiFilePlayer__JucePatternSequencer__
#define __JuceAbletonMidiFilePlayer__JucePatternSequencer__

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiSequenceViewer.h"

#include <deque>

#define EIGHTH_NOTES 2
#define SIXTEENTH_NOTES 4

class MidiMessageArraySorter
{
public:
    //function required by std::sort to be able to automatically sort an array
    static int compareElements(MidiMessage first, MidiMessage second){
        if (first.getNoteNumber() < second.getNoteNumber())
            return -1;
        else if (first.getNoteNumber() == second.getNoteNumber())
            return 0;
        else
            return 1;
    }
};

class JucePatternSequencer{
//friend class JuceSequenceLoopPlayer;
    
public:
    JucePatternSequencer();
    ~JucePatternSequencer();
    
    MidiMessageSequence rhythmPattern;
    //need to record the midi notes by storing the index in the pitchSet
    //as a usual midi pattern
    
    
    Array<MidiMessage> pitchSet;//set of notes to play our pattern with
    Array<MidiMessage> altPitchSet;//transformed set
    int getIndex(int& pitch);
    
    
    void loadSequence(MidiMessageSequence& sequence);
    
    MidiMessageSequence outputSequence;
    
    void printPitchSet();
    void printMidiMessageArray(Array<MidiMessage>& pitchArray);
    
    void newMidiMessage(MidiMessage& message);
    
    bool playedMidiMessage(const MidiMessage& message);
    
    int* milliscounter;
    
    float* loopMin;
    float* loopMax;
    void setLoopPoints(float& loopPointMin, float& loopPointMax);
    
    bool noteIsInPitchSet(int& pitch);
    
    void generateAlternativePitches(Array<MidiMessage>& newNotes);
    void generateOutputSequence(MidiMessageSequence& sequence);
    void generateOutputSequence(MidiMessageSequence& sequence, Array<MidiMessage>& altPitchSet);
    void reorderPitchSetAndRhythm();
    
    
    
    Array<MidiMessage> patternNotesPlayed;
    MidiMessageArraySorter sorter;//for sorting these arrays
    int patternNotesPlayedOn;
    bool reload;
    bool wantToReplaceOriginal;
    MidiSequenceViewer stepSequenceViewer;

//    MidiMessageArraySorter sorter;
    
};

#endif /* defined(__JuceAbletonMidiFilePlayer__JucePatternSequencer__) */
