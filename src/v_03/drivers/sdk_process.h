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


#include <sys/mman.h> // for shared memory created
#include <sys/stat.h> // for mode constants
#include <fcntl.h> // for O_* constant

// #include <cstddef>

#ifndef SDK_PROCESS_H
#define SDK_PROCESS_H

#define MESSAGE_BUFFER_SIZE     50
#define PROCESS_MAX_NUMBER	    4

#define SDK_PROCESS_STATUS__NO_DATA         0xfff0

class sdk_process {

    public:



    static bool     add_0   (std::string p_param);
    static bool     del_0   (void);
    static void     add_2   (void);
    static void     del_2   (void);
    static bool     check_process       (void);
    static bool     check               (void);

    static uint16_t read        (void);
    static bool     write       (char c);
    static void     write_end   (void);
    static void     start       (void);
    static void     next        (void);


    static uint16_t get_pid             (uint8_t p_id);
    static uint16_t get_pid_enabled     (void);


    private:


    typedef struct _process_struct
    {
        uint16_t pid;
        uint8_t  status;
    } process_struct_t;

    typedef struct _shared_mem_struct
    {
        int gNew;
        int gDel;
        int pid;
        int gActivePid;
        uint8_t gActiveProcessNum;
        char buffer[MESSAGE_BUFFER_SIZE];
    	process_struct_t process_buffer[PROCESS_MAX_NUMBER];
        // std::atomic_flag gLock;
        std::atomic_flag gLockProcess;

        int gProcessWriterPid;
        int gDataWriterPid;
    } shared_mem_struct_t;

    static int g_shmFd;
    static shared_mem_struct_t* g_msg_ptr;
    static bool     g_start;
    static uint16_t g_pid;
    static uint16_t g_process_buff[];

};


#endif 	// SDK_PROCESS_H
