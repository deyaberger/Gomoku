cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CPP_COMPILER g++-10)
set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "-O3")
add_compile_options(-O3)
project(Gomook VERSION 0.1.0)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

# message("This is normal")
# message("${Red}This is Red${ColourReset}")
# message("${Green}This is Green${ColourReset}")
# message("${Yellow}This is Yellow${ColourReset}")

# include(CTest)
# enable_testing()

if(V_TERM)
  message(STATUS "${Red}SIMPLE TERMINAL VERSION${ColourReset}")
  add_definitions(-DV_TERM)
  set(SOURCES
    src/main.cpp
    src/state.cpp
    src/utils.cpp
    src/eval.cpp
    src/minimax.cpp
    src/pattern.cpp
)
else()
  set(SOURCES
    src/main.cpp
    src/state.cpp
    src/utils.cpp
    src/eval.cpp
    src/minimax.cpp
    src/pattern.cpp
    src/server_sync.cpp
    )
    find_package(Boost REQUIRED)
endif()

add_executable(Gomook ${SOURCES})
find_package(Threads REQUIRED)
target_link_libraries(Gomook PRIVATE Threads::Threads)

if(SINGLE_FRED)
  message(STATUS "MULTITHREADING ${Red}DEACTIVATED${ColourReset}")
  add_definitions(-DSINGLE_THREAD)
#   set(SINGLE_FRED ON)
else()
    message(STATUS "MULTITHREADING ${Green}ACTIVATED${ColourReset}")

endif()

if(SSL)
  message(STATUS "SSL  ${Green}ACTIVATED${ColourReset}") 
  add_definitions(-DSSL)
  find_package(OpenSSL REQUIRED)
  target_link_libraries(Gomook PRIVATE OpenSSL::SSL)
  target_link_libraries(Gomook PRIVATE OpenSSL::Crypto)
#   set(SSL ON)
else()
  message(STATUS "SSL ${Red}DEACTIVATED${ColourReset}")
endif()



target_include_directories(Gomook
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)


set_property(TARGET Gomook PROPERTY CXX_STANDARD 17)
