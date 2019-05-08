cmake_minimum_required(VERSION 3.10)

project(blox VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(lmdb CONFIG REQUIRED)

add_library(libblox
        blox/datastore.cpp
        blox/datastore.h blox/iterator.cpp blox/iterator.h blox/cursor.cpp blox/cursor.h blox/map.cpp blox/map.h)

set_target_properties(libblox PROPERTIES OUTPUT_NAME blox)
target_include_directories(libblox PUBLIC 
                           $<BUILD_INTERFACE:${blox_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include>)
target_link_libraries(libblox PUBLIC lmdb)
if (MSVC) 
    target_compile_options(libblox PRIVATE /W4 /WX /MP)
else() 
    target_compile_options(libblox PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

if(BUILD_TESTING)
    find_package(GTest MODULE REQUIRED)
    add_executable(blox_test 
                   test/blox_test.cpp 
                   test/main.cpp)
    target_link_libraries(blox_test PRIVATE libblox GTest::GTest GTest::Main)
    gtest_discover_tests(blox_test)
    if (MSVC) 
        target_compile_options(blox_test PRIVATE /W4 /WX /MP)
    else() 
        target_compile_options(blox_test PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${blox_BINARY_DIR}/bloxConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(TARGETS libblox
        EXPORT bloxTargets
        INCLUDES DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/blox)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${blox_SOURCE_DIR}/cmake/bloxConfig.cmake"
    "${blox_BINARY_DIR}/bloxConfig.cmake"
    INSTALL_DESTINATION share/cmake/blox
)

install(EXPORT bloxTargets DESTINATION share/cmake/blox)
install(FILES "${blox_BINARY_DIR}/bloxConfigVersion.cmake"
              "${blox_BINARY_DIR}/bloxConfig.cmake"
        DESTINATION share/cmake/blox)