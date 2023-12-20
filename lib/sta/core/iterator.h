#ifndef _STA_ITERATOR_
#define _STA_ITERATOR_

#include "sta.h"

BEGIN_NP_BLOCK

template<class Iterator> struct iterator_traits;
template<class T> struct iterator_traits<T*>;

template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&> struct iterator;

struct _STAXXEXPORT input_iterator_tag {};
struct _STAXXEXPORT output_iterator_tag {};
struct _STAXXEXPORT forward_iterator_tag : public input_iterator_tag {};
struct _STAXXEXPORT bidirectional_iterator_tag : public forward_iterator_tag {};
struct _STAXXEXPORT random_access_iterator_tag : public bidirectional_iterator_tag {};

template <class InputIterator, class Distance> _STAXXEXPORT void advance(InputIterator& i, Distance n) {
	while (n > 0) {
		--n;
		++i;
	}
}

template <class InputIterator> _STAXXEXPORT typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
	typename iterator_traits<InputIterator>::difference_type d = 0;
	while (first++ != last) {
		d++;
	}
	return d;
}

// subclause _lib.predef.iterators_, predefined iterators:
template <class Iterator> class reverse_iterator;
template <class Iterator> bool operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> bool operator<(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> bool operator!=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> bool operator>(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> bool operator>=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> bool operator<=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
template <class Iterator> reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& x);
template <class Container> class back_insert_iterator;
template <class Container> back_insert_iterator<Container> back_inserter(Container& x);
template <class Container> class front_insert_iterator;
template <class Container> front_insert_iterator<Container> front_inserter(Container& x);
template <class Container> class insert_iterator;
template <class Container, class Iterator>
insert_iterator<Container> inserter(Container& x, Iterator i);

//Actual Template definitions
template<class Iterator> struct _STAXXEXPORT iterator_traits {
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
	typedef typename Iterator::iterator_category iterator_category;
};

//Pointer specialization - required by standard
template<class T> struct _STAXXEXPORT iterator_traits<T*> {
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef random_access_iterator_tag iterator_category;
};

//Specialization recomended by standard
/*	template<class T> struct _STAXXEXPORT iterator_traits<T __far*> {
	typedef long difference_type;
	typedef T value_type;
	typedef T __far* pointer;
	typedef T __far& reference;
	typedef random_access_iterator_tag iterator_category;
};*/

/*	template <class BidirectionalIterator> _STAXXEXPORT void
		reverse(BidirectionalIterator first, BidirectionalIterator last)
	{
		typename iterator_traits<BidirectionalIterator>::difference_type n = distance(first, last);
		--n;
		while(n > 0){
			typename iterator_traits<BidirectionalIterator>::value_type tmp = *first;
			*first++ = * --last;
			*last = tmp;
			n -= 2;
		}
	};*/


template <class Category,
	class T,
	class Distance,
	class Pointer,
	class Reference>
struct _STAXXEXPORT iterator {
	typedef T         value_type;
	typedef Distance  difference_type;
	typedef Pointer   pointer;
	typedef Reference reference;
	typedef Category  iterator_category;
};


template <class Iterator>
class _STAXXEXPORT reverse_iterator
	:
	public iterator<typename iterator_traits<Iterator>::iterator_category,
	typename iterator_traits<Iterator>::value_type, typename iterator_traits<Iterator>::difference_type,
	typename iterator_traits<Iterator>::pointer, typename iterator_traits<Iterator>::reference>
{
public:
	typedef Iterator iterator_type;

	reverse_iterator() : current() {}
	explicit reverse_iterator(Iterator x) : current(x) { }
	template<class U> reverse_iterator(const reverse_iterator<U>& x) : current(x.base()) {}

	Iterator base() const { return current; }       // explicit

	typename iterator_traits<Iterator>::reference operator*() const { Iterator tmp = current; return *--tmp; }
	typename iterator_traits<Iterator>::pointer   operator->() const { return &(operator*()); }
	typename iterator_traits<Iterator>::reference operator[](typename iterator_traits<Iterator>::difference_type n) const {
		return current[-n - 1];
	}

	reverse_iterator& operator++() { --current; return *this; }
	reverse_iterator  operator++(int) { reverse_iterator tmp = *this; --current; return tmp; }
	reverse_iterator& operator--() { ++current; return *this; }
	reverse_iterator  operator--(int) { reverse_iterator tmp = *this; ++current; return tmp; }

	reverse_iterator  operator+ (typename iterator_traits<Iterator>::difference_type n) const {
		reverse_iterator retval(*this);
		retval += n;
		return retval;
	}
	reverse_iterator& operator+=(typename iterator_traits<Iterator>::difference_type n) {
		current -= n;
		return *this;
	}
	reverse_iterator operator- (typename iterator_traits<Iterator>::difference_type n) const {
		reverse_iterator retval(*this);
		retval -= n;
		return retval;
	}
	reverse_iterator& operator-=(typename iterator_traits<Iterator>::difference_type n) {
		current += n;
		return *this;
	}

protected:
	Iterator current;
	friend bool operator== <Iterator>(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
	friend bool operator< <Iterator>(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y);
};


template <class Iterator> _STAXXEXPORT bool
	operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() == y.base();
}

template <class Iterator> _STAXXEXPORT bool
	operator<(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() < y.base();
}

template <class Iterator> _STAXXEXPORT bool
	operator!=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() != y.base();
}

template <class Iterator> _STAXXEXPORT bool
	operator>(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() > y.base();
}

template <class Iterator> _STAXXEXPORT bool
	operator>=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() >= y.base();
}

template <class Iterator> _STAXXEXPORT bool
	operator<=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() <= y.base();
}

template <class Iterator> _STAXXEXPORT typename reverse_iterator<Iterator>::difference_type
	operator-(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return y.base() - x.base();
}

template <class Iterator> _STAXXEXPORT reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& x)
{
	return reverse_iterator<Iterator>(x.base() - n);
}

template <class Container>
class _STAXXEXPORT back_insert_iterator :
	public iterator<output_iterator_tag, void, void, void, void>
{
public:
	typedef Container container_type;
	explicit back_insert_iterator(Container& x) :container(x) {}
	back_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
		container.push_back(value);
		return *this;
	}
	back_insert_iterator<Container>& operator*() {
		return *this;
	}
	back_insert_iterator<Container>& operator++() {
		return *this;
	}
	back_insert_iterator<Container>  operator++(int) {
		return *this;
	}
protected:
	Container& container;
};

template <class Container>
_STAXXEXPORT back_insert_iterator<Container> back_inserter(Container& x) {
	return back_insert_iterator<Container>(x);
}

template <class Container>
class _STAXXEXPORT front_insert_iterator
	: public iterator<output_iterator_tag, void, void, void, void>
{
protected:
	Container& container;
public:
	typedef Container container_type;
	explicit front_insert_iterator(Container& x) : container(x) {}
	front_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
		container.push_front(value);
		return *this;
	}

	front_insert_iterator<Container>& operator*() { return *this; }
	front_insert_iterator<Container>& operator++() { return *this; }
	front_insert_iterator<Container> operator++(int) { return *this; }
};

template <class Container>
_STAXXEXPORT front_insert_iterator<Container>
	front_inserter(Container& x) {
	return front_insert_iterator<Container>(x);
}

template <class Container>
class _STAXXEXPORT insert_iterator
	: public iterator<output_iterator_tag, void, void, void, void>
{
public:
	typedef Container container_type;
	insert_iterator(Container& x, typename Container::iterator i) : container(x), iter(i) {}
	insert_iterator<Container>& operator=(const typename Container::value_type& value) {
		iter = container.insert(iter, value);
		++iter;
		return *this;
	}
	insert_iterator<Container>& operator*() { return *this; }
	insert_iterator<Container>& operator++() { return *this; }
	insert_iterator<Container> operator++(int) { return *this; }
protected:
	Container& container;
	typename Container::iterator iter;
};

template <class Container, class Iterator>
_STAXXEXPORT insert_iterator<Container>
	inserter(Container& x, Iterator i) {
	return insert_iterator<Container>(x, typename Container::iterator(i));
}

END_NP_BLOCK

#endif