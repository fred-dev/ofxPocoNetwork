
#include "UDPClient.h"

namespace ofxPocoNetwork {
    
UDPClient::UDPClient() {
    receiveSize = 256;
    sleepTime = 16;
    connected = false;
    waitingRequest = false;
}

UDPClient::~UDPClient() {
    disconnect();
}

void UDPClient::disconnect() {
    
    connected = false;
    if(socketAddress) delete socketAddress;
    if(socket) delete socket;
}

void UDPClient::connect(string ipAddr, int port,int sourcePort) {

    // setup tcp poco client
    socketAddressSource = new Poco::Net::SocketAddress(ipAddr, sourcePort);
    socketAddress = new Poco::Net::SocketAddress(ipAddr, port);
    socket = new Poco::Net::DatagramSocket(*socketAddressSource);
    socket->setReuseAddress(true);
    
    
    
    try {
        
        // client must connect to server
        socket->bind(*socketAddressSource);
        socket->connect(*socketAddress);
        connected = true;
        ofLog() << "UDPClient connected";
        ofLog() << "Max receive size: " << socket->getReceiveBufferSize();
        ofLog() << "Max send size: " << socket->getSendBufferSize(); // 9216
        
        
    } catch (Poco::Exception& exc) {
        
        disconnect();
        ofLog() << "UDPClient Error: could not create socket- " << exc.displayText();
    }
}

int UDPClient::getDatagramSourcePort(){
        return socket->impl()->address().port();
        
    }

// send
//--------------------------------------------------------------
int UDPClient::sendMessage(string msg) {
    
    if(connected) {
        ofBuffer buffer(msg);
        return sendMessage(buffer);
    }
    return 0;
}

int UDPClient::sendMessage(ofBuffer& buffer) {
    if(connected) {
        try {
            int sent = socket->sendBytes(buffer.getData(), buffer.size());
            //int sent = socket->sendTo(buffer, sendSize, *socketAddress);
            return sent;
        } catch (Poco::Exception &e) {
            ofLogError() << "* UDPClient send fail 1";
            return 0;
        } catch(std::exception& exc) {
            ofLogError() << "* UDPClient send fail 2";
            return 0;;
        } catch (...) {
            ofLogError() << "* UDPClient send fail 3";
            return 0;
        }
    }
    return 0;
}
    void UDPClient::threadedFunction(){
        
        while( isThreadRunning() ){
            
            if(connected) {
                
                // currently only setup for receiving
                // receive message - blocks thread until message
                try {
                    
                    // receive
                    ofBuffer buffer;
                    buffer.allocate(receiveSize);
                    Poco::Net::SocketAddress sender; // use this to identify the client
                    int n = socket->receiveFrom(buffer.getData(), buffer.size(), sender);
                    
                    // who sent message (sender)
                    //ofLog() << "Received message from: " << sender.toString() << ", size: " << n;
                    //ofLog() << "Message: " << buffer.getData();
                    
                    
                    // copy/replace buffer / or push into queue
                    mutex.lock();
                    receiveBuffers.push(buffer);
                    mutex.unlock();
                    
                    // test: send a message back to sender- works
                    //int sent = socket->sendTo("hello", 5, sender);
                    //ofLog() << "Sent back: " << sent;
                    
                } catch (Poco::Exception &e) {
                    ofLogError() << "* UDPServer read fail 1";
                    //disconnect();
                } catch(std::exception& exc) {
                    ofLogError() << "* UDPServer read fail 2";
                    //disconnect();
                } catch (...) {
                    ofLogError() << "* UDPServer read fail 3";
                    //disconnect();
                }
            }
            
            sleep(sleepTime);
        }
        
    }
    
    void UDPClient::setReceiveSize(int size) {
        Poco::ScopedLock<ofMutex> lock(mutex);
        receiveSize = size;
    }
    int UDPClient::parseMessage() {
        Poco::ScopedLock<ofMutex> lock(mutex);
        return receiveBuffers.size();
    }
    bool UDPClient::hasWaitingMessages() {
        Poco::ScopedLock<ofMutex> lock(mutex);
        return receiveBuffers.size() > 0;
    }
    
    bool UDPClient::getNextMessage(ofBuffer& message) {
        Poco::ScopedLock<ofMutex> lock(mutex);
        if(receiveBuffers.size()) {
            message = receiveBuffers.front();
            receiveBuffers.pop();
            return true;
        }
        return false;
    }
    bool UDPClient::getNextMessage(uint8_t msg[]) {
        Poco::ScopedLock<ofMutex> lock(mutex);
        if(receiveBuffers.size()) {
            msg = (uint8_t*)receiveBuffers.front().getData();
            receiveBuffers.pop();
            return true;
        }
        return false;
    }
    bool UDPClient::getNextMessage(string& msg) {
        Poco::ScopedLock<ofMutex> lock(mutex);
        if(receiveBuffers.size()) {
            msg = receiveBuffers.front().getData();
            receiveBuffers.pop();
            return true;
        }
        return false;
    }
// advanced- internal poco buffer sizes
//--------------------------------------------------------------
void UDPClient::setMaxSendSize(int size) {
    if(connected) {
        
        // advanced: change pocos max internal buffer send size- default send is 9216
        socket->setSendBufferSize(size);
        ofLog() << "Max send size: " << socket->getSendBufferSize();
    }
}

int UDPClient::getMaxSendSize() {
    if(connected) {
        return socket->getSendBufferSize();
    }
    return 0;
}

} // namespace ofxPocoNetwork