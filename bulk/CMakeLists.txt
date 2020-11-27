cmake_minimum_required(VERSION 3.14)
project(BulkPaf20)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
fetchcontent_declare(
        bulk
        GIT_REPOSITORY https://github.com/jwbuurlage/Bulk
        GIT_TAG v2.0.0
        GIT_PROGRESS TRUE
)

FetchContent_GetProperties(bulk)
if (NOT bulk_POPULATED)
    FetchContent_Populate(bulk)
    add_subdirectory(${bulk_SOURCE_DIR} ${bulk_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

add_executable(BulkParallel main.cpp)
target_link_libraries(BulkParallel bulk_thread)