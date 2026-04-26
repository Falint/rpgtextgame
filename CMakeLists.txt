cmake_minimum_required(VERSION 3.10)

project(TextRPG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
    src/main.cpp
    src/domain/inventory.cpp
    src/domain/enemy.cpp
    src/domain/player.cpp
    src/data/monsters.cpp
    src/data/weapons.cpp
    src/data/items.cpp
    src/data/upgrades.cpp
    src/game/item_service.cpp
    src/game/battle_service.cpp
    src/game/shop_service.cpp
    src/tui/terminal.cpp
    src/tui/renderer.cpp
    src/tui/screens.cpp
    src/tui/screens2.cpp
    src/tui/screens3.cpp
    src/tui/app.cpp
)

add_executable(textrpg ${SOURCE_FILES})

target_include_directories(textrpg PRIVATE
    src
)

if (MSVC)
    target_compile_options(textrpg PRIVATE /W4)
else()
    target_compile_options(textrpg PRIVATE
        -Wall
        -Wextra
        -O2
    )
endif()