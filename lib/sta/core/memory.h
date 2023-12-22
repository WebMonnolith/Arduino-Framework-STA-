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


template <class T> _STAXXEXPORT 
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t n);

template <class T> 
_STAXXEXPORT void return_temporary_buffer(T* p);

template <class T> class allocator;
// Specialize for void:

/**
 * @brief Specialization of the allocator class for void type.
 * 
 * This class template represents an allocator specifically designed for 
 * allocating memory for void type.
 */
template <> 
class _STAXXEXPORT allocator<void> {
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void  value_type;
	template <class U> struct rebind { typedef allocator<U> other; };
};

/**
 * @brief Generic allocator class template.
 * 
 * This class template represents a generic allocator that can allocate 
 * memory for any specified type T.
 * @tparam T The type for which memory is to be allocated.
 */
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

/**
 * @brief Raw storage iterator class template.
 * 
 * This class template represents an iterator that provides a way to iterate 
 * over uninitialized memory and store values of type T.
 * @tparam Out Output iterator type.
 * @tparam T   Type of elements the iterator can store.
 */
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

/**
 * @brief smart_ptr Interface
 * 
 * This interface defines the basic methods required for a smart pointer.
 * It serves as a blueprint for implementing custom smart pointer classes.
 * 
 * @tparam T Type of the managed object.
 */
template <class T> 
class _STAXXEXPORT smart_ptr {
public:
    /**
     * @brief Virtual destructor.
     * 
     * Ensures proper destruction of derived classes.
     */
    virtual ~smart_ptr() = default;
protected:
    /**
     * @brief Set the internal pointer to the given raw pointer.
     * 
     * This method assigns the managed pointer to the provided raw pointer.
     * 
     * @param _ptr Pointer to assign internally.
     */
    virtual inline void _unsafe_set(T* _ptr) = 0;

    /**
     * @brief Retrieve the internal raw pointer.
     * 
     * This method returns the raw pointer managed by the smart pointer.
     * 
     * @return T* Raw pointer managed by the smart pointer.
     */
    virtual inline T* _unsafe_get() const = 0;

    /**
     * @brief Release ownership of the managed pointer.
     * 
     * This method releases the ownership of the managed pointer and returns it.
     * After releasing ownership, the smart pointer no longer manages the object.
     * 
     * @return T* Released raw pointer.
     */
    virtual T* release() = 0;

    /**
     * @brief Reset the managed pointer to a new pointer or nullptr.
     * 
     * This method changes the managed pointer to a new pointer or nullptr,
     * optionally deallocating the previously managed object.
     * 
     * @param p New pointer to manage (default: nullptr).
     */
    virtual void reset(T* p = nullptr) = 0;

    // Add any additional virtual methods or functionalities if needed
    // virtual T& operator*() = 0;
    // virtual T* operator->() = 0;
};

/**
 * @brief This class template offers a basic garbage collection mechanism for pointers.
 *
 * The auto_ptr template class manages pointers, ensuring that the elements they point to are automatically
 * destroyed when the auto_ptr object itself is destroyed.
 *
 * auto_ptr objects have the distinctive feature of assuming ownership of the pointers assigned to them:
 * When an auto_ptr object takes ownership of an element, it becomes responsible for destroying the pointed-to element
 * and deallocating the memory allocated to it upon its own destruction. This destruction occurs automatically via the destructor
 * by invoking the operator delete.
 *
 * Consequently, it is important to ensure that no two auto_ptr objects possess ownership of the same element,
 * as both would attempt to destruct it at some point. During an assignment operation between two auto_ptr objects,
 * ownership is transferred. The object losing ownership ceases to point to the element and is set to the null pointer.
 * @tparam T Type of the managed object.
 */
template <class T>  
[[deprecated("This class template is deprecated. unique_ptr is a new facility with a similar functionality, but with improved security (no fake copy assignments), added features (deleters) and support for arrays.")]]
class _STAXXEXPORT auto_ptr : public smart_ptr<T> {
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

	inline void _unsafe_set(T* _ptr) override {
		this->object = _ptr;
	}

	inline T* _unsafe_get() const override {
		return object;
	}

	T* release() override {
		T* temp(object);
		object = 0;
		return temp;
	}

	void reset(T* p = 0) override {
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

/**
 * @brief Manages a pointer and its associated resource with unique ownership.
 * 
 * unique_ptr manages a pointer to a dynamically allocated object, ensuring that the object is 
 * deleted when the unique_ptr goes out of scope or is reset. It follows the RAII (Resource Acquisition Is Initialization) principle,
 * providing a safe and efficient way to handle dynamically allocated memory.
 * 
 * @tparam T Type of the managed object.
 */
template<typename T> 
class _STAXXEXPORT unique_ptr : public smart_ptr<T> {
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

	inline void _unsafe_set(T* _ptr) override {
		this->_ptr = _ptr;
	}

	inline T* _unsafe_get() const override {
		return _ptr;
	}

	T* release() override {
		T* temp(_ptr);
		_ptr = 0;
		return temp;
	}

	void reset(T* p = 0) override {
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

/**
 * @class ref_ptr
 * @brief A smart pointer class similar to std::shared_ptr that manages a pointer while tracking references.
 *
 * This class helps in automatic memory management by keeping track of the number of references to a dynamically allocated object.
 * It uses reference counting to ensure that the object is deallocated when the last reference to it is destroyed.
 *
 * @tparam T The type of the managed object.
 */
template <class T> 
class _STAXXEXPORT ref_ptr : public smart_ptr<T> {
public: // CON-/DESTRUCTORS
	ref_ptr() : _ref(nullptr) {}
	ref_ptr(T* ref) : _ref(ref) {}
	ref_ptr(T& ref) : _ref(&ref) {}
	ref_ptr(const ref_ptr<T>& other) : _ref(other._ref) {}

	template <class BaseClass>
	ref_ptr(ref_ptr<BaseClass>& other) : _ref((T*) *other) {}
public: // HELPER FUNCTIONS

	inline void _unsafe_set(T* _ptr) override {
		this->_ref = _ptr;
	}

	inline T* _unsafe_get() const override {
		return _ref;
	}

	T* release() override {
		T* temp(_ref);
		_ref = 0;
		return _ref;
	}

	void reset(T* p = 0) override {
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

/**
 * @brief Allocates temporary buffer memory for elements of type T.
 * 
 * @tparam T Type of elements in the buffer.
 * @param n Number of elements to allocate space for.
 * @return A pair containing a pointer to the allocated memory block and the size allocated.
 *         If allocation fails, the pointer in the pair will be null and size will be 0.
 */
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

/**
 * @brief Clears and releases a temporary buffer allocated by get_temporary_buffer.
 * 
 * @tparam T Type of elements in the buffer.
 * @param p Pointer to the buffer to be cleared and deallocated.
 */
template <class T> 
_STAXXEXPORT void return_temporary_buffer(T* p) {
	clear(p);
}

#pragma GCC diagnostic pop

END_NP_BLOCK

#endif