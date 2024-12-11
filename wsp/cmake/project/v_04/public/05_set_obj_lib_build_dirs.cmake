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

## root folder: relative path of "src" (project root folder) respect project build folder
## linker needs the following definitions to set library path
## this variable is used in "target_link_options" statement of "tool_cmd.cmake" file of ${MAIN_NAME} app and
## it is necessary to inform the linker where the library obj file is located
## the CODE_DIR__FROM_BUILD variable will be set by module which will launched the link command
set(LIBS_SDK_PLATFORM_OBJ_LIB_DIR__FROM_BUILD   ${CODE_DIR__FROM_BUILD}/libs/mpfw_code_libs_sdk_${WP_PLATFORM_STR}/src/v_${SDK_PLATFORM_LIB_VER} )

end_include()
