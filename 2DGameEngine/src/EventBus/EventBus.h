#pragma once

#include <map>
#include <typeindex>
#include <memory>
#include <list>
#include <functional>
#include <utility>

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

	template <typename TEvent, typename TOwner>
	void SubscribeToEvents(TOwner* ownerInstance, void (TOwner::* CallbackFunction)(TEvent&));
	
	template <typename TEvent, typename ...TArgs>
	void EmitEvents(TArgs&& ...args);

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
inline void EventBus::SubscribeToEvents(TOwner* ownerInstance, void(TOwner::* CallbackFunction)(TEvent&))
{
	if (!subscribers[typeid(TEvent)].get()) {
		subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
	}

	auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, CallbackFunction);
	subscribers[typeid(TEvent)].get()->push_back(std::move(subscriber));
}

template<typename TEvent, typename ...TArgs>
inline void EventBus::EmitEvents(TArgs&& ...args)
{
	HandlerList* subscribersList = subscribers[typeid(TEvent)].get();

	if (! subscribersList) {
		return;
	}

	TEvent event(std::forward<TArgs>(args)...);
	for (auto handlerIt = subscribersList->begin(); handlerIt != subscribersList->end(); ++handlerIt) {
		(*handlerIt)->Execute(event);
	}
}
