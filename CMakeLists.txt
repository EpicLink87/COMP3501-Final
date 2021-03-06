cmake_minimum_required(VERSION 2.6)

# Name of project
project(SubAttack)

# Specify project files: header files and source files
set(HDRS
    asteroid.h 
	camera.h 
	game.h 
	light_node.h
	model_loader.h
	resource.h 
	resource_manager.h 
	scene_graph.h 
	scene_node.h 
	particle_system.h
	particle_fountain.h
	skybox.h
	player.h 
	turret.h
	world.h
	bound.h
	attack.h
	enemy.h
	mine.h
	submarine.h
	pulse.h
	torpedo.h
	depthCharge.h
)
 
set(SRCS
    asteroid.cpp 
	camera.cpp 
	game.cpp 
	light_node.cpp
	main.cpp 
	resource.cpp 
	resource_manager.cpp 
	scene_graph.cpp 
	scene_node.cpp 
	particle_system.cpp
	particle_fountain.cpp
	player.cpp 
	turret.cpp 
	world.cpp
	skybox.cpp
	bound.cpp
	attack.cpp
	enemy.cpp
	mine.cpp
	submarine.cpp
	pulse.cpp
	torpedo.cpp
	depthCharge.cpp
	material_vp.glsl 
	material_fp.glsl
	particle_vp.glsl
	particle_gp.glsl
	particle_fp.glsl
	UI_vp.glsl 
	UI_fp.glsl
	skybox_fp.glsl
	skybox_vp.glsl
	explosion_vp.glsl
	explosion_gp.glsl
	explosion_fp.glsl
	pulse_vp.glsl
	pulse_gp.glsl
	pulse_fp.glsl
	bubbles_vp.glsl
	bubbles_gp.glsl
	bubbles_fp.glsl
	trail_vp.glsl
	trail_gp.glsl
	trail_fp.glsl
)

# Add path name to configuration file
configure_file(path_config.h.in path_config.h)

# Add executable based on the source files
add_executable(SubAttack ${HDRS} ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(SubAttack ${OPENGL_gl_LIBRARY})

# Other libraries needed
set(LIBRARY_PATH "" CACHE PATH "Folder with GLEW, GLFW, GLM, and SOIL libraries")
include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(SubAttack ${GLEW_LIBRARY})
target_link_libraries(SubAttack ${GLFW_LIBRARY})
target_link_libraries(SubAttack ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(SubAttack PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)
