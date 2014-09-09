#include "ArcDriverFilterToken.hpp"
#include "ArcTokenDriver.hpp"

extern "C" {
#include "arc_packet.h"
}

using namespace arc_driver;


ArcDriverFilterToken::ArcDriverFilterToken(){
    //Nothing todo yet
}
void ArcDriverFilterToken::openSerial(std::string const& device, int baudrate){
    driver.openSerial(device, baudrate);
}
void ArcDriverFilterToken::openInet(std::string const& ip, int port){
    driver.openInet(ip.c_str(), port);
}
void ArcDriverFilterToken::openUDP(std::string const& ip, int port){
    driver.openUDP(ip.c_str(), port);
}

int ArcDriverFilterToken::readPacket(arc_packet_t* packet){
    int ret;
    if (ret = driver.getPacket(packet, 0, 0)) {
        if (isTokenPacket(packet)){
            //TODO warn;
            return 0;
        } else {
            return ret; 
        }
    }
}

int ArcDriverFilterToken::sendPacket(arc_packet_t* packet){
    if (!isTokenPacket(packet)) {
        return driver.sendPacket(packet);
    } else {
        throw std::runtime_error("You try to send a Protocol Packet via none Protocol Driver.");
    }
}

int ArcDriverFilterToken::process(int sendtimeout){
    //Nothing special todo here
    return 0;
}

bool ArcDriverFilterToken::isValid(){
    return driver.isValid();
}
