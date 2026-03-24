#include "../Engine/core/Window.hpp"

class B;

class A 
{
public:
	A() = default;
private:
	int value1 = 20;
	friend class B;
	int value = 10;
};

class B
{
public:
	B() 
	{
		std::cout << a.value1 << std::endl;
	
	}
	A a;
};




int main() 
{
	B b;
}