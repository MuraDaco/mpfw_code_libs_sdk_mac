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
    -g
    -O3 
    -Wall 
    -Wextra 
    -pedantic   
    -fno-common 
    -ffunction-sections 
    -fdata-sections 
    -fno-builtin 
    -fno-rtti 
    -fno-exceptions    
    -DWP_PLATFORM_MAC
    -DRS485_ADDRESS=0x32
    # $<$<BOOL:${WP_PLATFORM_LINUX}>:-DWP_PLATFORM_LINUX>
    # $<$<BOOL:${WP_PLATFORM_NXP}>:  -DWP_PLATFORM_NXP>
    -Wfatal-errors
)
