#ifndef _ARCDRIVER_ARC_TOKEN_DRIVER_HPP_
#define _ARCDRIVER_ARC_TOKEN_DRIVER_HPP_
extern "C" {
#include "arc_packet.h"
}
#include <base/Time.hpp>
#include <vector>
#include <boost/circular_buffer.hpp>
#include "ArcDriver.hpp"
#include "ArcInterface.hpp"
namespace arc_driver
{
    static bool isTokenPacket(arc_packet_t* packet){
        if (packet->packet_id == GIVE_TOKEN ||  packet->packet_id == REGISTER || packet->packet_id == GIVE_BACK) {
            return true;
        }
        return false;
    }
    enum TokenRegisterState {UNREGISTERED, WAITING_FOR_TOKEN, REGISTERED};
    //Represents the State of a Station ON the master
    struct StationState {
        //The ID of this station
        ARC_SYSTEM_ID id;
        //How often the master don't get the token back since the last time
        unsigned int timeouts;
        base::Time last_connection;
    };
    struct MasterState {
        //No Station has the token. It's free.
        bool free_token;
        //This station has the token or should get it
        unsigned int current_station;
        //All registered stations
        std::vector<StationState> station_states;
        //The time of giving the token to the current stattion
        //it's needed to timeout the station if the station don't give back
        //the token
        base::Time last_free_token;
        //Token Changes since last Register Chance
        unsigned int register_chance;
    };
    //This struct represents the state of a client ON the client
    struct TokenClientState {
        //Has this client the token?
        bool has_token;
        //Am I registered?
        TokenRegisterState register_state;
        //How many bytes i have send since i have the token
        unsigned int send_bytes;
        //Time of getting the last token
        base::Time last_token;
        //Register Chance Penalty
        //This Counter represents how many register chances must not used from this client
        //Because of a possible register collision the last time
        unsigned int register_penalty;
    };
    class ArcTokenDriver : public ArcInterface
    {
        private:
            bool master;
            TokenClientState token_client_state;
            MasterState state_master;
            boost::circular_buffer<arc_packet_t>* readBuffer; 
            boost::circular_buffer<arc_packet_t>* sendBuffer;
            StationState* getStationState(ARC_SYSTEM_ID system);
            ArcDriver driver;
            bool handleMasterCommand(arc_packet_t* packet);
            bool handleCommand(arc_packet_t* packet);
            bool isUnregistered(ARC_SYSTEM_ID system);
            ARC_SYSTEM_ID station_id;
            void sendRegisterPacket();
            void sendGTTPacket(ARC_SYSTEM_ID to_station);
            void sendGTBPacket();
            int writePacket(arc_packet_t* packet);
            void handleToken();
            void handleIncommingPackets();
            bool sendPackets();
            void giveTokenBack();
        public:
            ArcTokenDriver(ARC_SYSTEM_ID station_id);
            ~ArcTokenDriver();
            void openSerial(std::string const& port, int baudrate);
            int sendPacket(arc_packet_t* packet);
            int readPacket(arc_packet_t* packet);
            int process(int sendtimeout);
            void makeMeMaster();
            bool isValid();
    };
}
#endif
