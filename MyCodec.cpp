//Created by Riku Konishi
#pragma once
#include <Arduino.h>


const uint8_t START_MARKER = 195;
const uint8_t END_MARKER   = 231;


//For UART communication
//Max data size is 8 bytes
//This allows you to transmit precise data and discard corrupted packets
//Generates a marker (key) to avoid collisions between payload data and start/end flags
class Codec {
    public:
        void encode(uint8_t *message_write, int start, int end, int keyindex); //Encode message
        int decode(uint8_t *message_read, int start, int end, int keyindex); //Decode message
    private:
        int key;
        bool error;
};


void Codec::encode(uint8_t *message_write, int start, int end, int keyindex) {
    if((end - start + 1) <= 8) {
        uint8_t key = 0;
        for (int i = start; i <= end; i++) {
            if (message_write[i] == START_MARKER) {
                message_write[i] = START_MARKER - 1;
                key |= (1 << (i - start));
            } else if (message_write[i] == END_MARKER) {
                message_write[i] = END_MARKER - 1;
                key |= (1 << (i - start));
            }
        }
        message_write[keyindex] = key; //Bitmask
    }
}

int Codec::decode(uint8_t *message_read, int start, int end, int keyindex) {
    error = 0;
    if((end - start + 1) <= 8) {
        for (int i = start; i <= end; i++) {
            if (message_read[keyindex] & (1 << (i - start))) {
                if (message_read[i] == (START_MARKER - 1)) message_read[i] = START_MARKER;
                else if (message_read[i] == (END_MARKER - 1)) message_read[i] = END_MARKER;
                else error = 1;
            }
        }
    }
    return error;
}
