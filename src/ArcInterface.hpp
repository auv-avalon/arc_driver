#ifndef _ARCDRIVER_ARC_INTERFACE_HPP_
#define _ARCDRIVER_ARC_INTERFACE_HPP_
namespace arc_driver
{
    class ArcInterface {
        public:
            virtual ~ArcInterface() {}
            virtual void openSerial(std::string const& port, int baudrate) = 0;
            virtual int sendPacket(arc_packet_t* packet) = 0;
            virtual int readPacket(arc_packet_t* packet) = 0;
            virtual int process(int sendtimeout) = 0;
            virtual bool isValid() = 0;
    };
}
#endif
