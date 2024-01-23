//  *******************************************************************************
//  
//  mpfw / fw2 - Multi Platform FirmWare FrameWork
//  Copyright (C) (2023) Marco Dau
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  You can contact me by the following email address
//  marco <d o t> ing <d o t> dau <a t> gmail <d o t> com
//  
//  *******************************************************************************
#include <iostream>
#include <unistd.h>
#include <thread>

#include <ncurses.h>

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.



#ifndef SDK_USART_H
#define SDK_USART_H

class sdk_usart {

    public:

    typedef void    (*callback_protocol_x_buffer_management_t)  (void* p_ptrData);
    typedef uint8_t (*callback_protocol_status_get_t)           (void);
    typedef void    (*callback_protocol_status_set_t)           (void);

    typedef struct _callback_protocol_functions {
        callback_protocol_x_buffer_management_t tx_buffer_management;
        callback_protocol_x_buffer_management_t rx_buffer_management;

        callback_protocol_status_get_t          tx_status_get__running;
        callback_protocol_status_get_t          tx_status_get__completed;
        callback_protocol_status_get_t          rx_status_get__running;
        callback_protocol_status_get_t          rx_status_get__completed;

        callback_protocol_status_set_t          tx_status_set__free;
        callback_protocol_status_set_t          rx_status_set__free;
        callback_protocol_status_set_t          tx_status_set__running;
        callback_protocol_status_set_t          rx_status_set__running;

        callback_protocol_status_get_t          rx_status_get__free;
        callback_protocol_status_set_t          tx_status_set__completed;
    } callback_protocol_functions_t;


    // callback protocol functions
    static void     reg_write               (void* p_ptrData, char c);
    static uint8_t  reg_read                (void* p_ptrData);
    static void     tx_enable               (void* p_ptrData);
    static void     rx_enable               (void* p_ptrData);
    // static void     tx_disable              (void* p_ptrData, char c);
    // static void     rx_disable              (void* p_ptrData, char c);


    // interrupt equivalent function
    static void     start                    (std::string p_param);
    static bool     loop                     (void);
    static void     end_0                    (void);
    static void     end                      (void);
    static bool     check_loop_end           (void);
    static void     rx_stop                  (void);


    static void     service_message     (int row, char const *p_CharBufferTx, int p_par1, int p_par2);

    private:


    static int ch;
    static int ch_0;
    static int g_c;
    static int g_tx_c;
    static bool g_shamem_fifo_full;
    static uint8_t g_shm_status;
    static int startx;
    static int starty;
    static bool g_rx_stop;

    static bool g_test_1;
    static bool g_test_2;

    static int g_rx_cns_data;

    static uint16_t g_start_counter;
    static uint16_t g_start_counter_max;
    static uint32_t g_tx_counter;
    static uint32_t g_tx_end_counter;
    static uint32_t g_tx_line_conflict_counter;
    static uint32_t g_rx_line_conflict_counter;
    static void     processes_on_line	(void);
    static void     service_message_clear   (void);
    static int      check_console_service_command   (int c_cns);

    static callback_protocol_functions_t g_callback_protocol_functions;

};


#endif 	// SDK_USART_H
