cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Wconversion")
endif()
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

set(CMAKE_C_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

project("order_book")

set(SOURCE_FILES src/StreamManager.cpp src/Book.cpp)

# Add source to this project's executable.
add_executable(order_book "order_book.cpp" ${SOURCE_FILES})

# Set the directories that should be included in the build command for this target when running g++
# these will be included as -I/directory/path/
target_include_directories(order_book PRIVATE ${PROJECT_SOURCE_DIR}/include)
