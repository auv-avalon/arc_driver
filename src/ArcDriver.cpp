#include "ArcDriver.hpp"
#include <iodrivers_base/Exceptions.hpp>
extern "C" {
#include "arc_packet.h"
}
namespace arc_driver{

ArcDriver::ArcDriver()
: iodrivers_base::Driver(ARC_MAX_FRAME_LENGTH)
{

}

int ArcDriver::getPacket(arc_packet_t* packet, int timeout, int first_byte_timeout){
    uint8_t buffer[ARC_MAX_FRAME_LENGTH];
    size_t size = 0;
    try {
        size = Driver::readPacket(buffer, ARC_MAX_FRAME_LENGTH, timeout, first_byte_timeout);
    } catch (iodrivers_base::TimeoutError timeout){
    //} catch (...){
        //ignore Timeout Errors
    }
    int len = 0;

    //A Packet in Buffer?
    if (size) {
        len = parsePacket(buffer, size, packet);
    }
    return len;
}

int ArcDriver::sendPacket(arc_packet_t* packet){
    unsigned int size;
    uint8_t buffer[ARC_MAX_FRAME_LENGTH];
    size = createPacket(packet, buffer);
    return Driver::writePacket(buffer, size, 3000);
}

int ArcDriver::extractPacket(uint8_t const *buffer, size_t buffer_size) const
{
    arc_packet_t packet;
    return parsePacket(buffer, buffer_size, &packet);
}
}
