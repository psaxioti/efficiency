cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_COMPILER   /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

project(efficiency VERSION 1.5 LANGUAGES CXX)
add_definitions(-DVERSION=${CMAKE_PROJECT_VERSION})

cmake_policy(SET CMP0100 NEW)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_CROSSCOMPILING)
    find_library(GSL_LIBRARIES
        NAMES libgsl libgslblas
        PATHS ${CMAKE_FIND_ROOT_PATH}/lib
        REQUIRED)
    set(CUSTOM off)
else()
    find_package(GSL REQUIRED)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
#find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

include_directories(include)
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE headers include/*.h*)

if(${QT_VERSION_MAJOR} VERSION_LESS 6)
    file(GLOB_RECURSE resources Resources/*.qrc)
endif()

set(CMAKE_CXX_FLAGS -ffloat-store)

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    ${sources}
    ${headers}
    ${resources}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    ${GSL_LIBRARIES}
)

if(${QT_VERSION_MAJOR} VERSION_GREATER_EQUAL 6)
    qt_add_resources(${CMAKE_PROJECT_NAME} "app_images"
        PREFIX Icons
        BASE Resources/Icons
        FILES
            Resources/Icons/Debertin.png
            Resources/Icons/IAEA.png
            Resources/Icons/Kalfas.png
    )

    qt_add_resources(${CMAKE_PROJECT_NAME} "sourcesLib"
        PREFIX sources
        BASE Resources
        FILES
            Resources/Sources_DB.lib
    )
endif()

if(CMAKE_CROSSCOMPILING)
    set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${WinArch}
            WIN32_EXECUTABLE ON
#            SUFFIX "_${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
    )
    file(COPY ${PROJECT_SOURCE_DIR}/Data DESTINATION ${CMAKE_SOURCE_DIR}/${WinArch})
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND find_dlls ${CMAKE_FIND_ROOT_PATH}
        COMMAND make_msi  ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION} "44F0A703-674F-4831-87F5-E50599899F73"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${WinArch}
    )
else()
    set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
#            SUFFIX "_Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}"
            MACOSX_BUNDLE ON
    )
endif()

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY Data DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME})
