#   *******************************************************************************
#   
#   mpfw / fw2 - Multi Platform FirmWare FrameWork
#   Copyright (C) (2023) Marco Dau
#   
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published
#   by the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#   
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.
#   
#   You can contact me by the following email address
#   marco <d o t> ing <d o t> dau <a t> gmail <d o t> com
#   
#   *******************************************************************************
target_compile_options(${EXECUTABLE} PRIVATE
    -Os 
    -fno-common 
    -g 
    -Wall 
    -c 
    -ffunction-sections 
    -fdata-sections 
    -ffreestanding 
    -fno-builtin 
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>: -fno-rtti -fno-exceptions -std=gnu++14>        

    $<$<BOOL:${WP_PLATFORM_NXP}>: -mcpu=cortex-m4>
    $<$<BOOL:${WP_PLATFORM_NXP}>: -mfpu=fpv4-sp-d16>
    $<$<BOOL:${WP_PLATFORM_NXP}>: -mfloat-abi=hard>
    $<$<BOOL:${WP_PLATFORM_NXP}>: -mthumb>
    $<$<BOOL:${WP_PLATFORM_NXP}>: -fstack-usage>
    $<$<BOOL:${WP_PLATFORM_NXP}>: -specs=nano.specs>

    -Wfatal-errors

)

if(WP_PLATFORM_NXP)

    target_compile_definitions(${EXECUTABLE} PRIVATE
        -DCPU_LPC54608J512ET180 
        -DCPU_LPC54608J512ET180_cm4 
        -DFSL_RTOS_FREE_RTOS 
        -DSDK_OS_FREE_RTOS 
        -DSERIAL_PORT_TYPE_UART=1 
        -DSDK_DEBUGCONSOLE=1 
        -D__MCUXPRESSO 
        -D__USE_CMSIS 
        -D__NEWLIB__ 
    )

endif()
