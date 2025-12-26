cmake_minimum_required(VERSION 4.0)
project(CountryBrawl)

#Yeah, it is pretty cool that SDL thinks it can handle main ... but no thanks
add_definitions(-DSDL_MAIN_HANDLED)

set(CMAKE_CXX_STANDARD 20)


file (COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

if (UNIX)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 sdl2)
    pkg_check_modules(SDL2_image SDL2_image)
    pkg_check_modules(SDL2_ttf SDL2_ttf)
    pkg_check_modules(SDL2_mixer SDL2_mixer)
    link_libraries(${SDL2_LIBRARIES})
    link_libraries(${SDL2_image_LIBRARIES})
    link_libraries(${SDL2_mixer_LIBRARIES})
    link_libraries(${SDL2_ttf_LIBRARIES})
endif (UNIX)
if (WIN32)
    set(SDL2_PATH "C:/libs/SDL2")
    set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include")
    set(SDL2_LIB_DIR "${SDL2_PATH}/lib")

    include_directories(${SDL2_INCLUDE_DIR})
    link_directories(${SDL2_LIB_DIR})

    link_libraries(SDL2)
    link_libraries(SDL2main)
    link_libraries(SDL2_image)
    link_libraries(SDL2_ttf)
    link_libraries(SDL2_mixer)

endif (WIN32)


add_executable(CountryBrawl main.cpp
        engine.cpp
        engine.h
        texwrap.cpp
        getAssets.h
        scene.cpp
        scene.h
        inputData.h
        startMenu.cpp
        startMenu.h
        menuItem.cpp
        menuItem.h
        country.cpp
        game.cpp
        game.h
        tile.cpp
        mapData.cpp)
