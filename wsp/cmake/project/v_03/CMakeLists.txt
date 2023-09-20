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
message(STATUS "---- start section of <module_libs_sdk_${WP_PLATFORM_STR}> module ")

# step 1 - 1st level path definition
set(CODE_DIR        ../../../../../.. )
set(CODE_CMAKE_MODULE_VER_DIR      ${CODE_DIR}/${CMAKE_MODULE_VER_RPATH} )
include(${CODE_DIR}/${CMAKE_MODULE_RPATH}/set_apps_libs_dirs.cmake )


# step 2 - 2nd level path definition - include path definition
include(include_dependencies.cmake )

# step 3 - set cpp sources files
include(set_cpp_files.cmake )
include(${LIBS_SDK_PLATFORM_CMAKE_PUBLIC_DIR}/set_lib_sdk_platform_name.cmake )

message(STATUS "SDK_PLATFORM_CONF_DIR: ${SDK_PLATFORM_CONF_DIR}")


add_library(${LIB_SDK_PLATFORM}
    ${CODE_FILES_SDK_PLATFORM_CPP}
)

set(EXECUTABLE ${LIB_SDK_PLATFORM} )

include(compile_option.cmake )

target_include_directories(${LIB_SDK_PLATFORM} PUBLIC 
    ${CODE_DIR_LIB_SDK_PLATFORM_INCLUDE}
)

target_compile_definitions(${LIB_SDK_PLATFORM} PRIVATE 
    SDK_MAC_LIB_VERSION=1.1
)

message(STATUS "---- end   section of <module_libs_sdk_${WP_PLATFORM_STR}> module ")

