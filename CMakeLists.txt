cmake_minimum_required(VERSION 3.16)
include(GNUInstallDirs)

set(PROJ KanoopPiQt)
project(${PROJ})

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiod>=2.0)
qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(KANOOP_PI_VERSION "2.0.0")

file(GLOB_RECURSE KANOOP_PI_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE KANOOP_PI_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

qt_add_library(${PROJ} MANUAL_FINALIZATION ${KANOOP_PI_SOURCES} ${KANOOP_PI_HEADERS})

target_compile_options(${PROJ} PRIVATE -Wextra -Wall -Werror)

set_target_properties(${PROJ} PROPERTIES VERSION "${KANOOP_PI_VERSION}")
set_target_properties(${PROJ} PROPERTIES SOVERSION 2)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libKanoopPi.pc.in" "libKanoopPi.pc" @ONLY)

target_include_directories(${PROJ} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Kanoop)
target_include_directories(${PROJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(NOT TARGET KanoopCommonQt)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../KanoopCommonQt/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../KanoopCommonQt" KanoopCommonQt)
    else()
        message(FATAL_ERROR "KanoopCommonQt not found. Clone it beside this repo or build from the superproject.")
    endif()
endif()

target_link_libraries(${PROJ} PUBLIC KanoopCommonQt)
target_link_libraries(${PROJ} PRIVATE Qt6::Core PkgConfig::GPIOD)

add_compile_definitions(KANOOPPI_LIBRARY)
add_compile_definitions(QT_DEPRECATED_WARNINGS)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)

if (DEFINED $ENV{KYBER_OUTPUT_PREFIX})
    set(KANOOP_INSTALL_PREFIX "$ENV{KYBER_OUTPUT_PREFIX}/")
endif()

set(_no_metatypes "${CMAKE_CURRENT_BINARY_DIR}/${PROJ}_no_metatypes.json")
file(TOUCH "${_no_metatypes}")
set_target_properties(${PROJ} PROPERTIES INTERFACE_QT_META_TYPES_BUILD_FILE "${_no_metatypes}")
qt_finalize_target(${PROJ})

install(TARGETS ${PROJ} LIBRARY DESTINATION "${KANOOP_INSTALL_PREFIX}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/Kanoop" DESTINATION "${KANOOP_INSTALL_PREFIX}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}" FILES_MATCHING PATTERN "*.h")

option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
