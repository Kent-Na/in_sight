env = Environment()
Import('env')

mainFiles = [
	'is_core.cpp'
]

main = ['simple_test.cpp']
main.extend(mainFiles)

env.Program(target = 'main', source = main)

