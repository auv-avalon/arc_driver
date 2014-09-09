#include <arc_driver/ArcTokenDriver.hpp>
extern "C" {
#include <arc_driver/arc_packet.h>
}
#include <iostream>

int main(int argc, char** argv)
{
    arc_driver::ArcTokenDriver driver = arc_driver::ArcTokenDriver(OCU);
    driver.openSerial("/dev/ttyUSB1", 115200);
    driver.makeMeMaster();
    std::cout << "Device opened" << std::endl;
    int counter = 0;
    while (1){
        if (counter >1){
            counter = 0;
        driver.process(100);
        arc_packet_t packet;
        if (driver.readPacket(&packet)){
            std::cout << "Got a Arc Packet" << std::endl;
            std::cout << "    From Orginator: "; 
            switch (packet.originator) {
                case SLAVE:
                    std::cout << "SLAVE" << std::endl;
                    break;
                case MASTER:
                    std::cout << "MASTER" << std::endl;
                    break;
            }
            std::cout << "   To or From System: ";
            switch (packet.system_id){
                case AVALON:
                    std::cout << "Avalon" << std::endl;
                    break;
                case ASV:
                    std::cout << "ASV" << std::endl;
                    break;
                case OCU:
                    std::cout << "OCU" << std::endl;
                    break;
                case REGISTER_CHANCE:
                    std::cout << "All System with Register Chance" << std::endl;
                    break;
            }
            std::cout << "   Witch Packet Type: ";
            switch (packet.packet_id) {
                case PING:
                    std::cout << "PING" << std::endl;
                    break;
                case SET_STATE:
                    std::cout << "SET STATE" << std::endl;
                    break;
                case DGPS:
                    std::cout << "DGPS" << std::endl;
                    break;
                case STATUS:
                    std::cout << "STATUS" << std::endl;
                    break;
                case CONTROL:
                    std::cout << "CONTROL" << std::endl;
                    break;
                case ID_CAN:
                    std::cout << "ID CAN" << std::endl;
                    break;
                case MODE_CHANGE:
                    std::cout << "MODE CHANGE" << std::endl;
                    break;
                case ID_CAN_ACKED:
                    std::cout << "ID CAN ACKED" << std::endl;
                    break;
                case GIVE_TOKEN:
                    std::cout << "GIVE TOKEN" << std::endl;
                    break;
                case GIVE_BACK:
                    std::cout << "GIVE_BACK" << std::endl;
                    break;
                case REGISTER:
                    std::cout << "REGISTER" << std::endl;
                    break;
            }
        }
        }
        counter++;
        usleep(100000);
    }
    return 0;
}

