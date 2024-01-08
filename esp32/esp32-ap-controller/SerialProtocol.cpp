#include "SerialProtocol.h"

SerialProtocol::SerialProtocol(/* args */)
{
    current_state = wait;
    serialPacket = new SerialPacket();
    receivedPacket = new SerialPacket();
}

SerialProtocol::~SerialProtocol()
{
    delete serialPacket;
    delete receivedPacket;
}

int SerialProtocol::Send(unsigned char * packet)
{
    *data = *packet;
    serialPacket->Build(packet, sizeof(packet));
    return sendData();
}

int SerialProtocol::Receive(SerialPacket &packet)
{
    int result = recvData();
    if (result == 0)
    {
        packet = *receivedPacket;
    }
    else
    {
        return 1;
    }
    return 0;
}

int SerialProtocol::SendReceive(unsigned char * packet)
{
    current_state = send_data;
    *data = *packet;
    while (current_state != wait)
    {
        advance_state(current_state, send_data);
    }
}

int SerialProtocol::recvData()
{
    enum state_codes new_state = send_acknowledge;

    if (receivedPacket->Receive())
    {
        recieve_error = true;
    }
    if (receivedPacket->Check())
    {
        if (receivedPacket->packet[0] == 0xFA && receivedPacket->packet[1] == 0xFB)
        {
            if (receivedPacket->packet[3] == ACK)
            {
                ack_received = true;
                return 1;
            }
            else if (receivedPacket->packet[3] == NACK)
            {
                nack_received = true;
                return 2;
            }
            else if (receivedPacket->packet[3] != ACK && receivedPacket->packet[3] != NACK && receivedPacket->packet[3] < 20)
            {
                data_received = true;
                return 0;
            }
            else
            {
                recieve_error = true;
            }
        }
        else
        {
            recieve_error = true;
        }
    }
    else
    {
        recieve_error = true;
    }
    if (recieve_error)
    {
        return 3;
    }
}

int SerialProtocol::sendData()
{
    return serialPacket->Send();
}

state_codes SerialProtocol::GetCurrentState()
{
    return current_state;
}

int SerialProtocol::sendAck()
{
    serialPacket = new SerialPacket();
    unsigned char ack[5] = {ACK, 0x00};
    serialPacket->Build(ack, sizeof(ack));
    return sendData();
}

state_codes SerialProtocol::run_cmd()
{
    return wait;
}

void SerialProtocol::clearRecvState()
{   
    ack_received = false;
    data_received = false;
    recieve_error = false;
    nack_received = false;
}

void SerialProtocol::discardData()
{
    receivedPacket = new SerialPacket();
}

int SerialProtocol::advance_state()
{
    return advance_state(current_state, wait);
}

int SerialProtocol::advance_state(int currentState, state_codes event)
{
    switch (current_state)
    {
        case wait:
            clearRecvState();
            if (Serial2.available() > 0)
            {
                next_state = receive_data;
            }
            else
            {
                next_state = wait;
            }
            break;
        case send_data:
            Send(data);
            next_state = receive_acknowledge;
            break;
        case receive_data:
            {
                int result = recvData();
                if (result == 0)
                {
                    next_state = send_acknowledge;
                }
                else if (result == 1)
                {
                    next_state = wait;
                }
                else if (result == 2)
                {
                    next_state = send_data;
                }
                else if (result == 3)
                {
                    next_state = discard_data;
                }
            }
            break;
        case send_acknowledge:
            sendAck();
            next_state = run_command;
            break;
        case receive_acknowledge:
            {
                int result = recvData();
                if (result == 1)
                {
                    next_state = wait;
                }
                else if (result == 2)
                {
                    next_state = send_data;
                }
                else if (result == 3)
                {
                    next_state = discard_data;
                }
                else
                {
                    //dont know what to do here
                    //next_state = run_command;
                    next_state = wait;
                }
            }
            break;
        case run_command:
            {
                enum state_codes next = run_cmd();
                next_state = next;
            }
            break;
        case discard_data:
            discardData();
            next_state = wait;
            break;
    }
    current_state = next_state;
}

