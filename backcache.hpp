#pragma once
#include <array>
#include <limits>

#define DEBUG_BC

template<class T,std::size_t N>
class BackCache: protected std::array<T,N>
{
	typedef std::array<T,N> Base;
public:
	using typename Base::reference;
	using typename Base::size_type;

	/*
		Overriden methods are commented out
	*/
	using Base::at;
	using Base::back;
	using Base::begin;
	using Base::cbegin;
	using Base::cend;
	using Base::crbegin;
	using Base::crend;
	using Base::data;
	using Base::empty;
	using Base::end;
	using Base::fill;
	using Base::front;
	using Base::max_size;
	using Base::operator[]; // Using base's const version
	using Base::rbegin;
	using Base::rend;
	using Base::size;
	using Base::swap;

	reference operator[](size_type n) noexcept
    {
		_range.Add(n);
		return Base::operator[](n);
	}

private:
	class Range
	{
	public:
		void Add(size_type n)
		{
			if (n<_from) _from=n;
			if (n>_to) _to=n;
#ifdef DEBUG_BC
			std::cout<<"Add("<<n<<")\n";
#endif
		}
		void Set(size_type f,size_type t)
		{
#ifdef DEBUG_BC
			std::cout<<"SetRange("<<f<<", "<<t<<")\n";
#endif
			if (f<_from) _from=f;
			if (t>_to) _to=t;
			if (_to>=size()) _to=size()-1;// force
#ifdef DEBUG_BC
			std::cout<<"RANGE is now "<<_from<<" to "<<_to<<"\n";
#endif
		}
		void SetFull()
		{
			if (!empty())
				SetRange(0,size()-1);
		}

	private:
		// Alias to biggest size_t.
		static constexpr size_type MAXSIZE=std::numeric_limits<size_type>::max();
		// Dirty range. Class member initialization used so we don't have to
		// rewrite all the constructors.
		size_type _from{MAXSIZE},_to{0};
	} _range{};
};
