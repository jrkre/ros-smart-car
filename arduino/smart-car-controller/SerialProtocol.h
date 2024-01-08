#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include "SerialPacket.h"

enum state_codes
{
    wait,
    send_data,
    receive_data,
    send_acknowledge,
    receive_acknowledge,
    run_command,
    discard_data
};

class SerialProtocol {

public:

    SerialProtocol();

    ~SerialProtocol();

    int Send(unsigned char *data);
    
    int Receive(SerialPacket &packet);

    //only really used for calling out of class
    int SendReceive(unsigned char *data);

    state_codes GetCurrentState();

    virtual int advance_state();


protected:
    
    /**
     * @brief should capture a packet, and then return an int based on whether or not it's an ack, nack, or data
     * 
     * @return int 
     * 0: data
     * 1: ack
     * 2: nack
     * 3: error
     */
    int recvData ();

    int sendData();

    int sendAck();

    enum state_codes run_cmd();

    /* data */
    state_codes current_state;
    state_codes next_state;

    unsigned char *data;
    SerialPacket *serialPacket;
    SerialPacket *receivedPacket;

private:

    int advance_state(int current_state, state_codes event);

    void clearRecvState();

    void discardData();


    /* state flags */
    bool ack_received;
    bool data_received;
    bool recieve_error;
    bool nack_received;

    union TimestampData
    {
        unsigned char data[4];
        float timestamp;
    } timestampData;
    



};

#endif //SERIAL_PROTOCOL_H