
file(GLOB_RECURSE MY_SHADERS "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")

# message(STATUS "The value of MY_VARIABLE is: ${CMAKE_SOURCE_DIR}")


set(IMGUI_SOURCES 
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_opengl3.cpp"
)


set(IMGUI_HEADERS
    "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui.h"
    "${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imconfig.h"
)

set (MY_SOURCES
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/axes3d.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/axisPlane.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/camera.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/context.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/optionsreader.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/render_object.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/shaders.cpp"
	"${CMAKE_SOURCE_DIR}/source/data_handle/sources/volume.cpp"
)

set(MY_HEADERS
    "${CMAKE_SOURCE_DIR}/source/data_handle/include/axes3d.h"
    "${CMAKE_SOURCE_DIR}/source/data_handle/include/axisPlane.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/camera.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/context.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/controls.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/exceptions.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/optionsreader.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/render_object.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/shaders.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/volume.h"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include/window.h"
	"${CMAKE_SOURCE_DIR}/include/common_defs.h" 
	"${CMAKE_SOURCE_DIR}/include/activity_stream.h" 
    "${CMAKE_SOURCE_DIR}/include/geo_data_handler.h"
)

source_group("Source Files\\imgui" FILES ${IMGUI_SOURCES})
source_group("Header Files\\imgui" FILES ${IMGUI_HEADERS})
source_group("Header Files" FILES ${MY_HEADERS})
source_group("Shaders" FILES ${MY_SHADERS})

add_executable(GradientAscent 
	main.cpp
	${MY_SOURCES} 
)

target_sources(GradientAscent PRIVATE 
    ${IMGUI_SOURCES} 
    ${IMGUI_HEADERS}
	${MY_SOURCES} 
    ${MY_HEADERS}
	${RAPIDXML_INCLUDE_DIRS}
)

# Headers
target_include_directories(GradientAscent PRIVATE 

	"${CMAKE_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/source/data_handle/include"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui"
	"${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends"
	${RAPIDXML_INCLUDE_DIRS}
)

# Link the imported target
target_link_libraries(GradientAscent PRIVATE 
    GDAL::GDAL
    CURL::libcurl
)

if(WIN32)
target_link_libraries(GradientAscent PRIVATE 
    GDAL::GDAL
    CURL::libcurl
	glfw
	glm::glm
	GLEW::GLEW
	opengl32
	soil
)
elseif(LINUX)
target_link_libraries(GradientAscent PRIVATE 
	GDAL::GDAL
    CURL::libcurl	
	glfw
	glm::glm
	GLEW::GLEW
	OpenGL::GL
	soil
)
endif()

# get_target_property(CURRENT_INCLUDES GradientAscent INCLUDE_DIRECTORIES)
# message(STATUS "GradientAscent target includes: ${CURRENT_INCLUDES}")

target_compile_features(GradientAscent PRIVATE cxx_std_20)

if(WIN32)
	add_custom_command(TARGET GradientAscent POST_BUILD

		# Copy shaders directory to both locations
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/source/data_handle/shaders" "${CMAKE_BINARY_DIR}/source/data_handle/shaders"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/source/data_handle/shaders" "${CMAKE_BINARY_DIR}/x64/$<CONFIG>/shaders"
	)

	set_target_properties(GradientAscent PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
	)
elseif(LINUX)
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
	
	set_target_properties(GradientAscent PROPERTIES
	    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	    #RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
	    #RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
	)

	add_custom_command(TARGET GradientAscent POST_BUILD

		# Copy shaders directory to both locations
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/CTLab/shaders" "${CMAKE_BINARY_DIR}/bin/shaders"
	)
endif()