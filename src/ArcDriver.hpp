#ifndef _ARCDRIVER_ARCDRIVER_HPP_
#define _ARCDRIVER_ARCDRIVER_HPP_
#include <iodrivers_base/Driver.hpp>
extern "C" {
#include "arc_packet.h"
}
namespace arc_driver
{
    class ArcDriver : public iodrivers_base::Driver
    {
        private:
            int extractPacket (uint8_t const *buffer, size_t buffer_size) const;
        public:
            ArcDriver();
            int getPacket(arc_packet_t* packet, int timeout, int first_byte_timeout);
            int sendPacket(arc_packet_t* packet);
    };
}
#endif
