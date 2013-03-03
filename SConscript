env = Environment()
Import('env')

mainFiles = [
	'is_core.cpp',
	'is_glut.cpp',
	'is_texture.cpp',
	'is_layouter.cpp',
	'is_graphics.cpp',
	'is_data_1d.cpp'
]

main = ['complex_test.cpp']
main.extend(mainFiles)

env.Program(target = 'complex', source = main)

main = ['static_cc.cpp']
main.extend(mainFiles)

env.Program(target = 'static_cc', source = main)

main = ['simple_2d_test.cpp']
main.extend(mainFiles)

env.Program(target = '2d_test', source = main)

main = ['simple_test.cpp']
main.extend(mainFiles)

env.Program(target = 'simple_test', source = main)

main = ['anti_gaussian.cpp']
main.extend(mainFiles)

env.Program(target = 'anti', source = main)

env.Library(target = 'in_sight', source = mainFiles);
