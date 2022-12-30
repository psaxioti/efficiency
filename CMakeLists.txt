cmake_minimum_required(VERSION 3.16)

project(efficiency VERSION 1.5 LANGUAGES CXX)

#set(VERSION ${CMAKE_PROJECT_VERSION})
add_definitions(-DVERSION=${CMAKE_PROJECT_VERSION})
set(CUSTOM ON)
set(QtVersion 6)
set(CMAKE_INSTALL_PREFIX bin)

option(QtVersion "Specify Qt version to use. Supported versions are 5 and 6.")

cmake_policy(SET CMP0100 NEW)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package("Qt${QtVersion}" REQUIRED COMPONENTS Widgets PrintSupport)

find_package(GSL REQUIRED)

include_directories(include)

set(CMAKE_CXX_FLAGS -ffloat-store)

file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE headers include/*.h*)

add_executable(efficiency
    main.cpp
    ${sources}
    ${headers}
)

target_link_libraries(efficiency PRIVATE 
    Qt::Widgets
    Qt::PrintSupport
    ${GSL_LIBRARIES}
    m
)

if(${CUSTOM})
    add_definitions(-DCUSTOM)
    set_target_properties(efficiency
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
            SUFFIX "_${QT_DEFAULT_MAJOR_VERSION}"
    )
endif()

install(TARGETS efficiency DESTINATION ${CMAKE_INSTALL_PREFIX})