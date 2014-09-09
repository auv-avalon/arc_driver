#include "ArcTokenDriver.hpp"
#define REGISTERCHANCE 20
#define SEND_LIMIT 30

namespace arc_driver{

ArcTokenDriver::ArcTokenDriver(ARC_SYSTEM_ID station_id){
    station_id = station_id;
    sendBuffer = new boost::circular_buffer<arc_packet_t>(1000);
    readBuffer = new boost::circular_buffer<arc_packet_t>(1000);
    master = false;
    token_client_state.has_token = false;
    token_client_state.register_state = UNREGISTERED;
    token_client_state.send_bytes = 0;
    token_client_state.register_penalty = 0;
    //Just master variables
    state_master.free_token = true;
    state_master.current_station = 0;
    srand(0);
}
ArcTokenDriver::~ArcTokenDriver(){
    delete sendBuffer;
    delete readBuffer;
}
StationState* ArcTokenDriver::getStationState(ARC_SYSTEM_ID system){
    for (std::vector<StationState>::iterator it = state_master.station_states.begin(); it != state_master.station_states.end(); it++){
        if (it->id == system){
            return &(*it);
        }
    }
    throw std::runtime_error("Packet from unknown Station");
}
void ArcTokenDriver::openSerial(std::string const& device, int baudrate){
    driver.openSerial(device, baudrate);
}
bool ArcTokenDriver::handleMasterCommand(arc_packet_t* packet){
    //Only Masters should handle master commands
    if (!master){
        throw std::runtime_error("this function is only for masters");
    }
    switch(packet->packet_id){
        case REGISTER:
            if (isUnregistered(packet->system_id)){
                StationState new_station;
                new_station.id = packet->system_id;
                new_station.timeouts = 0;
                new_station.last_connection = base::Time::now();
                std::cout << "A System is Registered via Amber. Station Number" << state_master.station_states.size() << std::endl;
                state_master.station_states.push_back(new_station);
            } else {
                //The Master can't do anything here.
                //Either there are two systems with the same system ID or
                //one systems registered two times
            }
            return true;
        case GIVE_BACK:
            {
                state_master.free_token = true; 
                StationState* state = getStationState(packet->system_id);
                state->timeouts = 0;
                state->last_connection = base::Time::now();
            }
            return true;
        case GIVE_TOKEN:
            //Something really went wrong here are two masters in the ARC Communication!
            //TODO react!
            return true;
        default:
            return false;
    }
    return false;
}
bool ArcTokenDriver::sendPackets(){
    if (token_client_state.has_token){
        while (token_client_state.has_token && !sendBuffer->empty()){
            if (token_client_state.send_bytes+sendBuffer->front().length+ARC_MIN_FRAME_LENGTH <= SEND_LIMIT){
                if (writePacket(&sendBuffer->front())){
                    token_client_state.send_bytes += sendBuffer->front().length+ARC_MIN_FRAME_LENGTH;
                    sendBuffer->pop_front();
                } else {
                    token_client_state.has_token = false;
                }
            } else {
                token_client_state.has_token = false;
            }
        }
        giveTokenBack();
    }
    return true;
}
void ArcTokenDriver::sendRegisterPacket(){
    arc_packet_t packet;
    packet.originator = SLAVE;
    packet.system_id = station_id;
    packet.packet_id = REGISTER;
    packet.length = 0; 
    writePacket(&packet);
}

void ArcTokenDriver::sendGTBPacket() {
    arc_packet_t packet;
    packet.originator = SLAVE;
    packet.system_id = station_id;
    packet.packet_id = GIVE_BACK;
    packet.length = 0; 
    writePacket(&packet);
}
void ArcTokenDriver::sendGTTPacket(ARC_SYSTEM_ID to_station){
    arc_packet_t packet;
    packet.originator = MASTER;
    packet.system_id = to_station;
    packet.packet_id = GIVE_TOKEN;
    packet.length = 0; 
    writePacket(&packet);
}

bool ArcTokenDriver::handleCommand(arc_packet_t* packet){
    switch (packet->packet_id){
        case GIVE_TOKEN:
            if (packet->system_id == station_id){
                token_client_state.has_token = true;
                token_client_state.send_bytes = 0;
                token_client_state.register_penalty = 0;
                token_client_state.last_token = base::Time::now();
                token_client_state.register_state = REGISTERED;
            } else if (token_client_state.register_state == UNREGISTERED && packet->system_id == REGISTER_CHANCE) {
                //only if there is no register penalty left
                if (!token_client_state.register_penalty){ 
                    sendRegisterPacket();
                    token_client_state.register_penalty = rand() % 5;
                } else {
                    //if i don't use this chance my penalty is reduced
                    token_client_state.register_penalty--;
                }

            } 
            return true;
        case GIVE_BACK:
        case REGISTER:
            //A no master client shouldn't handle this packet but they are no data packets
            return true;
        default:
            return false;
    }
    return false;

}
void ArcTokenDriver::handleToken(){
    if (!master) {
        throw std::runtime_error("Token Handling should be done by masters only");
    }
    bool timeout = false;
    if (base::Time::now().toSeconds() - state_master.last_free_token.toSeconds() > 0.5){
        timeout = true;
        state_master.station_states[state_master.current_station].timeouts++;
        //TODO Station unterumstaenden abmelden
    }
    if (state_master.free_token || timeout){
        //Have i give clients a chance to register at master?
        if (state_master.register_chance >= REGISTERCHANCE){
            state_master.register_chance = 0;
            sendGTTPacket(REGISTER_CHANCE);
        } else {
            state_master.register_chance++;
            state_master.current_station = (state_master.current_station+1)%state_master.station_states.size();
            ARC_SYSTEM_ID next_station = state_master.station_states[state_master.current_station].id;
            if (next_station == station_id){
                token_client_state.has_token = true;
                token_client_state.send_bytes = 0;
                token_client_state.register_penalty = 0;
                token_client_state.last_token = base::Time::now();
                token_client_state.register_state = REGISTERED;
            } else {
                sendGTTPacket(next_station);
            }
        }
        state_master.free_token = false;
        state_master.last_free_token = base::Time::now();
    }
}

void ArcTokenDriver::handleIncommingPackets(){
    arc_packet_t packet;
    while (driver.getPacket(&packet, 1, -1)){
        if (master){
            if (!handleMasterCommand(&packet)){
                readBuffer->push_back(packet);
            }
        } else {
            if (!handleCommand(&packet)){
                readBuffer->push_back(packet);
            }
        }
    }
}
bool ArcTokenDriver::isUnregistered(ARC_SYSTEM_ID system){
    for (std::vector<StationState>::iterator it = state_master.station_states.begin(); it != state_master.station_states.end(); it++){
        if (it->id == system){
            return false;
        }
    }
    return true;
}

int ArcTokenDriver::process(int sendtimeout){
    //handle Incomming Data
    handleIncommingPackets();

    //handle send rights
    if (master) {
        handleToken();
    }
    if (!sendPackets()){
        std::cout << "Something went wrong by sending Packets)" << std::endl;
    }
    return sendBuffer->size();
}
int ArcTokenDriver::writePacket(arc_packet_t* packet){
    driver.sendPacket(packet);
}

int ArcTokenDriver::sendPacket(arc_packet_t* packet){
    if (isTokenPacket(packet)){
        throw std::runtime_error("You try to send a Protocol packet");
    }
    sendBuffer->push_back(*packet);
    return 1;
}
int ArcTokenDriver::readPacket(arc_packet_t* packet){
    if (!readBuffer->empty()){
        *packet = readBuffer->front();
        readBuffer->pop_front();
        return packet->length;
    }
    return 0;
}

void ArcTokenDriver::makeMeMaster(){
    StationState new_station;
    new_station.id = station_id;
    new_station.timeouts = 0;
    new_station.last_connection = base::Time::now();
    state_master.station_states.push_back(new_station);
    master = true;
    token_client_state.register_state = REGISTERED;
}

void ArcTokenDriver::giveTokenBack(){
    token_client_state.has_token = false;
    if (master){
        state_master.free_token = true;
    } else {
        sendGTBPacket();
        token_client_state.send_bytes = 0;
    }
}
bool ArcTokenDriver::isValid(){
    return driver.isValid();
}
}
