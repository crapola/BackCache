#include "backcache.hpp"
#include <cassert>
#include <iostream>

using namespace std;

static bool success=true;

void TestResult(const BackCache<int>& tested,bool test)
{
	if (test)
		cout<<"OK\n";
	else
	{
	cout<<"\n***\n!!! ERROR !!!\n***\n";
	success=false;
	}

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
			   (tested.From()==range.first&&tested.To()==range.second));

}

int main()
{
	BackCache<int> bc{1,2,3,4,5};
	const BackCache<int>& cbc=bc;

	// Contructor
	// Range is reset/false on construction.
	Test("Constructor",bc, {1,2,3,4,5},false);

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
	// Insert
	{
		bc={1,2,3};
		const int v=0;
		bc.insert(bc.begin(),v);
		Test("insert(const_iter,const T&)",bc,{0,1,2,3},{0,3});
		bc.clear();
		bc={1,2,3,4,5};
		bc[1]=1;
		bc.insert(bc.begin()+3,9);
		Test("insert(const_iter,T&&)",bc,{1,1,3,9,4,5},{1,5});
		bc.Reset();
		bc.insert(bc.begin()+2,{8,7,6});
		Test("insert(const_iter,initlist)",bc,{1,1,8,7,6,3,9,4,5},{2,8});
		bc.clear();
		bc.insert(bc.end(),{5,4,3});
		Test("insert(const_iter,initlist)",bc,{5,4,3},{0,2});
		bc.Reset();
		Test("reset",bc,{5,4,3},false);
		bc.insert(bc.begin()+2,5,7);
		Test("insert(const_iter,size_t,const T&)",bc,{5,4,7,7,7,7,7,3},{2,7});
		bc.insert(bc.begin()+1,bc.begin(),bc.begin()+3);
		Test("insert(const_iter,inputiter,inputiter)",bc,{5,5,4,7,4,7,7,7,7,7,3},{1,10});
	}
	// Operator=
	{
		BackCache<int> other(12,1);
		other[9]=9;
		other[2]=2;
		other[4]=4;
		Test("operator[]",other,{1,1,2,1,4,1,1,1,1,9,1,1},{2,9});
		other.Info();
		bc=other;
		Test("operator=(BackCache&)",bc,{1,1,2,1,4,1,1,1,1,9,1,1},{2,9});
		// TODO: add =vector..?
	}
	// Operator[]
	{
		bc.Reset();
		int foo=bc[7]; // This is considered a write!
		Test("operator[]",bc,{1,1,2,1,4,1,1,1,1,9,1,1},{7,7});
		// Must use const container to read only
		const BackCache<int>& cbc=bc;
		const int& bar=cbc[6];
		Test("operator[]",bc,{1,1,2,1,4,1,1,1,1,9,1,1},{7,7});
		Test("operator[]",cbc,{1,1,2,1,4,1,1,1,1,9,1,1},{7,7});
		assert(&bc==&cbc);
		// Or use at() to guarantee read only
		const int& value=bc.at(3);
		Test("at",bc,{1,1,2,1,4,1,1,1,1,9,1,1},{7,7});
	}
	// Pop
	{
		//TODO
	}
	// Push
	{
	}

	cout<<(success?"Everything is OK":"Mistakes were made")<<endl;
	return 0;
}


