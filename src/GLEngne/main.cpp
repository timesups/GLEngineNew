#include "../Engine/core/Window.hpp"





int main() 
{
	Window window;
	if (!window.Create(800, 600, "GLEngle")) 
	{
		return -1;
	}



	while (window.IsRunning())
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);







		window.Update();
	}
}