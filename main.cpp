#include <iostream>
#include "backcache.hpp"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
	BackCache<int,1000> bc;

	// Operator[]
	bc[12]=4444;
	int a=bc[1];
	cout<<a;

    return 0;
}
