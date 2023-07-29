// ComUdpProxy.cpp : Defines the entry point for the application.
//

#include "ComUdpProxy.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "Hello ComUdpProxy." << endl;

	for (int i = 1; i < argc; i++)
	{
		cout << argv[i] << endl;
	}


	return 0;
}
