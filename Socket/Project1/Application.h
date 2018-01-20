#pragma once
#include "../EmailDLL/EmailDLL.h"
#include "../RegistrationDLL/RegistrationDLL.h"

class Application
{
public:
	Application() {};
	~Application() {};

public:
	//void run();

public:
	EmailDLL E;
	RegistrationDLL R;
};

