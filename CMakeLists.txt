cmake_minimum_required(VERSION 3.1)

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SRC 
	"src/is_core.cpp"
	"src/is_glut.cpp"
	"src/is_cocoa.mm"
	"src/is_texture.cpp"
	"src/is_graphics.cpp"
	"src/is_view.cpp"
	"src/is_layouter.cpp")

set(INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${FREETYPE_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS} 
    ${GLUT_INCLUDE_DIRS})

set(LIBS
    ${FREETYPE_LIBRARIES}
    ${OPENGL_LIBRARIES} 
    ${GLUT_LIBRARY})

set(INSIGHT_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)
set(INSIGHT_DEPENDENCY_LIBRARIES ${LIBS} PARENT_SCOPE)

add_library(in_sight_obj OBJECT ${SRC})
target_include_directories(in_sight_obj PUBLIC ${INCLUDE_DIRS})

add_library(in_sight $<TARGET_OBJECTS:in_sight_obj>)
target_include_directories(in_sight PUBLIC ${INCLUDE_DIRS})
target_link_libraries(in_sight PUBLIC ${LIBS})
