#ifndef _STA_event_H_
#define _STA_event_H_

#include "./core/functional.h"
#include "./core/exception.h"

#define STA_BIT(x) 						(1 << (x))
#define STA_LSB(x)						((x) ^ ((x) - 1) & (x))

// Macros for simplifying the abstract method implementations

#define STA_EVENT_CLASS_TYPE(type)  static sta::event_type getStaticType() { return sta::##type; }\
									virtual sta::event_type getevent_type() const override { return getStaticType(); }\
									virtual const char* getName() const override { return #type; }

#define STA_EVENT_CLASS_CATEGORY(category) virtual sta::int32 getCategoryFlags() const override {return category;}

BEGIN_NP_BLOCK

_STAXXEXPORT enum class event_type {
	none = 0,
	appTick, appUpdate, appError, appException,
	serverStatus

};

_STAXXEXPORT enum event_category {
	none = 0,
	eventCategoryException  		= STA_BIT(0),
	eventCategoryInput				= STA_BIT(1),
	eventCategoryServer 			= STA_BIT(2),
};


class _STAXXEXPORT event {
private:
	friend class event_dispatcher;
	int32 handled = 0;
public:
	event() = default;
    event(const event& event) {}
public:

	virtual ~event() = default;

	virtual event_type getevent_type() const = 0;
	virtual const char* getName() const = 0;
	virtual int32 getCategoryFlags() const = 0;
	virtual String toString() const { return String(this->getName()); } // DEBUG only

public:
	inline int32 isInCategory(event_category category) { return this->getCategoryFlags() & category; }
	inline int32 isHandled() const { return this->handled; }
};

class _STAXXEXPORT event_dispatcher {
private:
	event& e;

	template <typename T>
	using eventFn = sta::function<bool(T&)>;

public:

	event_dispatcher(event& e) : e(e) {}

	template <typename T>
	int32 dispatch(eventFn<T> func) {
		if (this->event.getevent_type() == T::getStaticType()) {
			this->event.handled = func(*(T*)&this->event);
			return 1;
		}
		return 1;
	}
};

class _STAXXEXPORT exception_event : public event {
public:
	exception_event(exception e)
		: ex(e) 
	{}
public:
	STA_EVENT_CLASS_TYPE(event_type::appException)
	STA_EVENT_CLASS_CATEGORY(event_category::eventCategoryException)
private:
	exception ex;
};

class _STAXXEXPORT hid_event : public event {
public:
	hid_event(int32 value)
		: value(value) 
	{}
public:
	STA_EVENT_CLASS_TYPE(event_type::appUpdate)
	STA_EVENT_CLASS_CATEGORY(event_category::eventCategoryInput)
private:
	int32 value;
};

class _STAXXEXPORT server_event : public event {
public:
	server_event()
	{}
public:
	STA_EVENT_CLASS_TYPE(event_type::serverStatus)
	STA_EVENT_CLASS_CATEGORY(event_category::eventCategoryServer)
private:
};

END_NP_BLOCK

#endif