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

void Test(const char* what,BackCache<int>& tested,const vector<int> expected,const pair<size_t,size_t> range)
{
	cout<<what<<": ";

	bool test=(tested==expected)&&
			  (tested.RangeFrom()==range.first&&tested.RangeTo()==range.second);
	if (test)
		cout<<"OK\n";
	else
		cout<<"!!! ERROR !!!\n";
	tested.Info();
	cout<<"--------------------------------------------------\n";
}

int main()
{
	BackCache<int> bc{1,2,3,4,5};
	const BackCache<int>& cbc=bc;

	// Contructor
	// Range is reset/false o n construction.
	Test("Constructor",bc, {1,2,3,4,5},false);

	// Assign
	// Range is set to full.
	{
		bc.assign(4,12);
		Test("assign(size_type,value_type)",bc, {12,12,12,12},{0,3});
		vector<int> other{4,5};
		bc.assign(other.begin(),other.end());
		Test("assign(it,it)",bc, {4,5},{0,1});
		bc.assign({1,2,3});
		Test("assign(initlist)",bc, {1,2,3},{0,2});
	}
	// Erase
	// Range enlarged and boundaries are corrected as needed.
	{
		bc= {4,1,2,3,5,6,7}; //bc=BackCache(initialierlist<int>)
		bc[1]=-1;
		Test("op[]",bc,{4,-1,2,3,5,6,7},{1,1});
		bc.erase(bc.begin()+1);
		Test("erase(iterator)",bc, {4,2,3,5,6,7},{1,5});
		bc.erase(bc.begin()+1,bc.begin()+3);
		Test("erase(iterator,iterator)",bc, {4,5,6,7},{1,3});
		bc.erase(bc.begin()+2,bc.begin()+2); // NO OP
		Test("erase(nothing)",bc, {4,5,6,7},{1,3});

		//bc.erase(bc.end());
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
	// Operator=BackCache: Copies the range state.
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


