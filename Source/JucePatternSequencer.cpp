//
//  JucePatternSequencer.cpp
//  JuceAbletonMidiFilePlayer
//
//  Created by Andrew Robertson on 12/08/2014.
//
//

#include "JucePatternSequencer.h"

JucePatternSequencer::JucePatternSequencer(){

    loopMax = nullptr;
    loopMin = nullptr;
    
    MidiMessage m(144, 48, 127);
    m.setTimeStamp(0.0);
    newMidiMessage(m);
   
    m = MidiMessage(128, 48, 0);
    m.setTimeStamp(0.87);
    newMidiMessage(m);
    
    m = MidiMessage(144, 49, 127);
    m.setTimeStamp(1.0);
    newMidiMessage(m);
    
    m = MidiMessage(128, 49, 127);
    m.setTimeStamp(1.96);
    newMidiMessage(m);
    
    m = MidiMessage(144, 48, 127);
    m.setTimeStamp(2.0);
    newMidiMessage(m);
    
    m = MidiMessage(128, 48, 0);
    m.setTimeStamp(2.97);
    newMidiMessage(m);
    
    patternNotesPlayedOn = 0;
    
}


JucePatternSequencer::~JucePatternSequencer(){
    
}
//newBeatReceievd(){}



void JucePatternSequencer::loadSequence(MidiMessageSequence& sequence){
    
    //when loading we can do non-causal sequence by having the note off as well as the note on
    //requires the sequence is updated first, so calling here:
    sequence.updateMatchedPairs();
    
    pitchSet.clear();
    rhythmPattern.clear();
    if (loopMax != nullptr && loopMin != nullptr){
        for (int i = 0; i < sequence.getNumEvents(); i++){
            if (sequence.getEventTime(i) >= *loopMin && sequence.getEventTime(i) <= *loopMax){
                if (sequence.getEventPointer(i)->message.isNoteOn()){
                    newMidiMessage(sequence.getEventPointer(i)->message);
                    newMidiMessage(sequence.getEventPointer(sequence.getIndexOfMatchingKeyUp(i))->message);
                    //the matching note off
                }
            }
        }
    }
    
    rhythmPattern.updateMatchedPairs();
    pitchSet.sort(sorter);
    printPitchSet();
    
}

void JucePatternSequencer::setLoopPoints(float& loopPointMin, float& loopPointMax){
    loopMax = &loopPointMax;
    loopMin = &loopPointMin;
    std::cout << "Pattern sequencer between " << *loopMin << " and " << *loopMax << std::endl;
}


void JucePatternSequencer::newMidiMessage(MidiMessage& message){
    //get the rhythm - i.e. position in the bar
    //and the note - check if in set, if not we add to the set

    
    if (message.isNoteOn()){
        int pitch = message.getNoteNumber();
        if (noteIsInPitchSet(pitch)){
            int index = getIndex(pitch);
            //std::cout << "note on " << pitch << " present index " << index << std::endl;
            MidiMessage m(144, index, message.getVelocity());
            m.setTimeStamp(message.getTimeStamp());
            rhythmPattern.addEvent(m);
            std::cout << "new message ON " << pitch << ", index " << m.getNoteNumber() << ", time " << m.getTimeStamp() << std::endl;
        } else {
            //not in pitch set
            //std::cout << "pitch " << pitch << " not present in set " << std::endl;
            pitchSet.add(message);
            int index = getIndex(pitch);
            MidiMessage m(144, index, message.getVelocity());
            m.setTimeStamp(message.getTimeStamp());
            rhythmPattern.addEvent(m);
            std::cout << "new message ON " << pitch << ", index " << m.getNoteNumber() << ", time " << m.getTimeStamp() << std::endl;
        }
        
        outputSequence.addEvent(message);
        
    } else if (message.isNoteOff()){
        int pitch = message.getNoteNumber();
        if (noteIsInPitchSet(pitch)){
            int index = getIndex(pitch);
            //std::cout << "note off " << pitch << " present index " << index << std::endl;
            MidiMessage m(128, index, message.getVelocity());
            m.setTimeStamp(message.getTimeStamp());
            rhythmPattern.addEvent(m);
            std::cout << "new off message " << pitch << ", index " << m.getNoteNumber() << " " << m.getTimeStamp() << std::endl;
        }
        
        outputSequence.addEvent(message);
    } else {
        //incomplete
        rhythmPattern.addEvent(message);
        outputSequence.addEvent(message);
    }

}

bool JucePatternSequencer::playedMidiMessage(const MidiMessage& message){
    reload = false;
    if (message.isNoteOnOrOff()){
        if (message.isNoteOn()){
            if (patternNotesPlayedOn == 0)
                patternNotesPlayed.clear();//get rid of them as first one
            //new pitch message
            const uint8* data = message.getRawData();
            MidiMessage m(data[0], data[1], data[2]);
            m.setTimeStamp(0);//no roblem with this
            patternNotesPlayed.add(m);
            //patternNotesPlayed.sort(sorter);
            std::cout << "PATTERN NOTES, played" << std::endl;
            printMidiMessageArray(patternNotesPlayed);
            patternNotesPlayedOn++;
            reload = true;
            
            generateAlternativePitches(patternNotesPlayed);
            //now set the notes
            
        } else if (message.isNoteOff()){
            patternNotesPlayedOn--;
            std::cout << "PATTERN NOTES" << std::endl;
            printMidiMessageArray(patternNotesPlayed);
            if (patternNotesPlayedOn == 0)
                std::cout << "FINISHED " << std::endl;
        }
    }
    return reload;
}

bool JucePatternSequencer::noteIsInPitchSet(int& pitch){
    bool notePresent = false;
    for (int i = 0; i < pitchSet.size(); i++){
        if (pitchSet[i].getNoteNumber() == pitch)
            notePresent = true;
    }
    return notePresent;
}

int JucePatternSequencer::getIndex(int& pitch){
    for (int i = 0; i < pitchSet.size(); i++){
        if (pitchSet[i].getNoteNumber() == pitch)
            return i;
    }
    return -1;
}

void JucePatternSequencer::printPitchSet(){
    std::cout << "Pattern Sequencer: PITCH SET" << std::endl;
    printMidiMessageArray(pitchSet);
//    for (int i = 0; i < pitchSet.size(); i++){
//        std::cout << "pitch [" << i << "]: " << pitchSet[i].getNoteNumber() << std::endl;
//    }

}


void JucePatternSequencer::printMidiMessageArray(Array<MidiMessage>& pitchArray){
    for (int i = 0; i < pitchArray.size(); i++){
        std::cout << "pitch [" << i << "]: " << pitchArray[i].getNoteNumber() << std::endl;
    }
    
}


void JucePatternSequencer::generateAlternativePitches(Array<MidiMessage>& newNotes){
    altPitchSet.clear();
    altPitchSet = pitchSet;//the original notes
    altPitchSet.sort(sorter);
    newNotes.sort(sorter);
    std::cout << " EXISTING PITCHES " << std::endl;
    printMidiMessageArray(altPitchSet);
    for (int i = 0; i < newNotes.size(); i++){
        if (altPitchSet.size() > i){
            std::cout << "REPLACE " << altPitchSet[i].getNoteNumber() << " with " << newNotes[i].getNoteNumber() << std::endl;
            //MidiMessage m = newNotes[i];
            //altPitchSet[i] = m;
            altPitchSet.set(i, newNotes[i]);//NoteNumber(newNotes[i].getNoteNumber());
            //same velocity
//            altPitchSet[i].setVelocity(newNotes[i].getVelocity());
        }
    }
    
    for (int i = 0; i < altPitchSet.size(); i++){
        std::cout << "new pitch set [" << i << "] : " << altPitchSet[i].getNoteNumber() << std::endl;
    }
}


void JucePatternSequencer::generateOutputSequence(MidiMessageSequence& sequence, Array<MidiMessage>& newPitchSet){
    sequence.clear();
    for (int i = 0; i < rhythmPattern.getNumEvents(); i++){
        MidiMessage m = rhythmPattern.getEventPointer(i)->message;
        if (m.isNoteOnOrOff()){
            int indexInPitchSet = rhythmPattern.getEventPointer(i)->message.getNoteNumber();
            m.setNoteNumber(newPitchSet[indexInPitchSet].getNoteNumber());
            //quantise
            if (m.isNoteOn()){
                float time = m.getTimeStamp();
                m.setTimeStamp((double)round(time*SIXTEENTH_NOTES)/SIXTEENTH_NOTES);
                printf("[%i] ch.%i quantiser time %f, quant %f\n", m.getNoteNumber(), m.getChannel(), time, (double)round(time*SIXTEENTH_NOTES)/SIXTEENTH_NOTES );
            }
        } else {
            printf("adding non not-on-off %i\n", m.getControllerValue());
        }
        sequence.addEvent(m);
    }
    sequence.updateMatchedPairs();
    
}



void JucePatternSequencer::generateOutputSequence(MidiMessageSequence& sequence){
    sequence.clear();
    for (int i = 0; i < rhythmPattern.getNumEvents(); i++){
        MidiMessage m = rhythmPattern.getEventPointer(i)->message;
        if (m.isNoteOnOrOff()){
            int indexInPitchSet = rhythmPattern.getEventPointer(i)->message.getNoteNumber();
            m.setNoteNumber(pitchSet[indexInPitchSet].getNoteNumber());
            //quantise
            if (m.isNoteOn()){
                float time = m.getTimeStamp();
                m.setTimeStamp((double)round(time*SIXTEENTH_NOTES)/SIXTEENTH_NOTES);
                printf("[%i] quantiser time %f, quant %f\n", m.getNoteNumber(), time, (double)round(time*SIXTEENTH_NOTES)/SIXTEENTH_NOTES );
            }
        } else {
            printf("adding non not-on-off %i\n", m.getControllerValue());
        }
        sequence.addEvent(m);
    }
    sequence.updateMatchedPairs();

}



void JucePatternSequencer::reorderPitchSetAndRhythm(){
    Array<MidiMessage> pitchSetCopy = pitchSet;

    std::cout << "reorder" << std::endl;
    printPitchSet();
    //MidiMessageArraySorter sorter; put in main class
    pitchSet.sort(sorter);
    
    std::cout << "after sorting " << std::endl;
    printPitchSet();
    
    Array<int> pitchMap;
    for (int k = 0; k < pitchSetCopy.size(); k++){
        int note = pitchSetCopy[k].getNoteNumber();
        pitchSetCopy[k].setNoteNumber(getIndex(note));
        pitchMap.add(getIndex(note));
        std::cout << "pitch " << k << " is " << getIndex(note) << std::endl;
    }
    
    
    
    for (int i = 0; i < rhythmPattern.getNumEvents(); i++){
        int pitch = rhythmPattern.getEventPointer(i)->message.getNoteNumber();
   //     int tmp = (int)pitchSetCopy[pitch].getNoteNumber();
   //     int matchingIndex = getIndex(tmp);
   //     std::cout << "picth " << pitch << " matches " << matchingIndex << std::endl;
        rhythmPattern.getEventPointer(i)->message.setNoteNumber(pitchMap[pitch]);
//
    }
    rhythmPattern.updateMatchedPairs();
    
    //now need to relabel the rhythm
    
}

//add hanging note

//set pattern (on loop end)

