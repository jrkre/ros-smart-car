#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include "DeviceDriverSet.h"
#include "Arduino.h"

/* Argument types */
#define ARGINT 0x3B  // Positive int (LSB, MSB)
#define ARGNINT 0x1B // Negative int (LSB, MSB)
#define ARGSTR 0x2B  // String (Note: 1st byte is length!!)

/* Command Headers */
enum commands
{
    ENABLE = 0,
    MOVE = 1,       // sends twist packet
    SERVO = 2, // sends servo packet
    ULTRASONIC = 3, // reqests ultrasonic packet
    JOY_INFO = 4,   //
    STOP = 5,
    AP_CREATED = 6,
    AP_FAILED = 7,
    AP_CONNECTED = 8,
    AP_DISCONNECTED = 9,
    ACK = 10,
    NACK = 11,
    KEEPALIVE = 12,
    LINE_SENSOR = 13,
    LEFT_GROUND = 14,
    VOLTAGE = 15,
    ACCEL = 16
};




/**
 * @brief every serial packet looks like this before being built:
 *
 * unsigned char packet[i];
 *
 * packet[0] = COMMAND HEADER
 * packet[1] = ARGUMENT TYPE
 * packet[2] = ARG LENGTH
 * packet[3] = ARGUMENT
 * packet[4] = CHECKSUM
 * 
 * after build:
 * 
 * packet[0] = 0xFA
 * packet[1] = 0xFB
 * packet[2] = PACKET SIZE
 * packet[3] = COMMAND HEADER
 * packet[4] = ARGUMENT TYPE
 * packet[5] = ARG LENGTH
 * packet[n] = ARGUMENT
 *
 */
class SerialPacket
{

#define BAUD_RATE 115200
#define PACKET_SIZE 256

public:
    SerialPacket();
    SerialPacket(String port);
    ~SerialPacket();

    String portFilePath;

    unsigned char size;
    unsigned char packet[PACKET_SIZE];

    int fd;

    bool init();
    void Print();
    // void PrintHex();
    bool Build(unsigned char *data, unsigned char datasize);
    int CalcCheckSum();
    int Send();
    int Receive();
    bool Check();

    commands GetCommand();

    unsigned char GetArgLength();

    unsigned char GetArgType();

    unsigned char *GetArg();

    bool operator==(SerialPacket p)
    {
        if (size != p.size)
        {
            return false;
        }

        if (memcmp(packet, p.packet, size) != 0)
        {
            return false;
        }

        return true;
    }

    bool operator!=(SerialPacket p)
    {
        if (size != p.size)
        {
            return true;
        }

        if (memcmp(packet, p.packet, size) != 0)
        {
            return true;
        }

        return false;
    }
    void printPacket();

private:

};

#endif