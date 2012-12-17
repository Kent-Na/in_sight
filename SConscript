env = Environment()
Import('env')

mainFiles = [
	'is_core.cpp',
	'is_texture.cpp',
	'is_layouter.cpp',
	'is_graphics.cpp',
	'is_data_1d.cpp'
]

main = ['complex_test.cpp']
main.extend(mainFiles)

env.Program(target = 'main', source = main)

