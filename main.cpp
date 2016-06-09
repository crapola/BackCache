#include "backcache.hpp"
#include <iostream>

using namespace std;

void Test(const char* what,BackCache<int>& tested,const vector<int> expected,bool pending)
{
	cout<<what<<": ";
	bool test=(tested==expected)&&(tested.Pending()==pending);
	if (test)
		cout<<"OK\n";
	else
		cout<<"!!! ERROR !!!\n";
	tested.Info();
	tested.Reset();
	cout<<"--------------------------------------------------\n";
}

int main()
{
	BackCache<int> bc{1,2,3,4,5};
	const BackCache<int>& cbc=bc;

	// Contructor
	Test("Constructor",bc, {1,2,3,4,5},false);

	// Assign
	// Range is set to full.
	{
		bc.assign(4,12);
		Test("assign(size_type,value_type)",bc, {12,12,12,12},true);
		vector<int> other{4,5};
		bc.assign(other.begin(),other.end());
		Test("assign(it,it)",bc, {4,5},true);
		bc.assign({1,2,3});
		Test("assign(initlist)",bc, {1,2,3},true);
	}
	// Erase
	// Range boundaries are corrected.
	{
		bc= {4,1,2,3,5,6,7}; //bc=DirtVector(initialierlist<int>)
		bc[1]=-1;
		bc.Info();
		bc.erase(bc.begin()+1);
		Test("erase",bc, {4,2,3,5,6,7},true);
	}
	// Insert
	{
	}
	// Pop
	{
	}
	// Push
	{
	}
	// Resize
	{
	}
	// Swap
	{
	}
	// Operator[]
	{
	}
	// Operator=DirtVector: Copies the range state.
	{
		BackCache<int> foo{9,8,7,6,5};
		bc=foo;
		Test("operator=bc",bc, {9,8,7,6,5},false);
	}
	// Operator=vector
	/*
		{
			vector<int> bla{3,2,1,0};
			bc=bla;
			Test("operator=V",bc,{3,2,1,0},true);
		}*/
	return 0;
}


