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
#include <string>
#include <atomic>
#include <cstdint>
#include <unistd.h> // for getpid()


#include <sys/mman.h>   // for shared memory created
#include <sys/stat.h>   // for mode constants
#include <fcntl.h>      // for O_* constant



// #include <cstddef>

#ifndef SDK_PROCESS_H
#define SDK_PROCESS_H

#define MESSAGE_BUFFER_SIZE     50
#define PROCESS_MAX_NUMBER	    4

#define SDK_PROCESS_STATUS__RX_NO_DATA         0xfff0

class sdk_process {

    public:



    static bool     add_0   (std::string p_param);
    static bool     del_0   (void);
    static void     add_2   (void);
    static void     del_2   (void);
    static bool     check_process       (void);
    static bool     check               (void);

    static uint16_t read        (void);
    static void     start       (void);
    static void     next        (void);
    static bool     check_start (void);


    static uint16_t get_pid             (uint8_t p_id);
    static uint16_t get_pid_enabled     (void);

    static bool tx_fifo_full        (void);
    static void tx_fifo_empty_out   (void);
    static void tx_fifo_write       (char c);
    static uint16_t rx_fifo_read    (void);
    static bool rx_fifo_full        (void);
    static bool check_read_write    (void);

    static uint8_t error_get            (void);
    static uint8_t reader_counter_get   (uint8_t p_id);
    static void    reader_counter_clr   (void);


    private:


    typedef struct _process_struct
    {
        uint16_t pid;
        uint8_t  status;
        // bool     rts;
    } process_struct_t;

    typedef struct _shared_mem_struct
    {
        int gNew;
        int gDel;
        int pid;
        int gActivePid;
        uint8_t gActiveProcessNum;
        char buffer[MESSAGE_BUFFER_SIZE];
        uint8_t reader_counter;
    	process_struct_t process_buffer[PROCESS_MAX_NUMBER];
        std::atomic_flag gLockProcess;

        int gProcessWriterPid;
        int gDataWriterPid;
    } shared_mem_struct_t;

    static int g_shmFd;
    static shared_mem_struct_t* g_msg_ptr;
    static bool     g_start;
    static uint16_t g_pid;
    static uint16_t g_process_buff[];

    static bool g_all_read_start;
    static bool g_tx_data_new;
    static uint16_t g_tx_data;
    static uint16_t g_rx_data;
    static uint8_t  g_tx_error;
    static uint8_t  g_reader_counter[2];



};


#endif 	// SDK_PROCESS_H
