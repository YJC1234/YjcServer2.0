file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)


add_library(WebFrame STATIC ${srcs})

target_include_directories(WebFrame PUBLIC include)
target_link_libraries(WebFrame PUBLIC Config BaseNet)