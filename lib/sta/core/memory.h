#ifndef _STA_MEMORY_API_
#define _STA_MEMORY_API_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "sta.h"
#include "utility.h"
#include "iterator.h"
#include "functional.h"

BEGIN_NP_BLOCK

// Simple malloc wrapper
[[deprecated("Use 'new' keyword or one of the sta pointer types instead!")]]
auto allocate = function<void*(size_t)> {
	[](size_t __SIZE) {
		return malloc(__SIZE);
	}
};

// Simple realloc wrapper
[[deprecated("Use 'new' keyword or one of the sta pointer types instead!")]]
auto reallocate = function<void*(void*, size_t)> {
	[](void* __PTR, size_t __SIZE) {
		return realloc(__PTR, __SIZE);
	}
};

// Simple free wrapper
[[deprecated("Use 'new' keyword or one of the sta pointer types instead!")]]
auto clear = function<void(void*)> {
	[](void* __PTR) {
		free(__PTR);
		return;
	}
};

template <class T> class allocator;
// Specialize for void:

template <> 
class _STAXXEXPORT allocator<void> {
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void  value_type;
	template <class U> struct rebind { typedef allocator<U> other; };
};

template <class T> 
class _STAXXEXPORT allocator {
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef T* pointer;
	typedef const T* const_pointer;

	typedef T& reference;
	typedef const T& const_reference;
public:
	pointer address(reference r) const { return &r; }
	const_pointer address(const_reference r) const { return &r; }
public:
	allocator() _STAXX_USE_NOEXCEPT {}
	template <class U> allocator(const allocator<U>&) _STAXX_USE_NOEXCEPT;
	~allocator() _STAXX_USE_NOEXCEPT {}
public:
	//Space for n Ts
	pointer allocate(size_type n, typename allocator<void>::const_pointer = 0) {
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}
	void deallocate(pointer p, size_type) {
		::operator delete(p);
	}

	//Use placement new to engage the constructor
	inline void construct(pointer p, const T& val) { new(static_cast<void*>(p)) T(val); }
	inline void destroy(pointer p) { (static_cast<T*>(p))->~T(); }	//Call destructor

	size_type max_size() const _STAXX_USE_NOEXCEPT;
	template<class U> struct rebind { typedef allocator<U> other; };
};

template <class Out, class T> 
class _STAXXEXPORT raw_storage_iterator
	: public iterator<output_iterator_tag, void, void, void, void>
{
	Out p;

public:
	explicit raw_storage_iterator(Out pp) : p(pp) {  }
	raw_storage_iterator& operator*() { return *this; }
	raw_storage_iterator& operator=(const T& val) {
		T* pp = &*p;
		new(pp) T(val);
		return *this;
	}

	raw_storage_iterator& operator++() { ++p; return *this; }
	raw_storage_iterator operator++(int) {
		raw_storage_iterator t = *this;
		++p;
		return t;
	}
};

template <class T> _STAXXEXPORT 
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t n) {
	pair<T*, ptrdiff_t> retval;
	retval.first = static_cast<T*>(allocate(n * sizeof(T)));
	if (retval.first == 0) 
		retval.second = 0;
	else 
		retval.second = n;
	
	return retval;
}

template <class T> 
_STAXXEXPORT void return_temporary_buffer(T* p) {
	clear(p);
}

/*
This class template provides a limited garbage collection facility for pointers, 
by allowing pointers to have the elements they point to automatically destroyed when the auto_ptr object is itself destroyed.

auto_ptr objects have the peculiarity of taking ownership of the pointers assigned to them:
An auto_ptr object that has ownership over one element is in charge of destroying the element 
it points to and to deallocate the memory allocated to it when itself is destroyed. The destructor does this by calling operator delete automatically.

Therefore, no two auto_ptr objects should own the same element,
since both would try to destruct them at some point.
When an assignment operation takes place between two auto_ptr objects,
ownership is transferred, which means that the object losing ownership is set to no longer point to the element (it is set to the null pointer).
*/
template <class T> 
class 
[[deprecated("This class template is deprecated. unique_ptr is a new facility with a similar functionality, but with improved security (no fake copy assignments), added features (deleters) and support for arrays.")]]
_STAXXEXPORT auto_ptr {
public:
	typedef T element_type;
private:
	T* object;
	template <class Y> struct auto_ptr_ref {
		Y* p;
	};
public:
	explicit auto_ptr(T* p = 0) _STAXX_USE_NOEXCEPT : object(p) {  }
	auto_ptr(auto_ptr& p) _STAXX_USE_NOEXCEPT : object(p.release()) { }
	auto_ptr(auto_ptr_ref<T> r) _STAXX_USE_NOEXCEPT : object(r.p) {
		r.p = 0;
	}

	template<class Y> 
	auto_ptr(auto_ptr<Y>& p) _STAXX_USE_NOEXCEPT : object(p.release()) { }

	~auto_ptr() {
		delete object;
	}
public:

	inline void _unsafe_set(T* _ptr) {
		this->object = _ptr;
	}

	inline T* _unsafe_get() const _STAXX_USE_NOEXCEPT {
		return object;
	}

	T* release() _STAXX_USE_NOEXCEPT {
		T* temp(object);
		object = 0;
		return temp;
	}

	void reset(T* p = 0) _STAXX_USE_NOEXCEPT {
		if (p != object) {
			delete object;
			object = p;
		}
	}

public:
	template<class Y> 
	operator auto_ptr_ref<Y>() _STAXX_USE_NOEXCEPT {
		auto_ptr_ref<Y> retval;
		retval.p = this->object;
		this->object = 0;
		return retval;
	}

	template<class Y> 
	operator auto_ptr<Y>() _STAXX_USE_NOEXCEPT {
		auto_ptr<Y> retval(this->object);
		this->object = 0;
		return retval;
	}

	auto_ptr& operator=(auto_ptr& p) _STAXX_USE_NOEXCEPT {
		if (&p == this) {
			return *this;
		}
		delete this->object;
		this->object = p.release();
		return *this;
	}

	template<class Y> 
	auto_ptr& operator=(auto_ptr<Y>& p) _STAXX_USE_NOEXCEPT {
		if (&p == this) {
			return *this;
		}
		delete this->object;
		this->object = p.release();
		return *this;
	}

	T& operator*() const _STAXX_USE_NOEXCEPT {
		return *this->object;
	}

	T* operator->() const _STAXX_USE_NOEXCEPT {
		return this->object;
	}

	operator bool() const { 
		return this->object != nullptr;
	}
};

template <class T, typename... Args>
auto_ptr<T> make_auto(Args... args) {
	return auto_ptr<T>(new T(args...));
}

/*
These objects have the ability of taking ownership of a pointer:
 once they take ownership they manage the pointed object by becoming responsible for its deletion at some point.

unique_ptr objects automatically delete the object they manage (using a deleter) as soon as they themselves are destroyed,
or as soon as their value changes either by an assignment operation or by an explicit call to unique_ptr::reset.

unique_ptr objects own their pointer uniquely: no other facility shall take care of deleting the object,
and thus no other managed pointer should point to its managed object, since as soon as they have to,
unique_ptr objects delete their managed object without taking into account whether other pointers still point to the same object or not,
and thus leaving any other pointers that point there as pointing to an invalid location.
*/
template<typename T> 
class _STAXXEXPORT unique_ptr {
public: // CON-/DESTRUCTORS
	unique_ptr() _STAXX_USE_NOEXCEPT : unique_ptr(nullptr) {}
	unique_ptr(T* t) _STAXX_USE_NOEXCEPT : _ptr(t) {}

	template <class OtherType>
	unique_ptr(unique_ptr<OtherType>& other) _STAXX_USE_NOEXCEPT : _ptr(other._unsafe_get()) {
		other._unsafe_set(nullptr);
	}

	template <class OtherType>
	unique_ptr(unique_ptr<OtherType>&& other) _STAXX_USE_NOEXCEPT : _ptr(other._unsafe_get()) {
		other._unsafe_set(nullptr);
	}

	~unique_ptr() {
		delete _ptr;
	}
public:

	inline void _unsafe_set(T* _ptr) {
		this->_ptr = _ptr;
	}

	inline T* _unsafe_get() const {
		return _ptr;
	}

	T* release() _STAXX_USE_NOEXCEPT {
		T* temp(_ptr);
		_ptr = 0;
		return temp;
	}

	void reset(T* p = 0) _STAXX_USE_NOEXCEPT {
		if (p != _ptr) {
			delete _ptr;
			_ptr = p;
		}
	}

private:
	T* _ptr;
public: // OPERATORS
	unique_ptr<T>* operator=(T* t) {
		if (_ptr) delete _ptr;
		_ptr = t;
		return this;
	}

	template <class OtherType>
	unique_ptr<T>* operator=(unique_ptr<OtherType>& other) {
		if (_ptr) delete _ptr;
		_ptr = other._unsafe_get();
		other._unsafe_set(nullptr);
		return this;
	}

	T& operator*() const _STAXX_USE_NOEXCEPT {
		return *this->_ptr;
	}


	T* operator->() const _STAXX_USE_NOEXCEPT {
		return _ptr;
	}

	operator bool() {
		return _ptr != nullptr;
	}
};

template <class T, typename... Args>
unique_ptr<T> make_unique(Args... args) {
	return unique_ptr<T>(new T(args...));
}

// Non owning reference
template <class T> 
class _STAXXEXPORT ref_ptr {
public: // CON-/DESTRUCTORS
	ref_ptr() : _ref(nullptr) {}
	ref_ptr(T* ref) : _ref(ref) {}
	ref_ptr(T& ref) : _ref(&ref) {}
	ref_ptr(const ref_ptr<T>& other) : _ref(other._ref) {}

	template <class BaseClass>
	ref_ptr(ref_ptr<BaseClass>& other) : _ref((T*) *other) {}
public: // HELPER FUNCTIONS

	inline void _unsafe_set(T* _ptr) {
		this->_ref = _ptr;
	}

	inline T* _unsafe_get() const {
		return _ref;
	}

	T* release() _STAXX_USE_NOEXCEPT {
		T* temp(_ref);
		_ref = 0;
		return _ref;
	}

	void reset(T* p = 0) _STAXX_USE_NOEXCEPT {
		if (p != _ref) {
			delete _ref;
			_ref = p;
		}
	}
public: // OPERATORS
	ref_ptr<T>& operator=(const ref_ptr<T>& other) { 
		_ref = other._ref;
		return *this;
	}

	T* operator->() _STAXX_USE_NOEXCEPT { return _ref; }
	const T& operator->() const _STAXX_USE_NOEXCEPT { return *_ref; }

	T* operator*() _STAXX_USE_NOEXCEPT  { return _ref; }
	
	operator bool() const { return _ref != nullptr; }
private: // ATTRIBUTES
	T *_ref;
};

template <class T, typename... Args>
ref_ptr<T> make_ref(Args... args) {
	return ref_ptr<T>(new T(args...));
}

template <class T>
using shared_ptr = ref_ptr<T>;

// template < class T, typename ... Args>
// using make_shared = make_ref<T, Args>;

#pragma GCC diagnostic pop

END_NP_BLOCK

#endif