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

#define SDK_USART_DISPLAY__START_CONSOLE	10

#define SDK_USART_SERVICE_MSG__HEADER					0
#define SDK_USART_SERVICE_MSG__RX_LINE_CONFLICT			1
#define SDK_USART_SERVICE_MSG__TX_LINE_CONFLICT			2
#define SDK_USART_SERVICE_MSG__PROCESS_LIST				3
#define SDK_USART_SERVICE_MSG__PACKET_END				4

#define SDK_USART_SERVICE_MSG__RX_RESET					6
#define SDK_USART_SERVICE_MSG__EMPTY_COMMAND			7

#define SDK_USART_SERVICE_MSG__EXIT_STEP_1				7
#define SDK_USART_SERVICE_MSG__EXIT_STEP_2				8
#define SDK_USART_SERVICE_MSG__EXIT_STEP_3				9

#define SDK_USART_SERVICE_MSG__COMMAND_RECEIVED 		5
#define SDK_USART_SERVICE_MSG__RX_STATUS_RUNNING		6
#define SDK_USART_SERVICE_MSG__RX_STATUS_COMPLETED		7
#define SDK_USART_SERVICE_MSG__TX_STATUS_RUNNING		8
#define SDK_USART_SERVICE_MSG__TX_STATUS_COMPLETED		9

#define SDK_USART_SERVICE_MSG__READER_COUNTER_CLR		6
#define SDK_USART_SERVICE_MSG__READER_COUNTER_1			7
#define SDK_USART_SERVICE_MSG__READER_COUNTER_2			8
#define SDK_USART_SERVICE_MSG__TX_ERROR					9

#define SDK_USART_SERVICE_MSG__DEBUG_TEST_0				5
#define SDK_USART_SERVICE_MSG__DEBUG_TEST_1				6
#define SDK_USART_SERVICE_MSG__DEBUG_TEST_2				7
#define SDK_USART_SERVICE_MSG__DEBUG_TEST_3				8
#define SDK_USART_SERVICE_MSG__DEBUG_TEST_4				9

int sdk_usart::ch_0 = 0;
int sdk_usart::ch = 0;
int sdk_usart::g_c;
int sdk_usart::g_tx_c;
bool sdk_usart::g_shamem_fifo_full = false;
uint8_t sdk_usart::g_shm_status;
uint32_t sdk_usart::g_tx_counter = 0;
uint32_t sdk_usart::g_tx_end_counter = 0;
uint32_t sdk_usart::g_tx_line_conflict_counter = 0;
uint32_t sdk_usart::g_rx_line_conflict_counter = 0;

uint16_t sdk_usart::g_start_counter = 0;
uint16_t sdk_usart::g_start_counter_max = 0;
bool sdk_usart::g_rx_stop = false;

bool sdk_usart::g_test_1 = false;
bool sdk_usart::g_test_2 = false;
int sdk_usart::g_rx_cns_data;


void sdk_usart::reg_write(void* p_ptrData, char c)	{

	sdk_process::tx_fifo_write(c);
	addch(c);
}

uint8_t sdk_usart::reg_read(void* p_ptrData)	{
	int l_data = g_rx_cns_data;
	if(sdk_process::rx_fifo_full())	{
		l_data = sdk_process::rx_fifo_read();
		//addch(l_data);
	}

	return l_data;
}

void sdk_usart::tx_enable(void* p_ptrData)	{
}

void sdk_usart::rx_enable(void* p_ptrData)	{
}


bool sdk_usart::check_loop_end(void)	{
	return (ch == 'X');
}

void sdk_usart::rx_stop      (void)		{
	g_rx_stop = true;
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


	move(SDK_USART_SERVICE_MSG__COMMAND_RECEIVED,0);
	clrtoeol();
	move(SDK_USART_SERVICE_MSG__RX_STATUS_RUNNING,0);
	clrtoeol();
	move(SDK_USART_SERVICE_MSG__RX_STATUS_COMPLETED,0);
	clrtoeol();
	move(SDK_USART_SERVICE_MSG__TX_STATUS_RUNNING,0);
	clrtoeol();
	move(SDK_USART_SERVICE_MSG__TX_STATUS_COMPLETED,0);
	clrtoeol();

	refresh();

	move(y,x);
}


void sdk_usart::processes_on_line	(void)	{
	int x = getcurx(stdscr);
	int y = getcury(stdscr);

	move(SDK_USART_SERVICE_MSG__PROCESS_LIST,0);
	for(uint8_t i=0;i<PROCESS_MAX_NUMBER;i++){
		uint16_t l_pid;
		l_pid = sdk_process::get_pid(i);
		printw("Pid %1d: %05d --- ", i, l_pid);
	}

	refresh();
	move(y,x);
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
	if(l_test) mvprintw(SDK_USART_SERVICE_MSG__HEADER, 	0, "--- || ___ Ctrl-a, Ctrl-b or Ctrl-c to exit ___ || ---\n");
	else mvprintw(SDK_USART_SERVICE_MSG__HEADER, 		0, "No process added\n");
	processes_on_line();
	move(SDK_USART_DISPLAY__START_CONSOLE,0);
	curs_set(0);
	refresh();

}

void sdk_usart::end_0(void)	{
	ch_0 = 'X';
}

void sdk_usart::end(void)	{

	// DISPLAY MANAGEMENT
	clrtoeol();
	refresh();
	endwin();

	// PROCESS MANAGEMENT
	sdk_process::del_0();

}


int sdk_usart::check_console_service_command(int c_cns)	{
// check for exit commnad

	// --- 'f' ---
	if(6 == c_cns) {
		c_cns = ERR;
		service_message(SDK_USART_SERVICE_MSG__DEBUG_TEST_0,"Step_0 --- Set g_test_1", 0, 0);
		g_test_1 = true;
		ch = 0;
	}


	// // --- 'g' ---
	// if(7 == c_cns) {
	// 	c_cns = ERR;
	// 	service_message(SDK_USART_SERVICE_MSG__READER_COUNTER_1,"Reader counter 1 --- ", sdk_process::reader_counter_get(0), 0);
	// 	service_message(SDK_USART_SERVICE_MSG__READER_COUNTER_2,"Reader counter 2 --- ", sdk_process::reader_counter_get(1), 0);
	// 	service_message(SDK_USART_SERVICE_MSG__TX_ERROR,"Error on Reader counter --- ", sdk_process::error_get(), 0);
	// }
	// 
	// // --- 'f' ---
	// if(6 == c_cns) {
	// 	c_cns = ERR;
	// 	sdk_process::reader_counter_clr();
	// 	service_message(SDK_USART_SERVICE_MSG__READER_COUNTER_CLR,"Reset reader counter", 0, 0);
	// }

	// --- 'g' ---
	if(7 == c_cns) {
		c_cns = ERR;
		// reset the rx status
		if(g_callback_protocol_functions.rx_status_get__running()) {
			service_message(SDK_USART_SERVICE_MSG__RX_RESET,"RX - Performing reset", 0, 0);
			g_callback_protocol_functions.rx_status_set__free();
		}
	}
//
//	// --- 'f' ---
//	if(6 == c_cns) {
//		c_cns = ERR;
//		service_message(SDK_USART_SERVICE_MSG__EMPTY_COMMAND,"Exit code resetting ...", ch, ch);
//		ch = 0;
//	}

	// --- 'e' ---
	if(5 == c_cns) {
		c_cns = ERR;
		service_message_clear();
	}

	// --- 'd' ---
	if(4 == c_cns) {
		c_cns = ERR;
		service_message(SDK_USART_SERVICE_MSG__COMMAND_RECEIVED,        "Received Ctrl-d",			0, 0);
		// the shutdown procedure is started so rx must be stopped
		if(g_callback_protocol_functions.rx_status_get__running()) {
			service_message(SDK_USART_SERVICE_MSG__RX_STATUS_RUNNING,  	"rx status is RUNNING",		0, 0);
		}
		if(g_callback_protocol_functions.rx_status_get__completed()) {
			service_message(SDK_USART_SERVICE_MSG__RX_STATUS_COMPLETED,	"rx status is COMPLETED",	0, 0);
		}
		if(g_callback_protocol_functions.tx_status_get__running()) {
			service_message(SDK_USART_SERVICE_MSG__TX_STATUS_RUNNING,  	"tx status is RUNNING",		0, 0);
		}
		if(g_callback_protocol_functions.rx_status_get__completed()) {
			service_message(SDK_USART_SERVICE_MSG__TX_STATUS_COMPLETED,	"tx status is COMPLETED",	0, 0);
		}
	}

	// --- 'b' ---
	if(2 == c_cns) c_cns = 0;

	// --- 'a' ---
	if(1 == c_cns) {
		c_cns = ERR;
		switch (ch)	{
			case 'X':
				ch = 'Y';
				// the shutdown procedure is started so rx must be stopped
				service_message(SDK_USART_SERVICE_MSG__EXIT_STEP_2,"Exit step 2", 0, 0);
				if(g_callback_protocol_functions.rx_status_get__running()) {
					service_message(SDK_USART_SERVICE_MSG__RX_STATUS_RUNNING,"rx status is RUNNING", 0, 0);
					g_callback_protocol_functions.rx_status_set__free();
				}
				break;
			case 'Y':
				ch = 'Z';
				service_message(SDK_USART_SERVICE_MSG__EXIT_STEP_3,"Exit step 3", 0, 0);
				break;
			default:
				ch = 'X';
				service_message(SDK_USART_SERVICE_MSG__EXIT_STEP_1,"Exit step 1", 0, 0);
				break;
		}
		
	}

	return c_cns;
}

bool sdk_usart::loop(void)	{
	int ymax = getmaxy(stdscr);
	int y;
	sleep_for(1ms);
	y = getcury(stdscr);
	if(y == (ymax-1)) move(SDK_USART_DISPLAY__START_CONSOLE,0);

	// ************************************************************************
	// ********** RX MANAGEMENT (console)
	// ************************************************************************

	// read from console
	g_rx_cns_data = getch();
	g_rx_cns_data = check_console_service_command(g_rx_cns_data);




	// ************************************************************************
	// ********** TX MANAGEMENT
	// ************************************************************************
	if(!sdk_process::tx_fifo_full())	{
		if(g_test_2) {
			service_message(SDK_USART_SERVICE_MSG__DEBUG_TEST_3,"Step_3 --- Performing tx_buffer management", 0, 0);
			if(g_callback_protocol_functions.tx_status_get__running())	{
				g_test_2 = false;
				service_message(SDK_USART_SERVICE_MSG__DEBUG_TEST_4,"Step_4 --- Next statement is tx buffer management", 0, 0);
				g_callback_protocol_functions.tx_buffer_management(NULL);
			}
		} else {
			g_callback_protocol_functions.tx_buffer_management(NULL);
		}
	 	if(g_callback_protocol_functions.tx_status_get__completed()) {
			g_callback_protocol_functions.tx_status_set__free();
			g_tx_end_counter++;
			service_message(SDK_USART_SERVICE_MSG__PACKET_END,"End tx - a packet has been sent: ", g_tx_end_counter, 0);
		}
	}

	// check the status of shared memory
	// 1. a new process has been added or must be added
	// 2. a registered processes has been deleted or must be deleted
	if(sdk_process::check_process())	{
		// update the processes list because it has been modified
		processes_on_line();
	}

	if(sdk_process::check_read_write())	{
		// there is a line-conflict
		g_tx_line_conflict_counter++;
		// send a service error message
		service_message(SDK_USART_SERVICE_MSG__TX_LINE_CONFLICT,"Line conflict (tx procedure) - counter conflict: ", g_tx_line_conflict_counter, 0);
		// set communication status to rx running
		g_callback_protocol_functions.tx_status_set__completed();
	}
	
	// if(sdk_process::error_get())	{
	// 	service_message(SDK_USART_SERVICE_MSG__TX_ERROR,"Error on Reader counter --- ", sdk_process::error_get(), 0);
	// 	service_message(SDK_USART_SERVICE_MSG__READER_COUNTER_1,"Reader counter 1 --- ", sdk_process::reader_counter_get(0), 0);
	// 	service_message(SDK_USART_SERVICE_MSG__READER_COUNTER_2,"Reader counter 2 --- ", sdk_process::reader_counter_get(1), 0);
	// 	
	// }
	
	// ************************************************************************
	// ********** RX MANAGEMENT (console)
	// ************************************************************************
	// === data read management ====
	if(g_rx_stop) {
		service_message(SDK_USART_SERVICE_MSG__RX_RESET,"RX - Performing reset", 0, 0);
		g_callback_protocol_functions.rx_status_set__free();
		g_rx_stop = false;
	}
	
	if(sdk_process::rx_fifo_full())	{
		int l_data = sdk_process::rx_fifo_read();
		addch(l_data);
	}

	if(
		(sdk_process::rx_fifo_full()) ||
		(ERR != g_rx_cns_data)
	)	{
		// check that there is no line conflict
		if(
			(sdk_process::rx_fifo_full()) &&
			(ERR != g_rx_cns_data)
		)	{
			// there is line conflict
			// a process sent data on shared memory and now a data has been received from local console
			g_rx_line_conflict_counter++;
			// send a service error message
			service_message(SDK_USART_SERVICE_MSG__RX_LINE_CONFLICT,"Line conflict (rx procedure) - counter conflict - ", g_rx_line_conflict_counter, 0);
		}

		if(g_test_1) {
			if('|' == g_rx_cns_data) {
				g_test_1 = false;
				g_test_2 = true;
				service_message(SDK_USART_SERVICE_MSG__DEBUG_TEST_1,"Step_1 --- Set g_test_2", g_rx_cns_data, g_rx_cns_data);
			}
		}

		g_callback_protocol_functions.rx_buffer_management(NULL);

		if(g_test_2) {
			service_message(SDK_USART_SERVICE_MSG__DEBUG_TEST_2,"Step_2 --- After rx_management", g_rx_cns_data, g_rx_cns_data);
		}
	}

	//if(sdk_process::check_start())	{
	//	g_start_counter++;
	//	if(g_start_counter > 1)	{
	//		if(g_start_counter_max < g_start_counter) g_start_counter_max = g_start_counter;
	//		service_message(SDK_USART_SERVICE_MSG__RX_LINE_CONFLICT,"Start fail happend: counter ", g_start_counter_max, 0);
	//	} else {
	//		service_message(SDK_USART_SERVICE_MSG__TX_LINE_CONFLICT,"Start OK - ", 1, 0);
	//	}	
	//} else {
	//	g_start_counter = 0;
	//	g_start_counter_max = 0;
	//}

	sdk_process::next();

	if('Z' == ch) {
		if('X' == ch_0)	return false;
	}
	refresh();
	return true;
	
}
