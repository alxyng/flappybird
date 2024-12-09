cmake_minimum_required(VERSION 3.5)

project(flappybird)

add_subdirectory(deps/SDL)

add_subdirectory(deps/SDL_image)

set(SDL2MIXER_OPUS OFF)
add_subdirectory(deps/SDL_mixer)

# Create your game executable target as usual
add_executable(flappybird flappybird.c)

set_property(TARGET flappybird PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_include_directories(flappybird PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_image_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(flappybird PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(flappybird PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)

find_library(MATH_LIBRARY m)
if (MATH_LIBRARY)
    target_link_libraries(flappybird PRIVATE ${MATH_LIBRARY})
endif()
