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
##################################################################################
## _______________________________________________________________________________
## RELATIVE PATH Definitions
trace_execution()

##################################################################################
## Relative path from CMakeLists.txt project folder                             ##
##################################################################################

include(${CODE_MAIN_CMAKE_LIBS_SDK_VER_DEF_DIR}/set_class_src_ver.cmake)

set(LIBS_SDK_PLATFORM_SRC_DIR                  ${LIBS_SDK_PLATFORM_DIR}/src/v_${SDK_PLATFORM_LIB_VER}              )

set(SDK_PLATFORM_DRIVERS_DIR                    drivers                         )


set(CODE_DIR_LIB_SDK_PLATFORM_INCLUDE
    ${SDK_PLATFORM_CONF_DIR}

    ${LIBS_SDK_PLATFORM_SRC_DIR}/${SDK_PLATFORM_DRIVERS_DIR}
    
)

end_include()
