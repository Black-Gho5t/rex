cmake_minimum_required(VERSION 3.22.1)

project("rex")

add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp
        rex_core.cpp
        elf_trampoline.cpp
        art_runtime_hook.cpp
        hidden_api_hook.cpp
        vfs_hook.cpp
        isa_translator.cpp
        elf_loader.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log)
