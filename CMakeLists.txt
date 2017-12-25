cmake_minimum_required(VERSION 3.0)

set(SRC 
	"is_core.cpp"
	"is_glut.cpp"
	"is_cocoa.mm"
	"is_texture.cpp"
	"is_graphics.cpp"
	"is_view.cpp"
	"is_layouter.cpp")

add_executable(main ${SRC} "simple_2d_test.cpp")

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

target_include_directories(main PUBLIC 
    ${FREETYPE_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS} 
    ${GLUT_INCLUDE_DIRS})

target_link_libraries(main 
    ${FREETYPE_LIBRARIES}
    ${OPENGL_LIBRARIES} 
    ${GLUT_LIBRARY})
