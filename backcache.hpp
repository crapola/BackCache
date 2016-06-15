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
	using typename Base::const_pointer;
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
	//    Base::clear;
	using Base::crbegin;
	using Base::crend;
	//    Base::data;
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

	// Clear

	void clear()
	{
		Base::clear();
		_range.Reset();
	}

	// Data

	const_pointer data() const
	{
		return Base::data();
	}

	// Erase. Fixes the range.

	iterator erase(iterator pos)
	{
		PNAME
		iterator it=Base::erase(pos);
		if (empty())
			_range.Reset();
		else
		{
			_range.Add(it-begin(),size()-1);
		}
		return it;
	}

	iterator erase(iterator first, iterator last)
	{
		iterator it=Base::erase(first,last);
		if (empty())
			_range.Reset();
		else if (first!=last)
		{
			_range.Add(first-begin());
			_range.Cap(size()-1);
		}
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

	/*BackCache& operator=(const Base& o)
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
		_range.Add(size()-1,size()-1);
	}

	void push_back(T&& x)
	{
		PNAME
		Base::push_back(x);
		_range.Add(size()-1,size()-1);
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
	// BackCache
	// =========================================================================
public:
	size_type RangeSize() const
	{

		return _range.Size();
	}

	bool Pending() const
	{
		return _range;
	}

	void Reset()
	{
		_range.Reset();
	}

	void SetFull()
	{
		if (!empty())
			_range.Set(0,size()-1);
		else
			_range.Reset();
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
		// Add elements to current range (boolean union).
		void Add(size_type n)
		{
			if (n<_from) _from=n;
			if (n>_to) _to=n;
#ifdef DEBUG_BB
			std::cout<<"Add("<<n<<")\n";
#endif
		}
		void Add(size_type f,size_type t)
		{
#ifdef DEBUG_BB
			std::cout<<"AddRange("<<f<<", "<<t<<")\n";
#endif
			if (f<_from) _from=f;
			if (t>_to) _to=t;
#ifdef DEBUG_BB
			std::cout<<"RANGE is now "<<_from<<" to "<<_to<<"\n";
#endif
		}
		// Cap end of range to s
		void Cap(size_type s)
		{
			_to=(_to>s)?s:_to;
		}
		size_type From() const
		{
			return _from;
		}
		void Reset()
		{
			_from=MAXSIZE;
			_to=0;
		}
		// Redefine the range, ingoring previous state.
		void Set(size_type f,size_type t)
		{
#ifdef DEBUG_BB
			std::cout<<"SetRange("<<f<<", "<<t<<")\n";
#endif
			_from=f;
			_to=t;
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
		operator bool() const // True if pending, valid range.
		{
			return _from<=_to;
		}
	private:
		// Alias to biggest size_t.
		static constexpr size_type MAXSIZE=std::numeric_limits<size_type>::max();
		// Range.
		size_type _from{MAXSIZE},_to{0};
	} _range{};
	/*
		Class member initialization.used so we don't have to
		rewrite all the constructors.
	*/
#ifdef DEBUG_BB
public:
	size_t RangeFrom() const
	{
		return _range.From();
	}
	size_t RangeTo() const
	{
		return _range.To();
	}
#endif
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
