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
#include <thread>
#include <iostream>
#include <unistd.h>

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
    } callback_protocol_functions_t;


    // typedef struct _usart_handle_mod
    // {
    //     uint8_t *volatile ptrBuffer;            // rxData;            /*!< Address of remaining data to receive. */
    //     size_t BufferSize;                      // NEW       
    //     uint8_t *volatile ptrBufferDefault;     // rxData;            /*!< Address of remaining data to receive. */
    //     size_t BufferSizeDefault;               // NEW       
    //     volatile size_t DataCounter;            // rxDataSize;        /*!< Size of the remaining data to receive. */
    //     size_t PacketLength;                    // rxDataSizeAll;     /*!< Size of the data to receive. */
    //     volatile uint8_t State;                 //                    /*!< RX transfer state */
    // } usart_handle_mod_t;


    //static uint8_t send_non_blocking         (uint8_t* ptr_buffer);
    //static uint8_t send_non_blocking         (uint8_t* ptr_buffer, uint8_t p_size_buffer);
    //static uint8_t send_non_blocking         (char const *ptr_buffer);
    //static void    send_blocking             (uint8_t* ptr_buffer);
    //static void    send_blocking             (char const *ptr_buffer);
    //static void    send_blocking             (uint8_t* ptr_buffer, uint8_t p_size_buffer);
    //static void    send_blocking             (uint8_t* p_ptrBuffer, uint8_t p_size_buffer, uint8_t p_length);
    //static void    receive_non_blocking      (uint8_t p_packet_length);
    //static void    receive_non_blocking      (char*    ptr_buffer, uint8_t p_size_buffer);
    //static void    receive_non_blocking      (uint8_t* ptr_buffer, uint8_t p_size_buffer);
    //static void    receive_non_blocking      (uint8_t* ptr_buffer, uint8_t p_size_buffer, uint8_t p_packet_length);
    //static bool    receive_free              (void);
    //static bool    receive_check             (void);

    //static void    buffer_management         (int c);
    //static bool    status_free               (void);

    // callback protocol functions
    static void     reg_write               (void* p_ptrData, char c);
    static uint8_t  reg_read                (void* p_ptrData);
    static void     tx_enable               (void* p_ptrData);
    static void     rx_enable               (void* p_ptrData);
    static void     tx_disable              (void* p_ptrData, char c);
    static void     rx_disable              (void* p_ptrData, char c);


    // interrupt equivalent function
    static void    start                    (void);
    static bool    loop                     (void);
    static void    end                      (void);
    static bool    check_loop_end           (void);

    private:


    static int ch;
    static int g_c;
    static int startx;
    static int starty;

    static callback_protocol_functions_t g_callback_protocol_functions;

};


#endif 	// SDK_USART_H
