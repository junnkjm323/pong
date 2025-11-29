cmake_minimum_required(VERSION 3.28)
project(Pong CXX)

find_package(SDL2 REQUIRED)

add_library(game STATIC src/Game.cpp)
add_executable(pong src/Main.cpp)

target_link_libraries(pong PRIVATE 
    game
    # find_packageで取得したSDL2のインクルード情報とリンク情報を適用
    SDL2::SDL2 
)

target_include_directories(game PUBLIC include)
target_include_directories(pong PRIVATE include)