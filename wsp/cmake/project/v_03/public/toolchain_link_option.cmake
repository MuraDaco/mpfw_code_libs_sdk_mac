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
add_definitions(
    -Wfatal-errors
    -std=gnu++14
    )

# add_compile_options(-std=gnu++14)

target_link_options(${EXECUTABLE} PRIVATE
    -O3 
    -Wall 
    -Wextra 
    -pedantic 
    -lncurses
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMENT "mac - Platform selected: ${WP_PLATFORM_STR} - Config fw2_lib folder: ${FW2_LIB_CONFIG_DIR}"
)

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND size ${EXECUTABLE}
)
