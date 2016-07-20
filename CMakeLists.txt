cmake_minimum_required(VERSION 2.8)
project(final)

set(SOURCE_FILES
        main.c
        settings.c
        server.c)

add_executable(final ${SOURCE_FILES})
target_link_libraries(final pthread)
