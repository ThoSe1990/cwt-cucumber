
set(cucumber-src 
  ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cpp
)

add_library(cucumber STATIC ${cucumber-src})
add_library(cucumber-no-main STATIC ${cucumber-src})
