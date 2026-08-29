// circular_buffer.h
// the header file for circular buffer functions 
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include <stdint.h>
#include <stdbool.h>

#define CB_SIZE 256

typedef struct {
    volatile uint16_t head; //head next byte to be read 
    volatile uint16_t tail; //tail next byte to be write 
    uint8_t buffer[CB_SIZE]; 
}CircularBuffer;

// intiializes the circular buffer
void cb_init(CircularBuffer* cb);

// checks if the circular buffer is empty
// returns true if it is 
bool cb_isEmpty(CircularBuffer* cb);

// checks if the circular buffer is full
// returns true if it is 
bool cb_isFull(CircularBuffer* cb);

// puts a new byte into the buffer
// returns false if cb is full
// returns true if it is inserted
bool cb_put(CircularBuffer* cb, uint8_t data);

// gets a byte from teh buffer 
//returns false if the cb is empty, returns true if cb was getted from 
bool cb_get(CircularBuffer* cb, uint8_t* data);
#endif
