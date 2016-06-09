/*******************************************************************************

	A vector that keeps track of changes.

*******************************************************************************/
#pragma once

#include <limits>
#include <vector>

#define DEBUG_BB
#ifdef DEBUG_BB
#include <iostream>
#define PNAME std::cout<<"Call to "<<__FUNCTION__<<'\n';
#else
#define PNAME
#endif

template<class T,class Ta=std::allocator<T>>
class BackCache: protected std::vector<T,Ta>
{
	// =========================================================================
	// Vector
	// =========================================================================
	typedef std::vector<T,Ta> Base;

public:
	using typename Base::reference;
	using typename Base::const_reference;
	using typename Base::iterator; //make const too!
	using typename Base::const_iterator;
	using typename Base::size_type;
	using typename Base::value_type;

	/*
		This is protected inheritance so we need to redeclare functions as
		public, with using directives.
	*/

	using Base::Base; // Constructors

	/*
		Overriden methods are commented out
	*/

	//    Base::assign;
	using Base::at;
	using Base::back;
	using Base::begin;
	using Base::capacity;
	using Base::cbegin;
	using Base::cend;
	using Base::clear;
	using Base::crbegin;
	using Base::crend;
	using Base::data;
	//using Base::emplace;
	// using Base::emplace_back;
	using Base::empty;
	using Base::end;
	//    Base::erase;
	using Base::front;
	//    Base::insert;
	using Base::max_size;
	//    Base::operator=;
	using Base::operator[]; // Using base's const version
	//    Base::pop_back;
	//    Base::push_back;
	using Base::rbegin;
	using Base::rend;
	using Base::reserve;
	//    Base::resize;
	using Base::shrink_to_fit;
	using Base::size;
	//    Base::swap;

	// Assign

	void assign(size_type n,const value_type& val)
	{
		Base::assign(n,val);
		SetFull();
	}

	void assign(std::initializer_list<value_type> initlist)
	{
		Base::assign(initlist);
		SetFull();
	}

	template<typename Ti>
	void assign(Ti first, Ti last)
	{
		Base::assign(first,last);
		SetFull();
	}

	// Erase. Fixes the range.

	iterator erase(iterator pos)
	{
		PNAME
		iterator it=Base::erase(pos);
		_range.Set(it-begin(),size()-1);
		return it;
	}

	iterator erase(iterator first, iterator last)
	{
		PNAME
		iterator it=Base::erase(first,last);
		_range.Set(first-begin(),size()-1);
		return it;
	}

	// Insert

	iterator insert(iterator pos, const T& x)
	{
		PNAME
		iterator it=Base::insert(pos,x);
		_range.Set(pos-begin(),size()-1);
		return it;
	}

	void insert(iterator pos,std::initializer_list<T> l)
	{
		PNAME
		Base::insert(pos,l);
		_range.Set(pos-begin(),size()-1);
	}

	void insert(iterator pos, size_type n, const T& x)
	{
		PNAME
		Base::insert(pos,n,x);
		_range.Set(pos-begin(),size()-1);
	}

	template<typename InputIterator>
	void insert(iterator pos, InputIterator first,
				InputIterator last)
	{
		PNAME
		Base::insert(pos,first,last);
		_range.Set(pos-begin(),size()-1);
	}

	// Operators

	BackCache& operator=(const BackCache& o)
	{
		Base::operator=(o);
		_range=o._range;
		return *this;
	}

	/*DirtVector& operator=(const Base& o)
	{
		Base::operator=(o);
		SetFullRange();
		return *this;
	}*/

	reference operator[](size_type n)
	{
		PNAME
		_range.Add(n);
		return Base::operator[](n);
	}

	// Push/Pop

	void pop_back()
	{
		PNAME
		Base::pop_back();
		_range.Set(_range.From(),size()-1);

	}

	void push_back(const T& x)
	{
		PNAME
		Base::push_back(x);
		_range.Set(size()-1,size()-1);
	}

	void push_back(T&& x)
	{
		PNAME
		Base::push_back(x);
		_range.Set(size()-1,size()-1);
	}

	// Resize. Fix the range.
	void resize(size_type ns)
	{
		PNAME
		Base::resize(ns);
		_range.Set(_range.From(),std::min(_range.To(),size()-1));
	}

	// Swapping changes everything
	void swap(Base& x)
	{
		Base::swap(x);
		_range.Set(0,size()-1);
	}

	// =========================================================================
	// DirtVector
	// =========================================================================
public:
	size_type RangeSize() const
	{

		return _range.Size();
	}

	bool Pending() const
	{
		return _range.Pending();
	}

	void Reset()
	{
		PNAME
		_range.Reset();
	}

	void SetFull()
	{
		if (!empty())
			_range.Set(0,size()-1);
	}

	void Info() const
	{
#ifdef DEBUG_BB
		std::cout<<"Info: ";
		if (Pending())
		{
			std::cout<<"Pending=true; ";
		}
		else
		{
			std::cout<<"Pending=false; ";
		}
		std::cout<<"Range=["<<_range.From()<<"-"<<_range.To()<<"]; ";
		std::cout<<"Content=";
		for (const T& x: *this)
		{
			std::cout<<x<<", ";
		}
		std::cout<<std::endl;
#endif
	}

private:
	class Range
	{
	public:
		void Add(size_type n)
		{
			if (n<_from) _from=n;
			if (n>_to) _to=n;
#ifdef DEBUG_BB
			std::cout<<"Add("<<n<<")\n";
#endif
		}
		size_type From() const
		{
			return _from;
		}
		bool Pending() const
		{
			return _from<=_to;
		}
		void Reset()
		{
			_from=MAXSIZE;
			_to=0;
		}
		void Set(size_type f,size_type t)
		{
#ifdef DEBUG_BB
			std::cout<<"SetRange("<<f<<", "<<t<<")\n";
#endif
			if (f<_from) _from=f;
			if (t>_to) _to=t;
#ifdef DEBUG_BB
			std::cout<<"RANGE is now "<<_from<<" to "<<_to<<"\n";
#endif
		}

		size_type Size() const
		{
			if (!Pending()) return 0;
			return _to-_from+1;
		}
		size_type To() const
		{
			return _to;
		}
	private:
		// Alias to biggest size_t.
		static constexpr size_type MAXSIZE=std::numeric_limits<size_type>::max();
		// Dirty range.
		size_type _from{MAXSIZE},_to{0};
	} _range{};
	/*
		Class member initialization.used so we don't have to
		rewrite all the constructors.
	*/
};

// Compare to normal vector
template<typename _Tp, typename _Alloc>
inline bool operator==(const BackCache<_Tp, _Alloc>& __x,
					   const std::vector<_Tp, _Alloc>& __y)
{
	return (__x.size() == __y.size()
			&& std::equal(__x.begin(), __x.end(), __y.begin()));
}

template<typename _Tp, typename _Alloc>
inline bool operator==(const std::vector<_Tp, _Alloc>& __x,
					   const BackCache<_Tp, _Alloc>& __y)
{
	return (__x.size() == __y.size()
			&& std::equal(__x.begin(), __x.end(), __y.begin()));
}
