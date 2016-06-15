#include "backcache.hpp"
#include <iostream>

using namespace std;

void TestResult(const BackCache<int>& tested,bool test)
{
	if (test)
		cout<<"OK\n";
	else
		cout<<"\n***\n!!! ERROR !!!\n***\n";
	tested.Info();
	cout<<"--------------------------------------------------\n";
}

void Test(const char* what,const BackCache<int>& tested,const vector<int> expected,bool pending)
{
	cout<<what<<": ";
	TestResult(tested,tested==expected&&tested.Pending()==pending);
}

void Test(const char* what,const BackCache<int>& tested,const vector<int> expected,const pair<size_t,size_t> range)
{
	cout<<what<<": ";
	TestResult(tested,(tested==expected)&&
			   (tested.RangeFrom()==range.first&&tested.RangeTo()==range.second));

}

int main()
{
	BackCache<int> bc{1,2,3,4,5};
	const BackCache<int>& cbc=bc;

	// Contructor
	// Range is reset/false on construction.
	Test("Constructor",bc, {1,2,3,4,5},false);

	/* Iterators
		Only const iterators allowed.
		at
		begin
	...

	*/
	{
		BackCache<int>::const_iterator it=bc.begin();
		// Should cause error:
		//BackCache<int>::iterator it1=bc.begin();
		//vector<int>::iterator it2=bc.begin();
		//auto it3=bc.begin(); // Give vector const it
		//*it3=1000;
		bc.Info();
	}
	// At
	// Only const reference
	{
		// Error:
		/*
		int& foo=bc.at(0);
		foo=1111;
		bc.Info();
		*/
	}
	// Assign
	// Range is set to full.
	{
		bc.assign(4,12);
		Test("assign(size_type,value_type)",bc, {12,12,12,12}, {0,3});
		vector<int> other{4,5,6};
		bc.assign(other.begin(),other.end());
		Test("assign(it,it)",bc, {4,5,6}, {0,2});
		// Assigning nothing resets the range.
		bc.assign(other.begin()+1,other.begin()+1);
		Test("assign(it,it) where it's are equal",bc, {},false);
		bc.assign({1,2,3});
		Test("assign(initlist)",bc, {1,2,3}, {0,2});
		bc.assign({});
		Test("assign(empty initlist)",bc, {},false);
	}
	// Erase
	// Range enlarged and boundaries are corrected as needed.
	{
		bc= {4,1,2,3,5,6,7}; //bc=BackCache(initialierlist<int>)
		bc[1]=-1;
		Test("op[]",bc, {4,-1,2,3,5,6,7}, {1,1});
		bc.erase(bc.begin()+1);
		Test("erase(iterator)",bc, {4,2,3,5,6,7}, {1,5});
		bc.erase(bc.begin()+1,bc.begin()+3);
		Test("erase(iterator,iterator)",bc, {4,5,6,7}, {1,3});
		// NO OP does nothing.
		bc.erase(bc.begin()+2,bc.begin()+2);
		Test("erase(it,it) when equal (NOP)",bc, {4,5,6,7}, {1,3});
		// Erase all
		bc.erase(bc.begin(),bc.end());
		Test("erase(it,it) everything",bc, {},false);
		bc.push_back(9);
		bc.erase(bc.begin());
		Test("erase(it) last element",bc, {},false);
		// Check if keep previous state correctly
		bc= {5,0,0,5,5,5,5,5};
		bc[1]=5;
		Test("...",bc, {5,5,0,5,5,5,5,5}, {1,1});
		bc.erase(bc.begin()+3,bc.end());
		Test("erase(it,it) keep previous range",bc, {5,5,0}, {1,2});
		//
		bc= {1,2,3,3};
		bc[3]=4;
		bc.erase(bc.begin());
		Test("erase(it) special case",bc, {2,3,4}, {0,2});
	}
	// Clear
	// Range reset.
	{
		bc.clear();
		Test("clear",bc, {},false);
	}
	// Data
	// Non const access should error.
	{
		bc= {1,2,3};
		// int* dat=bc.data(); // error
		const int* dat=bc.data(); // ok
		std::cout<<"Data "<<dat[0]<<"\n";
		// dat[0]=9;// <-- error...
		bc.Info();
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


