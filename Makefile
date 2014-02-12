all:
	g++ -w -lGL -lGLU -lglut imageloader.cpp  Terrainlib.cpp glm.cpp objRendererlib.cpp physics.cpp  gameSource.cpp 

