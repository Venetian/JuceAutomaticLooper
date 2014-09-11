

/*
 ==============================================================================
 
 OSCfinder.cpp
 Created: 13 Sep 2011 7:03:15pm
 Author:  Hemmer / Andrew Robertson modifier
 
 ==============================================================================
 */

//#include "PluginProcessor.h"
//#include "OSCfinder.h"
#include "OSCfinder.h"


//, mlrVSTAudioProcessor * const owner)




/*if you get an error:
libc++abi.dylib: terminating with uncaught exception of type std::runtime_error: unable to bind udp socket
it may be something else is bound to this port alread
 //when compiling this, if you have a std:runtime exception, check there's not something running binding to osc in background
 //http://new-supercollider-mailing-lists-forums-use-these.2681727.n2.nabble.com/unable-to-bind-udp-socket-resolve-on-OS-X-td7604288.html
 //http://forum.openframeworks.cc/t/ofxosc-unable-to-connect-udp-socket-crashes-of-app-when-in-login-items-mac-osx-mavericks/14943/5

*/

// Constructor
OSCfinder:: OSCfinder(const String &prefix, int portId):
            Thread("OscListener Thread"),
            incomingPort(portId), //incomingPort can this be changed later ? ...
            s(IpEndpointName("localhost", incomingPort), this),
            buffer(), p(buffer, 1536),
            transmitSocket(IpEndpointName("localhost", OSC_SENDER_PORT)),
            OSCPrefix(prefix)
{
    std::cout << "OSC finder initialised to receive on port " << incomingPort << std::endl;
    setPrefix(prefix);

}


void OSCfinder::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& /*remoteEndpoint*/)
{
    //override this process to do your own osc parsing by extending the class
    //see eg OscAbletonFinder.h in JuceMidiLoopPlayer on Github
    try
    {
        String msgPattern = m.AddressPattern();
        const int numArgs = m.ArgumentCount();
    
        if (msgPattern.equalsIgnoreCase(OSCPrefix))
            std::cout << "osc message matches address: ";
        else
            std::cout << "received something else: ";
        std::cout << msgPattern;
        
        std::cout << " and " << numArgs << " argument(s)" << std::endl;

        osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
        
        for (int i = 0; i < numArgs; i++){
            //this isnt great - it assumes one argument which is a float
            
            //since we check, using the arg->AsFloatUnchecked() would probably be faster
            //but leaving the checked methods as safer in long run
            //string is an example of that below
            if (arg->IsFloat())
                std::cout << "float[" << i << "] : " << arg->AsFloat() << std::endl;
            else if (arg->IsInt32())
                std::cout << "int32[" << i << "] : " << arg->AsInt32() << std::endl;
            else if (arg->IsString())
                std::cout << "string[" << i << "] : " << arg->AsStringUnchecked() << std::endl;
            else
                std::cout << "arg type unknown (easy to add your desired type to the code)" << std::endl;
            
            //advance iterator to the index we want
            //might not be very efficient for large osc messages
            //but we progress sequentially here through message
            arg++;
        }
    }
    catch (osc::Exception& e)
    {
        DBG("error while parsing message: " << m.AddressPattern() << ": " << e.what() << "\n");
    }
}



//no longer need these guys
float OSCfinder::getFloatOSCArg(const osc::ReceivedMessage& m, int index)
{
    try
    {
        // we assume only one argument
        const int numArgs = m.ArgumentCount();
        if (numArgs < index) throw osc::MissingArgumentException();
        
        osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();

        //advance iterator to the index we want
        //probably not very efficient for large osc messages!
        while (index > 0){
            arg ++;
            --index;
        }
        
        const float floatArg = arg->AsFloat();
        
        return floatArg;
        
    }
    catch (osc::WrongArgumentTypeException& )
    {
        // if the argument is an int pretending to be a float, then we
        // need to consider that too so we cast to float
        try
        {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            const float floatArg = (float) arg->AsInt32();
            
            return floatArg;
        }
        catch (osc::Exception &e)
        {
            throw e;
        }
    }
    catch (osc::Exception &e)
    {
        // pass exception on
        throw e;
    }
    
}

//no longer required
int OSCfinder::getIntOSCArg(const osc::ReceivedMessage& m, int index)
{
    try
    {
        // we assume only one argument
        const int numArgs = m.ArgumentCount();
        if (numArgs < index) throw osc::MissingArgumentException();
        
        osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
        //advance iterator to the index we want
        //probably not very efficient for large osc messages!
        while (index > 0){
            arg ++;
            --index;
        }
        
        const int intArg = arg->AsInt32();
        
        return intArg;
        
    }
    catch (osc::Exception &e)
    {
        // pass exception on
        throw e;
    }
}


