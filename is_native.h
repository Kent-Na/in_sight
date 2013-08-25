namespace is{
#ifdef __APPLE__
	namespace native = cocoa;
#else
	namespace native = GLUT;
#endif
}
