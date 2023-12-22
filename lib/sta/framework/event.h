#ifndef _STA_EVENT_H_
#define _STA_EVENT_H_

#include "./core/functional.h"

#define STA_BIT(x) 						(1 << (x))
#define STA_LSB(x)						((x) ^ ((x) - 1) & (x))

// Macros for simplifying the abstract method implementations

#define LGE_EVENT_CLASS_TYPE(type)  static sta::EventType getStaticType() { return sta::EventType::##type; }\
									virtual sta::EventType getEventType() const override { return getStaticType(); }\
									virtual const char* getName() const override { return #type; }

#define LGE_EVENT_CLASS_CATEGORY(category) virtual sta::int32 getCategoryFlags() const override {return category;}

BEGIN_NP_BLOCK

_STAXXEXPORT enum class EventType {
	none = 0,
};

_STAXXEXPORT enum EventCategory {
	none = 0,
	eventCategoryException  		= STA_BIT(0),
	eventCategoryInput				= STA_BIT(1),
	eventCategoryServer 			= STA_BIT(2),
};


class _STAXXEXPORT Event {
private:
	friend class EventDispatcher;
	int32 handled = 0;
public:
    Event(const Event& event) {

    }
public:

	virtual ~Event() = default;

	virtual EventType getEventType() const = 0;
	virtual const char* getName() const = 0;
	virtual int32 getCategoryFlags() const = 0;
	virtual String toString() const { return String(this->getName()); } // DEBUG only

public:
	inline int32 isInCategory(EventCategory category) { return this->getCategoryFlags() & category; }
	inline int32 isHandled() const { return this->handled; }
};

class _STAXXEXPORT EventDispatcher {
private:
	Event& event;

	template <typename T>
	using EventFn = sta::function<bool(T&)>;

public:

	EventDispatcher(Event& event) : event(event) {}

	template <typename T>
	int32 dispatch(EventFn<T> func) {
		if (this->event.getEventType() == T::getStaticType()) {
			this->event.handled = func(*(T*)&this->event);
			return 1;
		}
		return 1;
	}
};

class _STAXXEXPORT ExceptionEvent {
    
}

END_NP_BLOCK

#endif