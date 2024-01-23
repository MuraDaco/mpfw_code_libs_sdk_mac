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

#include "sdk_process.h"
#include <iostream>


#define SHARED_OBJ_NAME         		"/mpfw_sdk_shared_mem"		// "/mpfw_sdk_process"


bool     sdk_process::g_start;
uint16_t sdk_process::g_pid;
int 								sdk_process::g_shmFd;
sdk_process::shared_mem_struct_t*	sdk_process::g_msg_ptr;


bool sdk_process::add_0(std::string p_param)	{
	bool l_result = false;

	if("r" == p_param)	{
		std::cout << "Performing ... shm_unlink(.) function" << std::endl;
		shm_unlink(SHARED_OBJ_NAME);
	} else {
		std::cout << "shm_unlink(.) function was not performed" << std::endl;
	}

	g_pid = getpid();

    g_shmFd = shm_open(SHARED_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(g_shmFd == -1) {
        printf("ERROR - opening shared memory\n");
    } else {
        ftruncate(g_shmFd, sizeof(shared_mem_struct_t));
        g_msg_ptr = (shared_mem_struct_t *) mmap(NULL, sizeof(shared_mem_struct_t), PROT_READ | PROT_WRITE, MAP_SHARED, g_shmFd, 0);

		// waiting for shared memory unlocked
	    while(g_msg_ptr->gLockProcess.test_and_set(std::memory_order_acquire))	{}

		if(0 == g_msg_ptr->gNew) {
			// now the resource is free & no other processes has asked to be added
			g_msg_ptr->gNew = g_pid;

			// check if the current process is the first one to be added
			if(0 == g_msg_ptr->gActiveProcessNum)	{
				// the current process is the first one
				// so no process is active
				add_2();
			}
			
			l_result = true;
		}
		// release the resources
		g_msg_ptr->gLockProcess.clear(std::memory_order_release);
	}

	return l_result;
}

void sdk_process::add_2(void)	{
	// a new process has to be added
	for(uint8_t i=0;i<PROCESS_MAX_NUMBER;i++) {
		if(g_msg_ptr->process_buffer[i].pid == 0)	{
			g_msg_ptr->process_buffer[i].pid = g_msg_ptr->gNew;

			g_msg_ptr->gNew = 0;
			g_msg_ptr->gActiveProcessNum = i+1;
			break;
		}
	}

	// check if the process that has just been added is the first one
	if(1 == g_msg_ptr->gActiveProcessNum)	{
		// the process that has just been added is the first one

		// enable the current process to manage the shared memory
		// it is possible to enable the current process here because it is the first 
		// otherwise it is not, it can be dangerous
		g_msg_ptr->gActivePid = g_pid;
	}
}

bool sdk_process::del_0(void)	{
	bool l_result = false;

	std::cout << "del_0" << std::endl;

    if(g_shmFd == -1) {
        printf("ERROR - opening shared memory\n");
    } else {

		// waiting for the current process became active/get the "token" 
		for(;;) {
			if(g_pid == g_msg_ptr->gActivePid)	{

				std::cout << "Last step [[-3]]" << std::endl;

				// waiting for shared memory became unlocked
			    if(!g_msg_ptr->gLockProcess.test_and_set(std::memory_order_acquire))	{
					std::cout << "Last step [[-2]]" << std::endl;
					break;
				} else {
					start();
					next();
				}
			}
		}

		// activate del procedure
		if(g_msg_ptr->gDel) {
			// delete another process that wants exit
			del_2();
		}

		std::cout << "Last step [[-1]]" << std::endl;

		// now the resource is free & no other processes has asked to be removed
		g_msg_ptr->gDel = g_pid;

		// check if the current process is the last one
		if(1 == g_msg_ptr->gActiveProcessNum)	{
			std::cout << "The current process is the last" << std::endl;
			// the current process is the last one
			// therefore it must perform 
			// the add procedure of new process if there is a request about it
			// it is ininfluent the order in which the addition and deleting procedure are performed

			// check if there is a new process to add
			if(g_msg_ptr->gNew) {
				// add new process
				add_2();
			}

			// auto-delete the current process
			del_2();
		}

		bool l_last_process = (0 == g_msg_ptr->gActiveProcessNum);

		// release the resources
		g_msg_ptr->gLockProcess.clear(std::memory_order_release);

		start();
		next();

		std::cout << "Last step [[0]]" << std::endl;

		munmap(g_msg_ptr, sizeof(shared_mem_struct_t));

		std::cout << "Last step [[1]]" << std::endl;

	    close(g_shmFd);

		std::cout << "Last step [[2]]" << std::endl;
			
		// remove shared mem object if the current removed process is the last one
		if(l_last_process) {
			std::cout << "Last step [[3]]" << std::endl;
			shm_unlink(SHARED_OBJ_NAME);
			std::cout << "No process is active -> performed the shm_unlink(.) function" << std::endl;
		}

		l_result = true;

	}

	return l_result;
}


void sdk_process::del_2	(void)	{
	uint8_t l_active_process_num = 0;
	// a process has to be removed
	for(uint8_t i=0;i<PROCESS_MAX_NUMBER;i++) {
		// std::cout << "Remove process id: " << (int) i << " -- pid: " << g_msg_ptr->gDel << std::endl;
		// search for pid process to remove
		if(g_msg_ptr->process_buffer[i].pid == g_msg_ptr->gDel)	{
			// pid process has been found
			// std::cout << "Removing process id: " << (int) i << " -- pid: " << g_msg_ptr->gDel << std::endl;

			// the number of active processes is at least equal to the id of the process to remove
			l_active_process_num = i;

			if((PROCESS_MAX_NUMBER-1) == i)	{
				// the process to remove is the younger and it has the maximum id (the process_buffer is full)
				// therefore it is not necessary to apply the shift procedure
				g_msg_ptr->process_buffer[i].pid   = 0;
				// std::cout << "Last step (1) to removing process id: " << (int) i << " -- pid: " << g_msg_ptr->gDel << std::endl;
				break;
			} else {
				// the process to remove is not the last process of the process_buffer
				// therefore the shift procedure must be applyed

				// start the procedure to shift by one position all the younger processes than the removed one
				for(uint8_t j=i+1;j<PROCESS_MAX_NUMBER;j++) {

					// check if the next process exists
					if(g_msg_ptr->process_buffer[j].pid)	{
						// a younger processes exist

						// increment the number of active processes
						l_active_process_num++;
						// shift the process
						g_msg_ptr->process_buffer[j-1].pid = g_msg_ptr->process_buffer[j].pid;

						g_msg_ptr->process_buffer[j].pid   = 0;
						if(PROCESS_MAX_NUMBER == (j+1))	{
							// before the current procedure the porcesses list was full
							// and this is the last step of the processes list shifting
							// std::cout << "Last step (2) to removing process id: " << (int) i << " -- pid: " << g_msg_ptr->gDel << std::endl;
						}
					} else {
						// no other processes exist
						// close the removing procedure

						// note: if "l_active_process_num" variable is 0 it means that the current process, that has been removed, is the last one
						g_msg_ptr->process_buffer[l_active_process_num].pid = 0;
						//  std::cout << "Last step (3) to removing process id: " << (int) i << " -- pid: " << g_msg_ptr->gDel << std::endl;
						// exit from loop
						break;
					}
				}
			}

			// check if the process that has just been removed was the active process
			// when this condition happens it means that the process was the last one
			if(g_msg_ptr->gDel == g_msg_ptr->gActivePid)	{
				// the removed process was the last one
				// but ...
				// if the removed process add a new process before deleting itself
				if(1 == l_active_process_num)	{
					// then the new process, that is also the new first process of the list, must become the active process
					g_msg_ptr->gActivePid = g_msg_ptr->process_buffer[0].pid;
				}
			}

			g_msg_ptr->gDel = 0;
			g_msg_ptr->gActiveProcessNum = l_active_process_num;

			// exit from loop
			break;
		}
	}

}


bool sdk_process::check_process(void)	{
	bool l_result = false;

	// check status of virtual file associate with shared memory
    if(g_shmFd != -1) {
		// virtual file has been open properly, the shared memory is available

		// check if the current process is active 
		// 1. to modify the processes list adding and/or removing a process
		// 2. to read   the processes list status
		if(
			(g_pid == g_msg_ptr->gActivePid) &&
			g_start
		)	{
			// it's own turn to modyfy o read the processes list

			// check the lock status of processes list modification requests
			// it can be locked by
			// a new process that want to be added
			// or a regsiterd process that want to be removed
		    if(!g_msg_ptr->gLockProcess.test_and_set(std::memory_order_acquire))
			{
				// the request of processes list modification is locked

				// check the request of adding a process
				if(g_msg_ptr->gNew) {
					add_2();
					// the list of processes must be read to update the visualization on console
					l_result = true;
					// advise the other processes that the processes list has been modified
					g_msg_ptr->gProcessWriterPid = g_pid;
				}

				// check the request of removing a process
				if(g_msg_ptr->gDel) {
					del_2();
					// the list of processes must be read to update the visualization on console
					l_result = true;
					// advise the other processes that the processes list has been modified
					g_msg_ptr->gProcessWriterPid = g_pid;
				}

				// release the resources
				g_msg_ptr->gLockProcess.clear(std::memory_order_release);

			}
				
			// check process buffer status
			if(g_msg_ptr->gProcessWriterPid)	{
				// one process modified processes list adding or removing a process

				// check if the current process, now (through the previuos statement), has just added or removed a process
				if(!l_result)	{
					// no process has been added or removed by the current process

					// check 
					if(g_pid == g_msg_ptr->gProcessWriterPid)	{
						// the current process has modified process buffer not now but in a previous cycle
						// all processes have read the new list of processes
						// therefore the procedure is terminated, reset the associated variable
						g_msg_ptr->gProcessWriterPid = 0;
					} else {
						// the current process has not modified the processes list
						// therefore ...
						// the list of processes must be read to update the visualization on console
						l_result = true;
					}
				}
			}
		}
	}

	return l_result;
}


bool     sdk_process::g_all_read_start;
bool     sdk_process::g_tx_data_new;
uint16_t sdk_process::g_tx_data;
uint16_t sdk_process::g_rx_data;
uint8_t  sdk_process::g_tx_error;
uint8_t  sdk_process::g_reader_counter[2];

bool sdk_process::rx_fifo_full        (void)	{
	return !(SDK_PROCESS_STATUS__RX_NO_DATA == g_rx_data);
}

bool sdk_process::tx_fifo_full   (void)	{
	return (g_tx_data_new);
}

void sdk_process::tx_fifo_empty_out   (void)	{
	g_tx_data_new = false;
}

void sdk_process::tx_fifo_write  (char c)	{
	g_tx_data = c;
	g_tx_data_new = true;
}

uint16_t sdk_process::rx_fifo_read  (void)	{
	return g_rx_data;
}


uint16_t sdk_process::read(void)	{
	return g_rx_data;
}

uint8_t sdk_process::error_get(void)	{
	return g_tx_error;
}

uint8_t sdk_process::reader_counter_get(uint8_t p_id)	{
	return g_reader_counter[p_id];
}

void sdk_process::reader_counter_clr   (void)	{
	g_reader_counter[0] = g_reader_counter[1] = 0;
}


bool sdk_process::check_read_write(void)	{
	bool l_result = false;
	g_rx_data = SDK_PROCESS_STATUS__RX_NO_DATA;

    if(g_shmFd != -1) {

		// waiting for acquire the token
		if(
			(g_pid == g_msg_ptr->gActivePid) &&
			g_start
		)	{
			// check data buffer status
			if(g_msg_ptr->gDataWriterPid)	{
				// write/read procedure is on
				if(g_pid == g_msg_ptr->gDataWriterPid)	{
					// the current process is the process that write/add data to packet buffer
					// all processes have read the data
					// therefore ...
					
					if(tx_fifo_full())	{
						// write data
						g_msg_ptr->buffer[0] = g_tx_data;
						// empty the fifo
						tx_fifo_empty_out();
					} else {
						// release the channel
					 	g_msg_ptr->gDataWriterPid = 0;
					}

				} else {
					// the current process is not the process that add data to packet buffer
					// therefore ...

					// check if there is a line conflict
					// the line conflict happen when the current process want to send data but 
					// another process has already locked the channel
					if(tx_fifo_full())	{
						// there is a data to send
						// therefore there is a line conflict
						l_result = true;

						// empty the fifo
						tx_fifo_empty_out();
					} else {
						// there is a data to read
						g_rx_data = g_msg_ptr->buffer[0];
					}

				}
			} else {
				// the channel is free
				// check if there is a data to send
				if(tx_fifo_full())	{
					// there is a data to send

					// get/lock the channel
					g_msg_ptr->gDataWriterPid = g_pid;
					// write data
					g_msg_ptr->buffer[0] = g_tx_data;
					// empty the fifo
					tx_fifo_empty_out();
				}
			}
		}
	}

	return l_result;
}


bool sdk_process::check_start(void)	{
	return ((g_pid == g_msg_ptr->gActivePid) && g_start);
}

void sdk_process::start(void)	{
	// scope explanation of the folowing statement
	// 1. (Introduction)  - you don't know where the thread start to work after a context switch of the os scheduler
	// 2. (Possible case) - in the worst case the thread start to work just from this function (the "sdk_process::next" function)
	// 3. (Consequences)  - if the following statement does not exist then the current thread loses a cycle to use the shared memory
	// 4. (Possible case) - furthermore, this case, in the worst and worst situation, can iterate every context swtich ...
	// 5. (Consequences)  - ... preventing to use/lock the shared memory from the current thread
	// 6. (Solution)      - By the following statement you are sure that an entire loop with the condition "shared memroy available" has been performed
	if(g_pid == g_msg_ptr->gActivePid)	{
		// it's own turn to modyfy o read the processes list
		g_start = true;
	}
}

//void __attribute__((optimize("O0"))) sdk_process::next(void)	{
void sdk_process::next(void)	{
	uint8_t l_next_id;
	// check status of virtual file associate with shared memory
    if(g_shmFd != -1) {
		// virtual file has been open properly, the shared memory is available

		// check if the current process is active to select the next process
		if(g_pid == g_msg_ptr->gActivePid)	{
			// it's own turn to modyfy o read the processes list

			if(g_start)	{
				g_start = false;
				for(uint8_t i=0;i<PROCESS_MAX_NUMBER;i++) {
					if(g_msg_ptr->process_buffer[i].pid == g_pid)	{
						l_next_id = i+1;

						// check if the next id is right
						if(PROCESS_MAX_NUMBER == l_next_id) l_next_id = 0;
						else {
							if(0 == g_msg_ptr->process_buffer[l_next_id].pid) l_next_id = 0;
						}

						// update the active pid
						g_msg_ptr->gActivePid = g_msg_ptr->process_buffer[l_next_id].pid;
					}
				}
			} else {
				g_start = true;
				g_tx_error++;
			}
		}

		// scope explanation of the folowing statement
		// 1. (Introduction)  - you don't know where the thread start to work after a context switch of the os scheduler
		// 2. (Possible case) - in the worst case the thread start to work just from this function (the "sdk_process::next" function)
		// 3. (Consequences)  - if the following statement does not exist then the current thread loses a cycle to use the shared memory
		// 4. (Possible case) - furthermore, this case, in the worst and worst situation, can iterate every context swtich ...
		// 5. (Consequences)  - ... preventing to use/lock the shared memory from the current thread
		// 6. (Solution)      - By the following statement you are sure that an entire loop with the condition "shared memroy available" has been performed
		//if(g_pid == g_msg_ptr->gActivePid)	{
		//	// it's own turn to modyfy o read the processes list
		//	g_start = true;
		//}
	}
}


uint16_t sdk_process::get_pid(uint8_t p_id)	{
	if(p_id < PROCESS_MAX_NUMBER) return g_msg_ptr->process_buffer[p_id].pid;
	else return 0;
}


uint16_t sdk_process::get_pid_enabled(void)	{
	return g_msg_ptr->gActivePid;
}


