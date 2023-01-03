cmake_minimum_required(VERSION 3.16)

project(efficiency VERSION 1.5 LANGUAGES CXX)
add_definitions(-DVERSION=${CMAKE_PROJECT_VERSION})

set(CUSTOM ON CACHE BOOL "Specify where the data files will be found.")
set(CMAKE_INSTALL_PREFIX bin)

cmake_policy(SET CMP0100 NEW)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_CROSSCOMPILING)
    find_library(GSL_LIBRARIES
        NAMES libgsl libgslblas
        PATHS ${CMAKE_FIND_ROOT_PATH}/lib)
    set(CUSTOM off)
else()
    find_package(GSL REQUIRED)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
#find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

include_directories(include)

set(CMAKE_CXX_FLAGS -ffloat-store)

file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE headers include/*.h*)

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    ${sources}
    ${headers}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    ${GSL_LIBRARIES}
)

#qt_add_resources(${CMAKE_PROJECT_NAME} "app_images"
#    PREFIX ${CMAKE_SOURCE_DIR}
#    BASE Data/Icons
#    FILES
#        /Data/Icons/Debertin.png
#        /Data/Icons/IAEA.png
#        /Data/Icons/Kalfas.png
#)

if(${CUSTOM})
    set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
#            SUFFIX "_Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
            MACOSX_BUNDLE ON
    )
endif()

if(CMAKE_CROSSCOMPILING)
    set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${WinArch}
            WIN32_EXECUTABLE ON
#            SUFFIX "_${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
    )
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND find_dlls ${CMAKE_FIND_ROOT_PATH}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${WinArch}
    )
endif()

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
