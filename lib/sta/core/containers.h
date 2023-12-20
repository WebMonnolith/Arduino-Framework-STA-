#ifndef _STA_CONTAINERS_
#define _STA_CONTAINERS_

#pragma GCC visibility push(default)
#pragma GCC diagnostic ignored "-Wconversion-null"
#pragma GCC diagnostic ignored "-Wsign-compare"

#include "sta.h"
#include "memory.h"
#include "cmath.h"

BEGIN_NP_BLOCK

template <typename T, size_t C>
struct _STAXXEXPORT data {
public:
	inline T& operator [] (uint32 index) {
		if (index < C)
			return this->__data[index];
		return NULL;
	} 

	inline T operator [] (uint32 index) const {
		if (index < C)
			return this->__data[index];
		return NULL;
	} 
public:
	T __data[C];
};

template <typename T>
struct _STAXXLOCAL array_traits {
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef pointer iterator;
	typedef const_pointer const_iterator;
	typedef size_t size_type;
};

template<class T>
class _STAXXEXPORT initializer_list {
public:
    // default constructor
    initializer_list() : array(NULL), len(0) {}

    size_t size() const {
        return this->len;
    }

    const T* begin() {
        return this->array;
    }

    const T* end() {
        return this->array + this->len;
    }

private:
    const T* array;
    size_t len;
private:
    // Initialize from a { ... } construct
    initializer_list(const T* a, size_t l) : array(a), len(l) { }
};

#define ScaleFactor 1.75;

template <class Ty>
class _STAXXEXPORT vector {
public:
	typedef unsigned Size;
public:
	vector(Size initialCpacity = 5) {
		firstElement = afterLastElement = new Ty[initialCpacity];
		capacity = initialCpacity;
	}

	template <size_t C>
	vector(const data<Ty, C>& __data) {
		for (int32 i = 0; i < C; i++) 
			this->push(__data[i]);
	}

	~vector() {
		dispose();
	}
public:

	void push(Ty& item) {
		if (size() == capacity) {
			resize();
		}
		*afterLastElement = item;
		afterLastElement++;
	}

	void push(Ty&& item) {
		if (size() == capacity) {
			resize();
		}
		*afterLastElement = item;
		afterLastElement++;
	}

	Ty pop() {
		//if(empty()) throw "Vector is empty, cant pop()";
		auto retVal = *(afterLastElement - 1);
		afterLastElement -= 1;
		return retVal;
	}

	Ty& get(Size index) {
		//if(index < size()) throw "Vector index is bad, probably not less than size()";
		return *(firstElement + index);
	}
	Size size() const {
		return afterLastElement - firstElement;
	}

	bool empty() const {
		return size() == 0;
	}
private:
	Ty* firstElement;
	Ty* afterLastElement;
	Size capacity;

	void resize() {
		Size newCapacity = capacity * ScaleFactor;
		Ty *newFirst, *newAfterLast;
		newFirst = newAfterLast = new Ty[newCapacity];

		for (Size i = 0; i < capacity; i++) {
			*newAfterLast = get(i);
			++newAfterLast;
		}

		dispose();

		capacity = newCapacity;
		firstElement = newFirst;
		afterLastElement = newAfterLast;
	}

	void dispose() {
		delete[] firstElement;
	}
};


template <typename T, size_t N>
class _STAXXEXPORT array {

public:
	array() = default;

	array(const data<T, N>& __data) {
		for (int32 i = 0; i < N; i++) 
			_data[i] = __data[i];
	}

	array(sta::initializer_list<T> init) {
		if (init.size() != N) {
			// error
		}
		size_t i = 0;
		for (const auto& item : init) {
			_data[i++] = item;
		}
	}

public:
	typename array_traits<T>::reference front() { return _data[0]; }
	typename array_traits<T>::const_reference front() const { return _data[0]; }
	typename array_traits<T>::reference back() { return _data[N - 1]; }
	typename array_traits<T>::const_reference back() const { return _data[N - 1]; }
	typename array_traits<T>::pointer data() _STAXX_NOEXCEPT { return _data; }
	typename array_traits<T>::const_pointer data() const _STAXX_NOEXCEPT { return _data; }
	typename array_traits<T>::reference at(size_t pos) { return _data[pos]; }
	typename array_traits<T>::const_reference at(size_t pos) const { return _data[pos]; }

	typename array_traits<T>::iterator begin() _STAXX_NOEXCEPT { return _data; }
	typename array_traits<T>::const_iterator begin() const _STAXX_NOEXCEPT { return _data; }
	typename array_traits<T>::const_iterator cbegin() const _STAXX_NOEXCEPT { return _data; }
	typename array_traits<T>::iterator end() _STAXX_NOEXCEPT { return _data + N; }
	typename array_traits<T>::const_iterator end() const _STAXX_NOEXCEPT { return _data + N; }
	typename array_traits<T>::const_iterator cend() const _STAXX_NOEXCEPT { return _data + N; }

	bool empty() const _STAXX_NOEXCEPT { return begin() == end(); }
	typename array_traits<T>::size_type size() const _STAXX_NOEXCEPT { return N; }
	typename array_traits<T>::size_type max_size() const _STAXX_NOEXCEPT { return N; }

	void fill(typename array_traits<T>::const_reference value) {
		for (auto i = 0u; i < N; ++i) {
			_data[i] = value;
		}
	}
private:
	typename array_traits<T>::value_type _data[N ? N : 1];
public:
	array& operator=(const array& other) = default;

	inline typename array_traits<T>::reference operator[](size_t i) { return _data[i]; }
	inline typename array_traits<T>::const_reference operator[](size_t i) const { return _data[i]; }
};

END_NP_BLOCK

#endif