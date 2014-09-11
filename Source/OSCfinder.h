//
//  OSCfinder.h
//  MidiSyncTracker
//
//  Created by Hemmer / modified by Andrew Robertson on 30/07/2014.
//
//

//#include "OSCfinder.h"

#ifndef __OSCFINDER__
#define __OSCFINDER__

#include "../JuceLibraryCode/JuceHeader.h"


//important: do include path to oscpack in header search paths
// OSC send includes

#include "osc/OscOutboundPacketStream.h"
//error here? : copy oscpack to your juce project, same level as Builds

#include "ip/IpEndpointName.h"
// OSC receive includes
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
// OSC shared includes
#include "ip/UdpSocket.h"

//#include "AudioSample.h"

// forward declaration
//class MainContentComponent;//mlrVSTAudioProcessor;

//#define OSC_RECEIVE_PORT 7500//change for alternative port
#define OSC_SENDER_PORT 8080//NOT USING?




class OSCfinder :  public osc::OscPacketListener,
public Thread
{
    
public:
    
    // Constructor
    OSCfinder(const String &prefix, int portId);//, MainContentComponent* const owner);//from mlrVSTAudioProcessor * const owner);
    
    ~OSCfinder()
    {
        // stop the OSC Listener thread running
        s.AsynchronousBreak();
        
        // allow the thread 2 seconds to stop cleanly - should be plenty of time.
        stopThread(2000);
    }
    
    // Start the oscpack OSC Listener Thread
    // NOTE: s.Run() won't return unless we force it to with
    // s.AsynchronousBreak() as is done in the destructor
    void run()
    {
        s.Run();
    }
  
    void setPrefix(const String &prefix)
    {
        OSCPrefix = "/" + prefix;
        
        DBG("prefix now: " << OSCPrefix);
    }
    
private:
    //Hemmer needed the component info so included a pointer - see his code for how
  //  MainContentComponent* const parent;
    
    int incomingPort;
    UdpListeningReceiveSocket s;
    
    
    // outgoing messages ////////////////////
    char buffer[1536];                  // to store message data
    osc::OutboundPacketStream p;
    UdpTransmitSocket transmitSocket;
    
    //string that we search for in osc message, declared on instantiation of oscFinder instance
    
    
    //float getFloatOSCArg(const osc::ReceivedMessage& m);//defunkt as using iterators now
    //these two below also not used but example how you might use iterator to get the exact elemetn you require
    float getFloatOSCArg(const osc::ReceivedMessage& m, int index);
    int getIntOSCArg(const osc::ReceivedMessage& m, int index);
    
    JUCE_LEAK_DETECTOR(OSCfinder);
    
protected:
    String OSCPrefix;
    //this is our main processing function
    //overwrite to to do other things with incoming osc messages
    virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& /*remoteEndpoint*/);
};


#endif





