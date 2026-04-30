cmake_minimum_required(VERSION 3.14)

project(TextRPG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── nlohmann/json (header-only, fetched automatically) ────────────────────────
include(FetchContent)
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(SOURCE_FILES
    src/main.cpp
    src/domain/inventory.cpp
    src/domain/enemy.cpp
    src/domain/player.cpp
    src/db/json_helpers.cpp
    src/db/data_loader.cpp
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

target_link_libraries(textrpg nlohmann_json::nlohmann_json)

if (MSVC)
    target_compile_options(textrpg PRIVATE /W4)
else()
    target_compile_options(textrpg PRIVATE
        -Wall
        -Wextra
        -O2
    )
endif()