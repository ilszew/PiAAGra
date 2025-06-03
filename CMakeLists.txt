cmake_minimum_required(VERSION 3.30)
project(PiAA_Warcaby)

set(CMAKE_CXX_STANDARD 20)

set(SFML_ROOT "C:/SFML-2.5.1")
set(CMAKE_MODULE_PATH "${SFML_ROOT}/lib/cmake/SFML" ${CMAKE_MODULE_PATH})

find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)

add_executable(Warcaby
        main.cpp
        Game.cpp
        Board.cpp
        GraphicalGame.cpp
)

target_link_libraries(Warcaby
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
)

if(WIN32)
    find_file(SFML_SYSTEM_DLL sfml-system-2.dll PATHS "${SFML_ROOT}/bin")
    find_file(SFML_WINDOW_DLL sfml-window-2.dll PATHS "${SFML_ROOT}/bin")
    find_file(SFML_GRAPHICS_DLL sfml-graphics-2.dll PATHS "${SFML_ROOT}/bin")
    find_file(SFML_AUDIO_DLL sfml-audio-2.dll PATHS "${SFML_ROOT}/bin")

    add_custom_command(TARGET Warcaby POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_SYSTEM_DLL}"
            "${SFML_WINDOW_DLL}"
            "${SFML_GRAPHICS_DLL}"
            "${SFML_AUDIO_DLL}"
            $<TARGET_FILE_DIR:Warcaby>)
endif()