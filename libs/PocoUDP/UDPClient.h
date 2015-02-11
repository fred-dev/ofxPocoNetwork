#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"

/*
 UDPClient
 - poco based udp: http://pocoproject.org/docs/Poco.Net.DatagramSocket.html
 - wip
 - sender only
 
 TODO: add receiving + threading maybe?
 */

namespace ofxPocoNetwork {

class UDPClient : public ofThread  {
public:
    
    UDPClient();
    virtual ~UDPClient();
    void disconnect();
    
    void connect(string ipAddr, int port,int sourcePort);
    bool connected;
    
    // send (blocking)
    int sendMessage(string msg);
    int sendMessage(ofBuffer& msg);
    
    // advanced- internal poco buffer sizes (best not to change this unless you need video or something)
    void setMaxSendSize(int size); // change pocos max send size- default 9216
    int getMaxSendSize();
    int getDatagramSourcePort();
    
    void threadedFunction();
    
    // receive
    bool hasWaitingMessages();
    int parseMessage();
    void setReceiveSize(int size);
    
    // send
    bool getNextMessage(ofBuffer& message);
    bool getNextMessage(string& msg);
    
    bool getNextMessage(uint8_t msg[]);
    
protected:
    int sleepTime;
    int receiveSize;
    
    bool waitingRequest; // temp var applied to any connection

    Poco::Net::SocketAddress* socketAddress;
    Poco::Net::SocketAddress* socketAddressSource;
    Poco::Net::DatagramSocket* socket;
    
    int maxBufferSendSize;
    queue<ofBuffer> receiveBuffers;

};

} // namespace ofxPocoNetwork