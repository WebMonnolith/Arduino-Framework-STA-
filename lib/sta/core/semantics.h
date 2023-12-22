#ifndef STA_SEMANTICS_H
#define STA_SEMANTICS_H

/* 
Defines default constructor 
 Auomates the boilerplate code for classes
*/
#	define STA_CLS_CTOR_DEFAULT(name) name() = default;

/* 
Defines default destructor 
 Auomates the boilerplate code for classes
*/
#	define STA_CLS_DTOR_DEFAULT(name) ~name() = default;

/* 
Defines default de-/constructor 
 Auomates the boilerplate code for classes
*/
#	define STA_CLS_DCTOR_DEFAULT(name) STA_CLS_CTOR_DEFAULT(name) STA_CLS_DTOR_DEFAULT(name) 

/* 
Defines constructor 
 Auomates the boilerplate code for classes
*/
#	define STA_CLS_CTOR(name) name();

/* 
Defines destructor 
 Auomates the boilerplate code for classes
*/
#	define STA_CLS_DTOR(name) ~name();

/* 
Defines de- & constructor 
 Auomates the boilerplate code for classes
*/
#	define STA_CLS_DCTOR(name) STA_CLS_CTOR(name) STA_CLS_DTOR(name)

/* 
Makes a class non copiable. 
 Auomates the boilerplate code for  
 declaring a class as non-copiable 
 */
#	define STA_CLS_NON_COPIABLE(name) name(const name&) = delete;\
									  name& operator= (const name&) = delete;

/*
Makes a class movable.
Automates the boilerplate code for 
declaring default move constructor 
and move operator. 
*/
#	define STA_CLS_CTOR_MOVE_DEFAULT(name) name(name&&) = default;\
										   name& operator = (name&&) = default;       

/*
Makes a class movable.
Automates the boilerplate code for
declaring move constructor and move operator.
*/
#	define STA_CLS_CTOR_MOVE(name) name(name&&) noexcept;\
										   name& operator = (name&&) noexcept; 

/*
Makes a class only movable, but non-copiable.
*/
#	define STA_CLS_MOVABLE_ONLY(name) STA_CLS_NON_COPIABLE(name) STA_CLS_CTOR_DEFAULT(name) 

/*
Declare default copy constructor explicitly
*/
#	define STA_CLS_CTOR_COPY_DEFAULT(name) name(const name&) noexcept = default;\
										   name& operator = (const name&) noexcept = default;


#endif