cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CPP_COMPILER g++-10)
set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")
project(Roobik VERSION 0.1.0)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)



# include(CTest)
# enable_testing()


set(SOURCES
    src/main.cpp
    src/state.cpp
    src/utils.cpp
    src/eval.cpp
    src/minimax.cpp
    src/pattern.cpp
    src/server_sync.cpp
    # src/test.cpp
)

add_executable(Gomook ${SOURCES})

target_include_directories(Gomook
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)


# find_package(PythonLibs REQUIRED)
# include_directories(${PYTHON_INCLUDE_DIRS})
# target_link_libraries(Roobik ${PYTHON_LIBRARIES})

set_property(TARGET Gomook PROPERTY CXX_STANDARD 17)
target_link_libraries(Gomook PRIVATE Threads::Threads)
target_link_libraries(Gomook PRIVATE OpenSSL::SSL)
target_link_libraries(Gomook PRIVATE OpenSSL::Crypto)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
