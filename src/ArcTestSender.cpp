#include <arc_driver/ArcDriver.hpp>
extern "C" {
#include <arc_driver/arc_packet.h>
}
#include <iostream>

int main(int argc, char** argv)
{
    arc_driver::ArcDriver driver;
    driver.openURI("serial:///dev/ttyUSB0:115200");
    std::cout << "Device opened" << std::endl;
    arc_packet_t packet;
    packet.originator = MASTER;
    packet.system_id = ASV;
    packet.packet_id = PING;
    packet.length = 0;
    while (1){
        driver.sendPacket(&packet);
    }
    return 0;
}

