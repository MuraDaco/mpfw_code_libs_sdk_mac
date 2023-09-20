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



int sdk_usart::ch = 'a';
int sdk_usart::g_c;



void sdk_usart::reg_write(void* p_ptrData, char c)	{
	addch(c);
}

uint8_t sdk_usart::reg_read(void* p_ptrData)	{
	return g_c;
}

void sdk_usart::tx_enable(void* p_ptrData)	{
}

void sdk_usart::rx_enable(void* p_ptrData)	{
}

// ALL-THREAD function 

void sdk_usart::tx_disable(void* p_ptrData, char c)	{

	addch(c);

}

void sdk_usart::rx_disable(void* p_ptrData, char c)	{

}


bool sdk_usart::check_loop_end(void)	{
	return (ch == 'X');
}




// SPECIFIC-THREAD function 

void sdk_usart::start(void)	{
	initscr();
	clear();
	noecho();
	cbreak();	// raw();					// Line buffering disabled. pass on everything. It does not need to press "enter" key to submit previous pressed key
	nodelay(stdscr, true);	// non-blocking input (getch) -> the same of timeout(0)
	keypad(stdscr, true);
	mvprintw(0, 0, "--- || ___ Ctrl-a, Ctrl-b or Ctrl-c to exit ___ || ---");
	move(2,0);
	curs_set(0);
	refresh();
}

void sdk_usart::end(void)	{
	clrtoeol();
	
	refresh();
	endwin();

}


bool sdk_usart::loop(void)	{
	int ymax = getmaxy(stdscr);
	int y;
	int c;
	sleep_for(10ms);
	y = getcury(stdscr);
	if(y == (ymax-1)) move(2,0);
		
	g_callback_protocol_functions.tx_buffer_management(NULL);
	// if(g_callback_protocol_functions.tx_status_get_running()) {
	// 	c = g_callback_protocol_functions.buffer_read();	// *(g_handle.ptrBuffer);
	// 	g_callback_protocol_functions.buffer_management(NULL,c);
	// 	if(g_callback_protocol_functions.tx_status_get_running()) addch(c);
	// }
	
 	if(g_callback_protocol_functions.tx_status_get__completed()) {
		g_callback_protocol_functions.tx_status_set__free();		// g_handle.State = SDK_USART_TX_STATUS__FREE;
	}

	c = getch();
	if(c != ERR) {
		if(c == 1) {
			if(ch == 'X') {
				return false;
			}
			ch = 'X';
		}
		if(c == 2) c = 0;

		g_c = c;
		g_callback_protocol_functions.rx_buffer_management(NULL);
		// if(g_callback_protocol_functions.rx_status_get_running()) {
		// 	g_callback_protocol_functions.buffer_write(c); // *(g_handle.ptrBuffer) = c;
		// 	g_callback_protocol_functions.buffer_management(NULL,c);
		// }
	}

	if(ch == 'X') {
		// if(SDK_USART_RX_STATUS__RUNNING == g_handle.State) g_handle.State = SDK_USART_RX_STATUS__FREE;
		if(g_callback_protocol_functions.rx_status_get__running()) {
			g_callback_protocol_functions.rx_status_set__free(); // g_handle.State = SDK_USART_RX_STATUS__FREE;
		}
	}

	return true;
	
}

