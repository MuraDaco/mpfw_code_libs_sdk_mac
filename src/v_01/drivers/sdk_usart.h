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

#include <ncurses.h>

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.



#ifndef SDK_USART_H
#define SDK_USART_H

class sdk_usart {

    public:

    typedef uint8_t (*callback_protocol_get_packet_length_t)  (uint8_t p_rx_byte);
    typedef uint8_t (*callback_protocol_get_status_t)         (void);
    typedef bool    (*callback_protocol_check_packet_end_t)   (uint8_t p_rx_byte);
    typedef void    (*callback_protocol_set_buffer_size_t)    (uint8_t p_size_buffer);

    typedef struct _callback_protocol_functions {
        callback_protocol_get_packet_length_t  callback_get_packet_length;
        callback_protocol_get_status_t         callback_get_status;
        callback_protocol_check_packet_end_t   callback_check_packet_end;
        callback_protocol_set_buffer_size_t    callback_set_buffer_size;
    } callback_protocol_functions_t;


    typedef struct _usart_handle_mod
    {
        uint8_t *volatile ptrBuffer;            // rxData;            /*!< Address of remaining data to receive. */
        size_t BufferSize;                      // NEW       
        uint8_t *volatile ptrBufferDefault;     // rxData;            /*!< Address of remaining data to receive. */
        size_t BufferSizeDefault;               // NEW       
        volatile size_t DataCounter;            // rxDataSize;        /*!< Size of the remaining data to receive. */
        size_t PacketLength;                    // rxDataSizeAll;     /*!< Size of the data to receive. */
        volatile uint8_t State;                 //                    /*!< RX transfer state */

    } usart_handle_mod_t;


    static uint8_t send_non_blocking         (uint8_t* ptr_buffer);
    static uint8_t send_non_blocking         (uint8_t* ptr_buffer, uint8_t p_size_buffer);
    static uint8_t send_non_blocking         (char const *ptr_buffer);
    static void    send_blocking             (uint8_t* ptr_buffer);
    static void    send_blocking             (uint8_t* ptr_buffer, uint8_t p_size_buffer);
    static void    send_blocking             (char const *ptr_buffer);
    static void    receive_non_blocking      (uint8_t p_packet_length);
    static void    receive_non_blocking      (char*    ptr_buffer, uint8_t p_size_buffer);
    static void    receive_non_blocking      (uint8_t* ptr_buffer, uint8_t p_size_buffer);
    static void    receive_non_blocking      (uint8_t* ptr_buffer, uint8_t p_size_buffer, uint8_t p_packet_length);
    static bool    receive_free              (void);
    static bool    receive_check             (void);
    static bool    check_loop_end            (void);
    static void    start                     (void);
    static bool    loop                      (void);
    static void    end                       (void);
    static void    init                      (uint8_t* p_ptrBuffer, uint8_t p_size_buffer, callback_protocol_functions_t* p_ptr_callback_protocol_functions);

    static void    buffer_management         (int c);
    static void    buffer_management_end     (int c);
    static bool    status_free               (void);


    private:

    static int ch;

    static int startx;
    static int starty;

    static usart_handle_mod_t g_handle;
    static callback_protocol_functions_t* g_ptr_callback_protocol_functions;

};


#endif 	// SDK_USART_H
