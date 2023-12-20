#ifndef _STA_FUNCTIONAL_
#define _STA_FUNCTIONAL_

#include "sta.h"

BEGIN_NP_BLOCK

/*
Example use of sta::function

... {
    ...
    auto func = sta::function<void(int)>{
        [](int a){
            Serial.println(a);
        }
    };
    ...
}

*/

template <typename T>
class function;

/* 
Simple lambda function API, to make use of lambdas in c++ easier.
Derived from std::function<ReturnValue, ... Args>
*/
template <typename ReturnValue, typename... Args>
class _STAXXEXPORT 
function<ReturnValue(Args...)> {
public: // CON-/DESTRUCTORS
    template <typename T>
    function(T t) {
        callable_ = new CallableT<T>(t);
    }

    ~function() {
        if (callable_ == nullptr) delete callable_;
    }
public: // OPERATORS 
    template <typename T>
    function& operator=(T t) {
        if (callable_ == nullptr) delete callable_;
        callable_ = new CallableT<T>(t);
        return *this;
    }

    ReturnValue operator()(Args... args) const {
        return callable_->Invoke(args...);
    }
private: // INTERNAL TYPES
    class ICallable {
    public:
        virtual ~ICallable() = default;
        virtual ReturnValue Invoke(Args...) = 0;
    };

    template <typename T>
    class CallableT : public ICallable {
    public:
        CallableT(const T& t)
        : t_(t) {
        }

        ~CallableT() override = default;

        ReturnValue Invoke(Args... args) override {
            return t_(args...);
        }

    private:
        T t_;
    };
private: // ATTRIBUTES
    ICallable* callable_;
};

template <class Arg, class Result> struct unary_function;
template <class Arg1, class Arg2, class Result> struct binary_function;

template <class T> struct plus;
template <class T> struct minus;
template <class T> struct multiplies;
template <class T> struct divides;
template <class T> struct modulus;
template <class T> struct negate;

template <class T> struct equal_to;
template <class T> struct not_equal_to;
template <class T> struct greater;
template <class T> struct less;
template <class T> struct greater_equal;
template <class T> struct less_equal;

template <class T> struct logical_and;
template <class T> struct logical_or;
template <class T> struct logical_not;

template <class Predicate> struct unary_negate;
template <class Predicate> unary_negate<Predicate>  not1(const Predicate&);
template <class Predicate> struct binary_negate;
template <class Predicate> binary_negate<Predicate> not2(const Predicate&);


template <class Operation> class binder1st;
template <class Operation, class T> binder1st<Operation> bind1st(const Operation&, const T&);
template <class Operation> class binder2nd;
template <class Operation, class T> binder2nd<Operation> bind2nd(const Operation&, const T&);

template <class Arg, class Result> class pointer_to_unary_function;
template <class Arg, class Result> pointer_to_unary_function<Arg,Result> ptr_fun(Result (*)(Arg));
template <class Arg1, class Arg2, class Result> class pointer_to_binary_function;
template <class Arg1, class Arg2, class Result>
    pointer_to_binary_function<Arg1,Arg2,Result> ptr_fun(Result (*)(Arg1,Arg2));

template<class S, class T> class mem_fun_t;
template<class S, class T, class A> class mem_fun1_t;
template<class S, class T> class const_mem_fun_t;
template<class S, class T, class A> class const_mem_fun1_t;
template<class S, class T> mem_fun_t<S,T> mem_fun(S (T::*f)());
template<class S, class T, class A> mem_fun1_t<S,T,A> mem_fun(S (T::*f)(A));
template<class S, class T> class mem_fun_ref_t;
template<class S, class T, class A> class mem_fun1_ref_t;
template<class S, class T> mem_fun_ref_t<S,T> mem_fun_ref(S (T::*f)());
template<class S, class T, class A> mem_fun1_ref_t<S,T,A> mem_fun1_ref(S (T::*f)(A));

//Implementation

template <class Arg, class Result> struct _STAXXEXPORT unary_function{
    typedef Arg argument_type;
    typedef Result result_type;
};


template <class Arg1, class Arg2, class Result> struct _STAXXEXPORT binary_function{
    typedef Arg1   first_argument_type;
    typedef Arg2   second_argument_type;
    typedef Result result_type;
};

template <class T> struct _STAXXEXPORT plus : binary_function<T,T,T>{
    T operator()(const T& x, const T& y) const{
        return x + y;
    }
};

template <class T> struct _STAXXEXPORT minus : binary_function<T,T,T>{
    T operator()(const T& x, const T& y) const{
        return x - y;
    }
};

template <class T> struct _STAXXEXPORT multiplies : binary_function<T,T,T>{
    T operator()(const T& x, const T& y) const{
        return x * y;
    }
};

template <class T> struct _STAXXEXPORT divides : binary_function<T,T,T>{
    T operator()(const T& x, const T& y) const{
        return x / y;
    }
};

template <class T> struct _STAXXEXPORT modulus : binary_function<T,T,T>{
    T operator()(const T& x, const T& y) const{
        return x % y;
    }
};

template <class T> struct _STAXXEXPORT negate : unary_function<T,T>{
    T operator()(const T& x) const{
        return -x;
    }
};

template <class T> struct _STAXXEXPORT equal_to : binary_function<T,T,bool>{
    bool operator()(const T& x, const T& y) const{
        return (x == y);
    }
};

template <class T> struct _STAXXEXPORT not_equal_to : binary_function<T,T,bool>{
    bool operator()(const T& x, const T& y) const{
        return (x != y);
    }
};

template <class T> struct _STAXXEXPORT greater : binary_function<T,T,bool>{
    bool operator()(const T& x, const T& y) const _STAXX_NOTHROW {
        return (x > y);
    }
};

template <class T> struct _STAXXEXPORT less : binary_function<T,T,bool>{
    bool operator()(const T& x, const T& y) const _STAXX_NOTHROW {
        return (x < y);
    }
};

template <class T> struct _STAXXEXPORT greater_equal : binary_function<T,T,bool>{
    bool operator()(const T& x, const T& y) const _STAXX_NOTHROW {
        return (x >= y);
    }
};

template <class T> struct _STAXXEXPORT less_equal : binary_function<T,T,bool>{
    bool operator()(const T& x, const T& y) const _STAXX_NOTHROW {
        return (x <= y);
    }
};

template <class T> struct _STAXXEXPORT logical_and : binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const{
        return (x && y);
    }
};

template <class T> struct _STAXXEXPORT logical_or : binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const{
        return (x || y);
    }
};

template <class T> struct _STAXXEXPORT logical_not : unary_function<T,bool> {
    bool operator()(const T& x) const{
        return !x;
    }
};

template <class Predicate> class _STAXXEXPORT unary_negate
    : public unary_function<typename Predicate::argument_type,bool>
{
public:
    explicit unary_negate(const Predicate& pred) : p(pred) {  }
    bool operator()(const typename Predicate::argument_type& x) const{
        return !p(x);
    }
private:
    Predicate p;
};


template <class Predicate> _STAXXEXPORT unary_negate<Predicate> not1(const Predicate& pred){
    return unary_negate<Predicate>(pred);
}


template <class Predicate> class _STAXXEXPORT binary_negate : public
    binary_function<typename Predicate::first_argument_type,
        typename Predicate::second_argument_type, bool>
{
public:
    explicit binary_negate(const Predicate& pred) : p(pred) {  }
    bool operator()(const typename Predicate::first_argument_type& x,
        const typename Predicate::second_argument_type& y) const
    {
        return !p(x, y);
    }
private:
    Predicate p;
};


template <class Predicate> _STAXXEXPORT binary_negate<Predicate> not2(const Predicate& pred){
    return binary_negate<Predicate>(pred);
}


template <class Operation> class _STAXXEXPORT binder1st
    : public unary_function<typename Operation::second_argument_type,
        typename Operation::result_type>
{
protected:
    Operation                      op;
    typename Operation::first_argument_type value;
public:
    binder1st(const Operation& x, const typename Operation::first_argument_type& y) : op(x), value(y){  }
    typename Operation::result_type operator()(const typename Operation::second_argument_type& x) const{
        return op(value,x);
    }
};


template <class Operation, class T> _STAXXEXPORT binder1st<Operation> bind1st(const Operation& op, const T& x){
    return binder1st<Operation>(op, typename Operation::first_argument_type(x));
}


template <class Operation> class _STAXXEXPORT binder2nd
    : public unary_function<typename Operation::first_argument_type,
        typename Operation::result_type>
{
protected:
    Operation                       op;
    typename Operation::second_argument_type value;
public:
    binder2nd(const Operation& x, const typename Operation::second_argument_type& y) : op(x), value(y) {  }
    typename Operation::result_type operator()(const typename Operation::first_argument_type& x) const{
        return op(x,value);
    }
};


template <class Operation, class T> _STAXXEXPORT
    binder2nd<Operation> bind2nd(const Operation& op, const T& x)
{
    return binder2nd<Operation>(op, typename Operation::second_argument_type(x));
}


template <class Arg, class Result> class _STAXXEXPORT
    pointer_to_unary_function : public unary_function<Arg, Result>
{
protected:
    Result (*func)(Arg);
public:
    explicit pointer_to_unary_function(Result (*f)(Arg)) : func(f) {  }
    Result operator()(Arg x) const{
        return func(x);
    }
};


template <class Arg, class Result> _STAXXEXPORT pointer_to_unary_function<Arg, Result> ptr_fun(Result (*f)(Arg)){
    return pointer_to_unary_function<Arg, Result>(f);
}


template <class Arg1, class Arg2, class Result>	class _STAXXEXPORT
    pointer_to_binary_function : public binary_function<Arg1,Arg2,Result>
{
protected:
    Result (*func)(Arg1, Arg2);
public:
    explicit pointer_to_binary_function(Result (*f)(Arg1, Arg2)) : func(f) {  }
    Result operator()(Arg1 x, Arg2 y) const{
        return func(x, y);
    }
};

template <class Arg1, class Arg2, class Result> _STAXXEXPORT
    pointer_to_binary_function<Arg1,Arg2,Result> ptr_fun(Result (*f)(Arg1, Arg2))
{
    return pointer_to_binary_function<Arg1,Arg2,Result>(f);
}


template <class S, class T> class _STAXXEXPORT mem_fun_t
    : public unary_function<T*, S>
{
public:
    explicit mem_fun_t(S (T::*p)()) : m(p) {  }
    S operator()(T* p) const { return (p->*m)(); }
private:
    S (T::*m)();
};


template <class S, class T, class A> class _STAXXEXPORT mem_fun1_t
    : public binary_function<T*, A, S>
{
public:
    explicit mem_fun1_t(S (T::*p)(A)) : m(p) {  }
    S operator()(T* p, A x) const { return (p->*m)(x); }
private:
    S (T::*m)(A);
};


template <class S, class T> class _STAXXEXPORT const_mem_fun_t
    : public unary_function<const T*, S>
{
public:
    explicit const_mem_fun_t(S (T::*p)() const) : m(p) {  }
    S operator()(const T* p) const { return (p->*m)(); }
private:
    S (T::*m)() const;
};


template <class S, class T, class A> class _STAXXEXPORT const_mem_fun1_t
    : public binary_function<T*, A, S>
{
public:
    explicit const_mem_fun1_t(S (T::*p)(A) const) : m(p) {  }
    S operator()(const T* p, A x) const { return (p->*m)(x); }
private:
    S (T::*m)(A) const;
};


template<class S, class T> _STAXXEXPORT mem_fun_t<S,T> mem_fun(S (T::*f)()){
    return mem_fun_t<S, T>(f);
}

template<class S, class T> _STAXXEXPORT const_mem_fun_t<S,T> mem_fun(S (T::*f)() const){
    return const_mem_fun_t<S, T>(f);
}

template<class S, class T, class A> _STAXXEXPORT mem_fun1_t<S,T,A> mem_fun(S (T::*f)(A)){
    return mem_fun1_t<S, T, A>(f);
}

template<class S, class T, class A> _STAXXEXPORT const_mem_fun1_t<S,T,A> mem_fun(S (T::*f)(A) const){
    return const_mem_fun1_t<S, T, A>(f);
}

template <class S, class T> class _STAXXEXPORT mem_fun_ref_t
    : public unary_function<T, S>
{
public:
    explicit mem_fun_ref_t(S (T::*p)()) : mf(p) {  }
    S operator()(T& p) { return (p.*mf)(); }
private:
    S (T::*mf)();
};

template <class S, class T, class A> class _STAXXEXPORT mem_fun1_ref_t
    : public binary_function<T, A, S>
{
public:
    explicit mem_fun1_ref_t(S (T::*p)(A)) : mf(p) {  }
    S operator()(T& p, A x) { return (p.*mf)(x); }
private:
    S (T::*mf)(A);
};

template<class S, class T> _STAXXEXPORT mem_fun_ref_t<S,T> mem_fun_ref(S (T::*f)()){
    return mem_fun_ref_t<S,T>(f);
}

template<class S, class T, class A> _STAXXEXPORT mem_fun1_ref_t<S,T,A> mem_fun1_ref(S (T::*f)(A)){
    return mem_fun1_ref_t<S,T,A>(f);
}

//These are SGI extensions which are checked for by some conformance checks.  They
// are *NOT* part of the C++ standard, however

template <class Op1, class Op2> class _STAXXEXPORT unary_compose :
	public unary_function<typename Op2::argument_type,
		typename Op1::result_type>
{
protected:
	Op1 mf1;
	Op2 mf2;
public:
	unary_compose(const Op1& x, const Op2& y) : mf1(x), mf2(y) {  }
	typename Op1::result_type operator()(const typename Op2::argument_type& x) const {
		return mf1(mf2(x));
	}
};

template <class Op1, class Op2> _STAXXEXPORT
inline unary_compose<Op1, Op2>
compose1(const Op1& fn1, const Op2& fn2){
	return unary_compose<Op1, Op2>(fn1, fn2);
}

template <class Op1, class Op2, class Op3> class _STAXXEXPORT binary_compose :
	public unary_function<typename Op2::argument_type, typename Op1::result_type>
{
protected:
	Op1 mf1;
	Op2 mf2;
	Op3 mf3;
public:
	binary_compose(const Op1 & x, const Op2 & y, const Op3 & z)
		: mf1(x), mf2(y), mf3(z){  }
	typename Op1::result_type operator()(const typename Op2::argument_type & x) const {
		return mf1(mf2(x), mf3(x));
	}
};

template <class Op1, class Op2, class Op3> inline _STAXXEXPORT binary_compose<Op1, Op2, Op3>
compose2(const Op1 & fn1, const Op2 & fn2, const Op3 & fn3){
	return binary_compose<Op1, Op2, Op3>(fn1, fn2, fn3);
}


END_NP_BLOCK

#endif