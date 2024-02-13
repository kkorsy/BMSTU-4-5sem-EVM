#include <stdlib.h>
#include <ctime>
#include <sys/time.h>
#include "lnh64.hxx"
#include "gpc_io_swk.h"
#include "gpc_handlers.h"
// #include "iterators.h"
#include "common_struct.h"
#include "compose_keys.hxx"

#define __fast_recall__

using namespace std;

extern lnh lnh_core;
volatile unsigned int event_source;

int main(void) {
    /////////////////////////////////////////////////////////
    //                  Main Event Loop
    /////////////////////////////////////////////////////////
    //Leonhard driver structure should be initialised
    lnh_init();
    for (;;) {
        //Wait for event
        event_source = wait_event();
        switch(event_source) {
            /////////////////////////////////////////////
            //  Measure GPN operation frequency
            /////////////////////////////////////////////
            case __event__(update) : update(); break;
            case __event__(select) : select(); break;
        }
        set_gpc_state(READY);
    }
}
    
//-------------------------------------------------------------
//      Вставка ключа и значения в структуру
//-------------------------------------------------------------
 
void update() {
    while(1){
                tables::key key=tables::key::from_int(mq_receive());
                if (key==-1ull) break;
                tables::val val=tables::val::from_int(mq_receive());
                TABLES.ins_async(key, val); //Вставка в таблицу с типизацией uint64_t
        } 
}


//-------------------------------------------------------------т
//      Передать все роли пользователя и время доступа 
//-------------------------------------------------------------

void select() {
    while (1)
    {
        uint64_t qip = mq_receive();
        if (qip == -1ull)
            break;
        // auto crole = TABLES.nsm(tables::key{.ip=qip});
        auto interface = TABLES.search(tables::key{.ip=qip});
        if (interface == -1)
            mq_send(-1);
        else
            mq_send(interface.value().val);
    }
}
