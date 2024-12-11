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
## ******************************************************************
## __________________________________________________________________
## SDK PLATFORM LIB CPP FILES Definitions
trace_execution()

set(CODE_FILES_SDK_PLATFORM_CPP_DRIVERS
    ${LIBS_SDK_PLATFORM_SRC_DIR}/${SDK_PLATFORM_DRIVERS_DIR}/sdk_process.cpp
    ${LIBS_SDK_PLATFORM_SRC_DIR}/${SDK_PLATFORM_DRIVERS_DIR}/sdk_usart.cpp
)



set(CODE_FILES_SDK_PLATFORM_CPP
    ${CODE_FILES_SDK_PLATFORM_CPP_DRIVERS}
)


end_include()
