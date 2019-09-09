cmake_minimum_required(VERSION 3.10)

project(datastore VERSION 1.0 LANGUAGES CXX)
include(CTest)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(lmdb CONFIG REQUIRED)

add_library(libdatastore
        datastore/client.cpp
        datastore/client.h
        datastore/clients/map.cpp
        datastore/clients/map.h
        datastore/clients/lmdb.cpp
        datastore/clients/lmdb.h
        datastore/map.cpp
        datastore/map.h
        datastore/bijective/stream.cpp
        datastore/bijective/stream.h
        datastore/clients/detail/lmdb.cpp
        datastore/clients/detail/lmdb.h
        datastore/clients/detail/map.cpp
        datastore/clients/detail/map.h
        datastore/bijective/function.cpp
        datastore/bijective/function.h
        datastore/bijective/map.cpp
        datastore/bijective/map.h
        datastore/bijective/pair.cpp
        datastore/bijective/pair.h
        )

set_target_properties(libdatastore PROPERTIES OUTPUT_NAME datastore)
target_include_directories(libdatastore PUBLIC
        $<BUILD_INTERFACE:${datastore_SOURCE_DIR}>
        $<BUILD_INTERFACE:${lmdb_INCLUDE_DIRS}>
                           $<INSTALL_INTERFACE:include>)
target_link_libraries(libdatastore PRIVATE lmdb)
if (MSVC)
    target_compile_options(libdatastore PRIVATE /W4 /WX /MP)
else ()
    target_compile_options(libdatastore PRIVATE -Wall -Wextra -pedantic -Werror)
    target_link_libraries(libdatastore PUBLIC stdc++fs)
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

if(BUILD_TESTING)
    find_package(GTest MODULE REQUIRED)
    add_executable(datastore_test
            test/datastore_test.cpp
            test/main.cpp
            test/map_test.cpp)
    target_link_libraries(datastore_test PRIVATE libdatastore GTest::GTest GTest::Main)
    gtest_discover_tests(datastore_test)
    if (MSVC)
        target_compile_options(datastore_test PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(datastore_test PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${datastore_BINARY_DIR}/datastoreConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(TARGETS libdatastore
        EXPORT datastoreTargets
        INCLUDES DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES
        datastore/client.h
        datastore/block.h
        DESTINATION include/datastore)
install(FILES
        datastore/clients/lmdb.h
        datastore/clients/map.h
        DESTINATION include/datastore/clients)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${datastore_SOURCE_DIR}/cmake/datastoreConfig.cmake"
        "${datastore_BINARY_DIR}/datastoreConfig.cmake"
        INSTALL_DESTINATION share/cmake/datastore
)

install(EXPORT datastoreTargets DESTINATION share/cmake/datastore)
install(FILES "${datastore_BINARY_DIR}/datastoreConfigVersion.cmake"
        "${datastore_BINARY_DIR}/datastoreConfig.cmake"
        DESTINATION share/cmake/datastore)