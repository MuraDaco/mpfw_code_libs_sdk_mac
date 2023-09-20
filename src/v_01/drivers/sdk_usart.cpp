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


//size_t niters;


#define SDK_USART_DEFAULT_BUFFER_SIZE          99
#define SDK_USART_DEFAULT_TERMINATED_CHAR		0	

#define SDK_USART_TX_STATUS__FREE               0 // 
#define SDK_USART_TX_STATUS__RUNNING	        1 // kUSART_TxBusy
#define SDK_USART_TX_STATUS__SEND_COMPLETED     2 // kUSART_TxIdle
#define SDK_USART_RX_STATUS__FREE               0
#define SDK_USART_RX_STATUS__RUNNING            3 // kUSART_RxBusy
#define SDK_USART_RX_STATUS__READY_TO_READ      4 // kUSART_RxIdle

int sdk_usart::ch = 'a';

int sdk_usart::startx = 0;
int sdk_usart::starty = 0;

// uint8_t* sdk_usart::ptr_buffer;
// uint16_t  sdk_usart::g_buffer_size; 	// p_buffer_size;
// uint8_t* sdk_usart::ptr_buffer_default;
// uint16_t  sdk_usart::g_buffer_default_size; 	// p_buffer_size;
// uint16_t  sdk_usart::g_packet_length;
// volatile uint8_t  sdk_usart::status = SDK_USART_RX_STATUS__FREE;
// uint8_t   sdk_usart::g_byte_counter;
// //uint8_t   sdk_usart::rx_byte_counter;
// //uint8_t   sdk_usart::tx_byte_counter;
// #define rx_byte_counter	g_byte_counter
// #define tx_byte_counter	g_byte_counter

sdk_usart::callback_protocol_functions_t* sdk_usart::g_ptr_callback_protocol_functions;
sdk_usart::usart_handle_mod_t sdk_usart::g_handle;

bool sdk_usart::status_free(void)	{
	return (
		(SDK_USART_TX_STATUS__FREE == g_handle.State) ||
		(SDK_USART_RX_STATUS__FREE == g_handle.State)
	);
}

// ALL-THREAD function 

uint8_t sdk_usart::send_non_blocking(uint8_t* p_ptrBuffer)	{
	uint8_t result = 0;

	if(status_free())	{
		g_handle.ptrBuffer 		= p_ptrBuffer; 						// ptr_buffer = p_ptrBuffer;
		g_handle.BufferSize   	= SDK_USART_DEFAULT_BUFFER_SIZE; 	// g_buffer_size = SDK_USART_DEFAULT_BUFFER_SIZE;
		g_handle.PacketLength 	= 0;								// g_packet_length = 0;
		g_handle.DataCounter  	= 0;								// tx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(SDK_USART_DEFAULT_BUFFER_SIZE);
		g_handle.State			= SDK_USART_TX_STATUS__RUNNING;		// status = SDK_USART_TX_STATUS__RUNNING;
		result = 1;
	}

	return result;
}

uint8_t sdk_usart::send_non_blocking(uint8_t* p_ptrBuffer, uint8_t p_size_buffer)	{
	uint8_t result = 0;

	if(status_free())	{
		g_handle.ptrBuffer 		= p_ptrBuffer; 						// ptr_buffer = p_ptrBuffer;
		g_handle.BufferSize   	= p_size_buffer;
		g_handle.PacketLength 	= 0;								// g_packet_length = 0;
		g_handle.DataCounter  	= 0;								// tx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(SDK_USART_DEFAULT_BUFFER_SIZE);
		g_handle.State			= SDK_USART_TX_STATUS__RUNNING;		// status = SDK_USART_TX_STATUS__RUNNING;
		result = 1;
	}

	return result;
}

uint8_t sdk_usart::send_non_blocking(char const *p_ptrBuffer)	{
	uint8_t result = 0;

	if(status_free())	{
		g_handle.ptrBuffer 		= (uint8_t* ) p_ptrBuffer; 			// ptr_buffer = (uint8_t* ) p_ptrBuffer;
		g_handle.BufferSize   	= SDK_USART_DEFAULT_BUFFER_SIZE; 	// g_buffer_size = SDK_USART_DEFAULT_BUFFER_SIZE;
		g_handle.PacketLength 	= 0;								// g_packet_length = 0;
		g_handle.DataCounter  	= 0;								// tx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(SDK_USART_DEFAULT_BUFFER_SIZE);
		g_handle.State			= SDK_USART_TX_STATUS__RUNNING;		// status = SDK_USART_TX_STATUS__RUNNING;
		result = 1;
	}

	return result;
}

void sdk_usart::send_blocking(uint8_t* p_ptrBuffer)	{
	while(!status_free()){sleep_for(10ms);}

	g_handle.ptrBuffer 		= p_ptrBuffer; 						// ptr_buffer = p_ptrBuffer;
	g_handle.BufferSize   	= SDK_USART_DEFAULT_BUFFER_SIZE; 	// g_buffer_size = SDK_USART_DEFAULT_BUFFER_SIZE;
	g_handle.PacketLength 	= 0;								// g_packet_length = 0;
	g_handle.DataCounter  	= 0;								// tx_byte_counter = 0;
	(g_ptr_callback_protocol_functions->callback_set_buffer_size)(SDK_USART_DEFAULT_BUFFER_SIZE);
	g_handle.State			= SDK_USART_TX_STATUS__RUNNING;		// status = SDK_USART_TX_STATUS__RUNNING;

	while(!status_free()){sleep_for(10ms);}						// while(SDK_USART_TX_STATUS__RUNNING == status){sleep_for(10ms);}

}

void sdk_usart::send_blocking(uint8_t* p_ptrBuffer, uint8_t p_size_buffer)	{
	while(!status_free()){sleep_for(10ms);}

	g_handle.ptrBuffer 		= p_ptrBuffer; 						// ptr_buffer = p_ptrBuffer;
	g_handle.BufferSize   	= p_size_buffer;
	g_handle.PacketLength 	= 0;								// g_packet_length = 0;
	g_handle.DataCounter  	= 0;								// tx_byte_counter = 0;
	(g_ptr_callback_protocol_functions->callback_set_buffer_size)(SDK_USART_DEFAULT_BUFFER_SIZE);
	g_handle.State			= SDK_USART_TX_STATUS__RUNNING;		// status = SDK_USART_TX_STATUS__RUNNING;

	while(!status_free()){sleep_for(10ms);}						// while(SDK_USART_TX_STATUS__RUNNING == status){sleep_for(10ms);}

}

void sdk_usart::send_blocking(char const *p_ptrBuffer)	{
	while(!status_free()){sleep_for(10ms);}

	g_handle.ptrBuffer 		= (uint8_t* ) p_ptrBuffer; 			// ptr_buffer = (uint8_t* ) p_ptrBuffer;
	g_handle.BufferSize   	= SDK_USART_DEFAULT_BUFFER_SIZE; 	// g_buffer_size = SDK_USART_DEFAULT_BUFFER_SIZE;
	g_handle.PacketLength 	= 0;								// g_packet_length = 0;
	g_handle.DataCounter  	= 0;								// tx_byte_counter = 0;
	(g_ptr_callback_protocol_functions->callback_set_buffer_size)(SDK_USART_DEFAULT_BUFFER_SIZE);
	g_handle.State			= SDK_USART_TX_STATUS__RUNNING;		// status = SDK_USART_TX_STATUS__RUNNING;

	while(!status_free()){sleep_for(10ms);}						// while(SDK_USART_TX_STATUS__RUNNING == status){sleep_for(10ms);}

}

// ALL-THREAD function 

bool sdk_usart::receive_free	(void)	{
	return (SDK_USART_RX_STATUS__FREE == g_handle.State);
}

bool sdk_usart::receive_check	(void)	{
	if(SDK_USART_RX_STATUS__READY_TO_READ == g_handle.State)	{
		g_handle.State			= SDK_USART_RX_STATUS__FREE;		// status = SDK_USART_RX_STATUS__FREE;
		return true;
	}
	return false;
}

void sdk_usart::receive_non_blocking(uint8_t p_length)	{
	if(SDK_USART_RX_STATUS__FREE == g_handle.State)	{
		g_handle.ptrBuffer 		= g_handle.ptrBufferDefault; 				
		g_handle.BufferSize   	= g_handle.BufferSizeDefault;				
		g_handle.PacketLength 	= p_length;							// g_packet_length = p_length;
		g_handle.DataCounter  	= 0;								// rx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(g_handle.BufferSizeDefault);
		g_handle.State			= SDK_USART_RX_STATUS__RUNNING;		// status = SDK_USART_RX_STATUS__RUNNING;
	}
}


void sdk_usart::receive_non_blocking(uint8_t* p_ptrBuffer, uint8_t p_size_buffer, uint8_t p_length)	{
	if(SDK_USART_RX_STATUS__FREE == g_handle.State)	{
		g_handle.ptrBuffer 		= p_ptrBuffer; 						// ptr_buffer = p_ptrBuffer;
		g_handle.BufferSize   	= p_size_buffer;				 	// g_buffer_size = p_size_buffer;
		g_handle.PacketLength 	= p_length;							// g_packet_length = p_length;
		g_handle.DataCounter  	= 0;								// rx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(p_size_buffer);
		g_handle.State			= SDK_USART_RX_STATUS__RUNNING;		// status = SDK_USART_RX_STATUS__RUNNING;
	}
}

void sdk_usart::receive_non_blocking(uint8_t* p_ptrBuffer, uint8_t p_size_buffer)	{
	if(SDK_USART_RX_STATUS__FREE == g_handle.State)	{
		g_handle.ptrBuffer 		= p_ptrBuffer; 						// ptr_buffer = p_ptrBuffer;
		g_handle.BufferSize   	= p_size_buffer;				 	// g_buffer_size = p_size_buffer;
		g_handle.PacketLength 	= 0;								// g_packet_length = 0;
		g_handle.DataCounter  	= 0;								// rx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(p_size_buffer);
		g_handle.State			= SDK_USART_RX_STATUS__RUNNING;		// status = SDK_USART_RX_STATUS__RUNNING;
	}
}

void sdk_usart::receive_non_blocking(char* p_ptrBuffer, uint8_t p_size_buffer)	{
	if(SDK_USART_RX_STATUS__FREE == g_handle.State)	{
		g_handle.ptrBuffer 		= (uint8_t *) p_ptrBuffer; 			// ptr_buffer = (uint8_t *) p_ptrBuffer;
		g_handle.BufferSize   	= p_size_buffer;				 	// g_buffer_size = p_size_buffer;
		g_handle.PacketLength 	= 0;								// g_packet_length = 0;
		g_handle.DataCounter  	= 0;								// rx_byte_counter = 0;
		(g_ptr_callback_protocol_functions->callback_set_buffer_size)(p_size_buffer);
		g_handle.State			= SDK_USART_RX_STATUS__RUNNING;		// status = SDK_USART_RX_STATUS__RUNNING;
	}
}



// ALL-THREAD function 

bool sdk_usart::check_loop_end(void)	{
	return (ch == 'X');
}


void sdk_usart::init(uint8_t* p_ptrBuffer, uint8_t p_size_buffer, callback_protocol_functions_t* p_ptr_callback_protocol_functions)	{
	g_handle.ptrBufferDefault  = p_ptrBuffer;
	g_handle.BufferSizeDefault = p_size_buffer;

	g_handle.State = SDK_USART_TX_STATUS__FREE;
	g_ptr_callback_protocol_functions = p_ptr_callback_protocol_functions;

}

void sdk_usart::buffer_management(int c)	{

	// increment tx byte counter
	g_handle.DataCounter++;

	// check packet length
	if(g_handle.PacketLength == 0) {

		// check the end of packet via terminating character = 0
		if((g_ptr_callback_protocol_functions->callback_check_packet_end)(c)) {
			// a null terminated character is received (terminated char = 0)
			// a char string is received

			// set the packet end condition
			g_handle.PacketLength = g_handle.DataCounter;

		} else {
			// get packet length
			g_handle.PacketLength = (g_ptr_callback_protocol_functions->callback_get_packet_length)(c);
		}
	} else {

		// check the end of packet via terminating character that is set reading header
		if((g_ptr_callback_protocol_functions->callback_check_packet_end)(c)) {
			// set the packet end condition
			g_handle.PacketLength = g_handle.DataCounter;
		}
	}

	// check the end of packet via terminated character set in packet header
	if((g_handle.DataCounter == g_handle.PacketLength) || (g_handle.DataCounter == g_handle.BufferSize)) {
		// the end of packet is reached
		buffer_management_end(c);
	} else {
		// the end of packet is NOT reached yet

		// increment pointer of tx buffer to point the next byte to send
		g_handle.ptrBuffer++;
	}

}

void sdk_usart::buffer_management_end(int c)	{

	switch (g_handle.State) {
		case SDK_USART_RX_STATUS__RUNNING:
			// update status 
			g_handle.State = SDK_USART_RX_STATUS__READY_TO_READ;
			break;
		case SDK_USART_TX_STATUS__RUNNING:
			addch(c);
			// update status 
			g_handle.State = SDK_USART_TX_STATUS__SEND_COMPLETED;
			break;
	}

}


// SPECIFIC-THREAD function 

void sdk_usart::start(void)	{
	initscr();
	clear();
	noecho();
	raw();					// Line buffering disabled. pass on everything. It does not need to press "enter" key to submit previous pressed key
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
		
	if(SDK_USART_TX_STATUS__RUNNING == g_handle.State) {
		c = *(g_handle.ptrBuffer);
		buffer_management(c);
		if(SDK_USART_TX_STATUS__RUNNING == g_handle.State) addch(c);
	}

	if(SDK_USART_TX_STATUS__SEND_COMPLETED == g_handle.State) {
		g_handle.State = SDK_USART_TX_STATUS__FREE;
	}

	c = getch();
	if(c != ERR) {
		if(c == 1) {
			if(ch == 'X') {
				return false;
			}
			ch = 'X';
		}
		if(c == 3) c = 0;

		if(SDK_USART_RX_STATUS__RUNNING == g_handle.State) {
			*(g_handle.ptrBuffer) = c;
			buffer_management(c);
		}
	}

	if(ch == 'X') {
		if(SDK_USART_RX_STATUS__RUNNING == g_handle.State) g_handle.State = SDK_USART_RX_STATUS__FREE;
	}

	return true;
	
}

/*
1   5    11   5    21   5    31   5    41   5    51   5    61   5    71   5    81   5    91   5    1
Questa è una prova. Vediamo se funziona!!! Hello World!!!!! By By By Marcooohoooohohohhooooo||!!! ^[
*/