#pragma once

#include <map>
#include <typeindex>
#include <memory>
#include <list>
#include <functional>

#include "Event.h"

class IEventCallback
{
public:
	virtual ~IEventCallback() = default;

	inline void Execute(Event& e);

private:
	virtual void Call(Event& e) = 0;
};

template<typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
public:
	typedef void (TOwner::* CallbackFunction)(TEvent&);

	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFucntion);

	virtual void Call(Event& e) override;

private:

	TOwner* ownerInstance;
	CallbackFunction callbackFunction;
};

template<typename TOwner, typename TEvent>
class MyEventCallback : public IEventCallback
{
public:
	typedef void (TOwner::* CallbackFunction)(TEvent&);

	MyEventCallback(TOwner* ownerInstance, CallbackFunction callbackFucntion);

	virtual void Call(Event& e) override;

private:

	std::function<void(const TOwner&, TEvent&)> callbackFunction;
};


using HandlerList = std::list<std::unique_ptr<IEventCallback>>;

class EventBus
{
public:
	EventBus();
	~EventBus();

	void Reset();

	template<typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::* callbackFunction)(TEvent& e));

	template<typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args);

private:
	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

};

void IEventCallback::Execute(Event& e)
{
	Call(e);
}

template<typename TOwner, typename TEvent>
inline EventCallback<TOwner, TEvent>::EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
	: ownerInstance(ownerInstance)
	, callbackFunction(callbackFunction)
{
}

template<typename TOwner, typename TEvent>
inline void EventCallback<TOwner, TEvent>::Call(Event& e)
{
	std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
}

template<typename TEvent, typename TOwner>
inline void EventBus::SubscribeToEvent(TOwner* ownerInstance, void(TOwner::*callbackFunction)(TEvent& e))
{
	if (!subscribers[typeid(TEvent)].get()) {
		subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
	}

	auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
	subscribers[typeid(TEvent)]->push_back(std::move(subscriber));

}

template<typename TEvent, typename ...TArgs>
inline void EventBus::EmitEvent(TArgs&& ...args)
{
	HandlerList* handlerList = subscribers.at(typeid(TEvent)).get();
	if (!handlerList) {
		return;
	}

	TEvent event(std::forward<TArgs>(args)...);
	for (auto handler_it = handlerList->begin(); handler_it != handlerList->end(); ++handler_it) {
		handler_it->get()->Execute(event);
	}
}
