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
//#include <string>
using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.

#include <ncurses.h>
#include "sdk_usart.h"

#include "sdk_process.h"


#define SDK_USART_SOURCE_SHM__RX_FREE		0
#define SDK_USART_SOURCE_SHM__RX_RUNNING	1

int sdk_usart::ch = 0;
int sdk_usart::g_c;
uint8_t sdk_usart::g_shm_status;
uint32_t sdk_usart::g_tx_counter = 0;
uint32_t sdk_usart::g_tx_end_counter = 0;
uint32_t sdk_usart::g_tx_line_conflict_counter = 0;
uint32_t sdk_usart::g_rx_line_conflict_counter = 0;


void sdk_usart::reg_write(void* p_ptrData, char c)	{

	if(sdk_process::write(c))	{
		// tx data is properly concluded
		// send data to local console
		addch(c);
		g_tx_counter++;
		service_message(3,"Write character [write() function] - ", c, g_tx_counter);
	} else {
		// there is a line-conflict
		g_tx_line_conflict_counter++;
		// send a service error message
		//mvprintw(3, 0, "Line conflict (tx procedure) - counter conflict: %04d", g_tx_line_conflict_counter);
		service_message(3,"Line conflict (tx procedure) - counter conflict: ", g_tx_line_conflict_counter, 0);
	}

}

uint8_t sdk_usart::reg_read(void* p_ptrData)	{
	return g_c;
}

void sdk_usart::tx_enable(void* p_ptrData)	{
}

void sdk_usart::rx_enable(void* p_ptrData)	{
}


bool sdk_usart::check_loop_end(void)	{
	return (ch == 'X');
}

// ******************************************************************************************
// ******************************************************************************************
void sdk_usart::service_message       (int row, char const *p_CharBufferTx, int p_par1, int p_par2)  {
	int x = getcurx(stdscr);
	int y = getcury(stdscr);

	mvprintw(row, 0, "%s - param1: %05d - param2: %05d", p_CharBufferTx, p_par1, p_par2);
	refresh();

	move(y,x);
}

// ******************************************************************************************
// ******************************************************************************************
void sdk_usart::service_message_clear      (void)  {
	int x = getcurx(stdscr);
	int y = getcury(stdscr);

	move(5,0);
	clrtoeol();
	move(6,0);
	clrtoeol();
	move(7,0);
	clrtoeol();
	move(8,0);
	clrtoeol();
	move(9,0);
	clrtoeol();

	refresh();

	move(y,x);
}


void sdk_usart::processes_on_line	(void)	{
	move(2,0);
	for(uint8_t i=0;i<PROCESS_MAX_NUMBER;i++){
		uint16_t l_pid;
		l_pid = sdk_process::get_pid(i);
		if(l_pid) printw("Pid %1d: %05d --- ", i, l_pid);
		else break;
	}

}

// SPECIFIC-THREAD function 

void sdk_usart::start(std::string p_param)	{

	// PROCESS MANAGEMENT
	bool l_test = sdk_process::add_0(p_param);

	// DISPLAY MANAGEMENT
	initscr();
	clear();
	noecho();
	cbreak();						// raw();	// Line buffering disabled. pass on everything. It does not need to press "enter" key to submit previous pressed key
	nodelay(stdscr, true);			// non-blocking input (getch) -> the same of timeout(0)
	keypad(stdscr, true);
	if(l_test) mvprintw(0, 0, "--- || ___ Ctrl-a, Ctrl-b or Ctrl-c to exit ___ || ---\n");
	else mvprintw(0, 0, "No process added\n");
	processes_on_line();
	move(10,0);
	curs_set(0);
	refresh();

}

void sdk_usart::end(void)	{

	// mvprintw(6, 0, "Test message 2 - exit status");
	// refresh();

	// DISPLAY MANAGEMENT
	clrtoeol();
	refresh();
	endwin();

	// PROCESS MANAGEMENT
	sdk_process::del_0();

}


bool sdk_usart::loop(void)	{
	int ymax = getmaxy(stdscr);
	int y;
	sleep_for(1ms);
	y = getcury(stdscr);
	if(y == (ymax-1)) move(10,0);

	sdk_process::start();

	//mvprintw(3, 0, "Test message 0");
	//refresh();
	
	// check the status of shared memory
	// 1. a new process has been added or must be added
	// 2. a registered processes has been deleted or must be deleted
	if(sdk_process::check_process())	{
		// update the processes list because it has been modified
		processes_on_line();
	}

	//mvprintw(4, 0, "Test message 1");
	//refresh();

	// ************************************************************************
	// ********** TX MANAGEMENT
	// ************************************************************************
	g_callback_protocol_functions.tx_buffer_management(NULL);
 	if(g_callback_protocol_functions.tx_status_get__completed()) {
		g_callback_protocol_functions.tx_status_set__free();
		sdk_process::write_end();
		g_tx_end_counter++;
		service_message(4,"Write character [write() function] - ", g_tx_end_counter, 0);
	}

	//mvprintw(5, 0, "Test message 2");
	//refresh();

	// ************************************************************************
	// ********** RX MANAGEMENT
	// ************************************************************************

	// read from console
	int c_cns = getch();
	// check for exit commnad
	if(6 == c_cns) {
		c_cns = ERR;
		if(g_callback_protocol_functions.rx_status_get__running()) {
			service_message(6,"rx status is RUNNING", 0, 0);
			g_callback_protocol_functions.rx_status_set__free();
		}
	}
	if(5 == c_cns) {
		c_cns = ERR;
		service_message_clear();
	}
	if(4 == c_cns) {
		c_cns = ERR;
		service_message(5,"Received Ctrl-d", 0, 0);
		// the shutdown procedure is started so rx must be stopped
		if(g_callback_protocol_functions.rx_status_get__running()) {
			service_message(6,"rx status is RUNNING", 0, 0);
		}
		if(g_callback_protocol_functions.rx_status_get__completed()) {
			service_message(7,"rx status is COMPLETED", 0, 0);
		}
		if(g_callback_protocol_functions.tx_status_get__running()) {
			service_message(8,"tx status is RUNNING", 0, 0);
		}
		if(g_callback_protocol_functions.rx_status_get__completed()) {
			service_message(9,"tx status is COMPLETED", 0, 0);
		}
	}

	if(1 == c_cns) {
		c_cns = ERR;
		switch (ch)	{
			case 'X':
				ch = 'Y';
				// the shutdown procedure is started so rx must be stopped
				if(g_callback_protocol_functions.rx_status_get__running()) {
					service_message(6,"rx status is RUNNING", 0, 0);
					g_callback_protocol_functions.rx_status_set__free();
				}
				break;
			case 'Y':
				return false;
				break;
			default:
				ch = 'X';
				break;
		}
		
	}

	// read from shared memory
	uint16_t c_shm = sdk_process::read();
	// check for data to read from shared memory
	if(SDK_PROCESS_STATUS__NO_DATA != c_shm)	{
		// data from another process is received
		g_shm_status = SDK_USART_SOURCE_SHM__RX_RUNNING;
		g_c = c_shm;
		g_callback_protocol_functions.rx_buffer_management(NULL);
	 	if(g_callback_protocol_functions.rx_status_get__completed()) {
			g_shm_status = SDK_USART_SOURCE_SHM__RX_FREE;
		}
	} else {
		// no data read from shared memory
		// therefore ...

		// check for data from console
		if(ERR != c_cns) {

			// the shared memory has absolute precedence
			if(SDK_USART_SOURCE_SHM__RX_FREE == g_shm_status)	{
				// no receving procedure is active on shared memory
				if(2 == c_cns) c_cns = 0;

				if(1 == c_cns) service_message(5,"ERROR - EXIT character is being processed - ", 0, 0);

				g_c = c_cns;
				g_callback_protocol_functions.rx_buffer_management(NULL);
			} else {
				// there is a line-conflict
				// a process sent data on shared memory and now a data has been received from local console
				g_rx_line_conflict_counter++;

				// send a service error message
				service_message(4,"Line conflict (rx procedure) - counter conflict - ", 0, 0);
			}
		}
	}

	sdk_process::next();

	return true;
	
}
