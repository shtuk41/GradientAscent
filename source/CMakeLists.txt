
set(MY_HEADERS
    "${CMAKE_SOURCE_DIR}/include/common_defs.h" 
	"${CMAKE_SOURCE_DIR}/include/activity_stream.h" 
    "${CMAKE_SOURCE_DIR}/include/geo_data_handler.h"
)

source_group("Header Files" FILES ${MY_HEADERS})


add_executable(GradientAscent 
	main.cpp
)

target_sources(GradientAscent PRIVATE 
    ${MY_HEADERS}
	${RAPIDXML_INCLUDE_DIRS}
)

# Headers
target_include_directories(GradientAscent PRIVATE 

	"${CMAKE_SOURCE_DIR}/include"
	${RAPIDXML_INCLUDE_DIRS}
)

# Link the imported target
target_link_libraries(GradientAscent PRIVATE 
    GDAL::GDAL
    CURL::libcurl
)

# get_target_property(CURRENT_INCLUDES GradientAscent INCLUDE_DIRECTORIES)
# message(STATUS "GradientAscent target includes: ${CURRENT_INCLUDES}")

set_target_properties(GradientAscent PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/bin"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)