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

set(INSIGHT_INCLUDE_DIRS
    include
    ${FREETYPE_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS} 
    ${GLUT_INCLUDE_DIRS})

set(LIBS
    jsoncpp
    ${FREETYPE_LIBRARIES}
    ${OPENGL_LIBRARIES} 
    ${GLUT_LIBRARY})

add_library(in_sight_obj OBJECT ${SRC})
target_include_directories(in_sight_obj PUBLIC ${INSIGHT_INCLUDE_DIRS})
#target_link_libraries(in_sight_obj PUBLIC ${LIBS})

add_library(in_sight $<TARGET_OBJECTS:in_sight_obj>)
target_include_directories(in_sight PUBLIC ${INSIGHT_INCLUDE_DIRS})
target_link_libraries(in_sight PUBLIC ${LIBS})

add_executable(main $<TARGET_OBJECTS:in_sight_obj> "main.cpp")
target_include_directories(main PRIVATE ${INSIGHT_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${LIBS})
