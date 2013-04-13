compile_source_message = '\033[34mCompiling > \033[0m$SOURCE'
link_program_message = '\033[34mLinking > \033[0m$TARGET'

env = Environment(
	CXXCOMSTR = compile_source_message,
	CCCOMSTR = compile_source_message,
#ARCOMSTR = link_library_message,
#	RANLIBCOMSTR = ranlib_library_message,
#	SHLINKCOMSTR = link_shared_library_message,
	LINKCOMSTR = link_program_message,
)

#env.Append(CCFLAGS = ['-O3'])
#env.Replace(CC = 'clang')
#env.Replace(CXX = 'clang++')
#env.Replace(CC = 'gcc-4.6')
#env.Replace(CXX = 'g++-4.6')
env.Append(CCFLAGS = ['-Wall','-g'])
#env.Append(CCFLAGS = ['-Wall','-g','-O3'])
env.Append(CFLAGS = ['-Wno-pointer-sign','-Wno-deprecated-declarations',
					 '-Wno-narrowing','-Wno-unused'])
env.Append(CCFLAGS = ['-Wno-narrowing','-Wno-unused'])
env.Append(CXXFLAGS = ['-xc++','-std=gnu++0x'])
env.Append(LIBS = ['m','freetype'])
env.Append(LIBS = ['GL','glut'])
#env.Append(LIBPATH = ['/usr/local/pgsql/lib'])
env.Append(CPPPATH = ['/usr/include/freetype2'])
env.Append(FRAMEWORKS = ['openGL','GLUT'])

Export('env')

SConscript('SConscript', variant_dir='build', duplicate=0)

