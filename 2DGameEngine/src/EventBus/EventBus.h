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


template<typename TEvent>
class EventCallback : public IEventCallback
{
public:
	using CallBackFunction_t = std::function<void(TEvent&)>;

	template<typename T>
	EventCallback(T&& callbackFunction) : callbackFunction(std::forward<T>(callbackFunction)) { }

	virtual void Call(Event& e) override;

private:

	CallBackFunction_t callbackFunction;
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


template<typename TEvent, typename TOwner>
inline void EventBus::SubscribeToEvents(TOwner* ownerInstance, void(TOwner::* CallbackFunction)(TEvent&))
{
	if (!subscribers[typeid(TEvent)].get()) {
		subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
	}

	std::function<void(TEvent&)> callback_fn = std::bind(CallbackFunction, ownerInstance, std::placeholders::_1);
	auto subscriber = std::make_unique<EventCallback<TEvent>>(callback_fn);

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

template<typename TEvent>
inline void EventCallback<TEvent>::Call(Event& e)
{
	if (callbackFunction) {
		callbackFunction(static_cast<TEvent&>(e));
	}
}
