#pragma once

#include "ofMain.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/NObserver.h"


using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::IdleNotification;
using Poco::Net::TimeoutNotification;
using Poco::Net::ErrorNotification;
using Poco::Net::ShutdownNotification;
using Poco::NObserver;
using Poco::Thread;
using Poco::Net::Socket;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;
using Poco::AutoPtr;
using Poco::ScopedLock;

namespace ofxPocoNetwork {
    
// defaults
#define HEADER_BYTES 4
#define DEFAULT_DELIMITER ';'
#define DEFAULT_FIXED_SIZE 256

// message protocol
enum MessageFraming {
    FRAME_NONE,
    FRAME_HEADER_AND_MESSAGE,
    FRAME_DELIMITED,
    FRAME_FIXED_SIZE
};

/*
 SocketConnectionHandler
 - assumes run by a thread or with a run() via a superclass (TCPSocketConnectionHandler, ReactorConnectionHandler)
 - has a mutex for the queues
 - socketPtr and framingProtocol must be initialised manually after superclass constructor with setup()
 - call processRead() and processWrite() 
 - internal message parser (MessageFraming)
 
 - 0. TYPE_NONE just gets whatever is available from socket
 - 1. TYPE_HEADER_AND_MESSAGE automatically sends/receives a 4 byte header before each message (default)
 - 2. TYPE_DELIMITED splits messages by delimeter
 - 3. TYPE_FIXED_SIZE messages will always be same size, eg. 256
 */

class SocketConnectionHandler {
public:
    
    SocketConnectionHandler();
    ~SocketConnectionHandler();
    
    void setup(StreamSocket* socket, MessageFraming protocol=FRAME_HEADER_AND_MESSAGE);
    
    virtual void disconnect();
    ofEvent<void> disconnectionEvent;
    
    // send/receive (into/from queue)
    virtual void sendMessage(ofBuffer& message);
    virtual bool getNextMessage(ofBuffer& message);
    bool hasWaitingMessages();
    
    // message framing options
    void setDelimiter(char d);
    void setFixedReceiveSize(int s);
    
    // main socket update functions
    virtual void processRead();
    virtual void processWrite();
    
protected:

    StreamSocket* socketPtr;
    MessageFraming messageFraming;
    ofMutex queueMutex; // for the in/out queues
    
    // send receive queues
    queue<ofBuffer> sendBuffers;
    queue<ofBuffer> receiveBuffers;
    
    
    // 0. none- just grabbing this many bytes
    void readAvailableBytes();
    void writeAvailableBytes();
    
    // 1. read a 4 byte header with message size, then message
    void readHeaderAndMessage();
    void writeHeaderAndMessage();
    bool isHeaderComplete;
    int nextMessageSize;
    
    // 2. delimiter - messages broken up by single character
    char delimiter;
    vector<char> delimBuffers;
    void readDelimitedMessage();
    void writeDelimitedMessage();
    
    // 3. fixed size - messages are same size
    int fixedReceiveSize;
    void readFixedSizeMessage();
    void writeFixedSizeMessage();

};

} // namespace ofxPocoNetwork