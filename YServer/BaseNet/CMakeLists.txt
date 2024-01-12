file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

find_library(URING uring REQUIRED)

add_library(BaseNet STATIC ${srcs})

target_include_directories(BaseNet PUBLIC include)
target_link_libraries(BaseNet PUBLIC ${URING} Config spdlog::spdlog WebFrame)
