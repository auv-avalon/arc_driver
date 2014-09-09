#ifndef _ARCDRIVER_ARCDRIVER_FILTER_TOKEN_HPP_
#define _ARCDRIVER_ARCDRIVER_FILTER_TOKEN_HPP_
#include <string> 
#include "arc_packet.h"
#include "ArcDriver.hpp"
#include "ArcInterface.hpp"
extern "C" {
#include "arc_packet.h"
}
namespace arc_driver
{
    class ArcDriverFilterToken : public ArcInterface
    {
        public:
            ArcDriverFilterToken();
            void openSerial(std::string const& device, int baudrate);
            void openUDP(std::string const& ip, int port);
            void openInet(std::string const& ip, int port);
            int readPacket(arc_packet_t* packet); 
            int sendPacket(arc_packet_t* packet);
            int process(int sendtimeout);
            bool isValid();
        private:
            ArcDriver driver;
    };
};
#endif
