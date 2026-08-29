
// circular_buffer.c
// the .c file for circular buffer

#include "circular_buffer.h"
#include <stdio.h>

void cb_init(CircularBuffer* cb){
    cb->head = 0;
    cb->tail = 0;
}
//cb_isEmpty()
//returns true if empty false if not empty 
bool cb_isEmpty(CircularBuffer* cb){
    if (cb->head == cb->tail){
        return true;
    }else{
        return false;
    }
}
//cb_isFull
//return true if full
bool cb_isFull(CircularBuffer* cb){
    if((cb->tail+1)%CB_SIZE == cb->head){
        return true;
    }else{
        return false;
    }
}
//cb_put()
//puts a new character into the buffer 
//returns  true if it inserted 
// if cb is full it will overwrite the oldest data
bool cb_put(CircularBuffer* cb, uint8_t data){
    if(cb_isFull(cb)){ //if fulls tart overwriting the oldest data
        cb->head = (cb->head+1)%CB_SIZE;
    }
    
    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail+1) % CB_SIZE;
    return true;
}
//cb_get()
//gets a character from the buffer
//returns false if the cb is empty, returns true if cb was getted from 
bool cb_get(CircularBuffer* cb, uint8_t* data){
    if(cb_isEmpty(cb)==true){
        return false;
    }
    *data = cb->buffer[cb->head];
    cb->head = (cb->head+1)%CB_SIZE;
    return true;
}

#define testing_cb
#ifdef testing_cb
/*testing cb*/

void app_main(void)
{
    int total = 0;
    int pass = 0;
    CircularBuffer cb;
    cb_init(&cb);
    total++;
    if(cb_isEmpty(&cb)){
        pass++;
    }else{
        printf("cb_isEmtpy fail 1\n");
    }
    cb_put(&cb,'h');

    total++;
    if(cb_isEmpty(&cb)==false){
         pass++;
    }else{
        printf("cb_isEmtpy fail 2\n");
    }

    total++;
    uint8_t cb_out;
    cb_get(&cb,&cb_out);
    if(cb_out == 'h'){
        pass++;
    }else{
       printf("cb_get fail 1\n");
    }
     total++;
    if(cb_isEmpty(&cb)==true){
        pass++;
    }else{
         printf("cb_isEmtpy fail 3\n");
    }
    
    uint8_t fill_count = 0;
    
    while(cb_isFull(&cb) == false){
        cb_put(&cb,'a');
        fill_count ++;
    }
    cb_put(&cb,'z'); //extra to test overwrite behavrior
    total++;
    if(cb_isFull(&cb)==true ){
        pass++;
    }else{
        printf("cb_isfull fail 1\n");
    }
    
    uint16_t read_count = 0;
    uint16_t a_count_expected = CB_SIZE-2;
    uint16_t z_count_expected =1;

    uint16_t a_count =0;
    uint16_t z_count =0;
    
    while(cb_isEmpty(&cb)==false){
        cb_get(&cb,&cb_out);
        if(cb_out == 'a'){ //succesfuly deques a cb_size times 
            read_count++;
            a_count++;
        }
        if(cb_out == 'z'){
            printf("successfully overwrote and found z\n");
             z_count++;
             read_count++;
        }
    }
    total++;
    if(a_count == a_count_expected){
        pass++;
    }

    total++;
    if(z_count == z_count_expected){
        pass++;
    }
    total++;
    if(read_count == CB_SIZE-1){
        pass++;
    }

    



    printf("%d / %d\n", pass, total);
}

#endif 
