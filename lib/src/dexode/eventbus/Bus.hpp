//
// Created by gelldur on 26.11.2019.
//
#pragma once

#include <any>
#include <atomic>
#include <memory>

#include "dexode/eventbus/Listener.hpp"
#include "dexode/eventbus/internal/ListenerAttorney.hpp"
#include "dexode/eventbus/internal/event_id.hpp"
#include "dexode/eventbus/stream/ProtectedEventStream.hpp"

namespace dexode::eventbus
{

class Bus;

template <typename Event>
using DefaultEventStream = eventbus::stream::ProtectedEventStream<Event>;
using CreateStreamCallback = std::unique_ptr<eventbus::stream::EventStream> (*const)();
using PostponeCallback = bool (*const)(Bus& bus, std::any event);

template <typename Event>
bool postpone(Bus& bus, std::any event);

template <typename Event>
std::unique_ptr<eventbus::stream::EventStream> createDefaultEventStream()
{
	return std::make_unique<DefaultEventStream<Event>>();
}

class PostponeHelper
{
public:
	internal::event_id_t eventID = nullptr;

	PostponeCallback postponeCallback = nullptr; // function pointer
	CreateStreamCallback createStreamCallback = nullptr; // function pointer

	PostponeHelper(const internal::event_id_t eventId,
	               PostponeCallback postponeCallback,
	               CreateStreamCallback createStreamCallback)
		: eventID(eventId)
		  , postponeCallback(postponeCallback)
		  , createStreamCallback(createStreamCallback) {}

	template <typename Event>
	static PostponeHelper create()
	{
		static_assert(internal::validateEvent<Event>(), "Invalid event");
		return PostponeHelper{internal::event_id<Event>(),
		                      postpone<Event>,
		                      createDefaultEventStream<Event>};
	}

	~PostponeHelper() = default;
};

class Bus
{
	template <typename>
	friend class dexode::eventbus::internal::ListenerAttorney;

public:
	using Listener = eventbus::Listener<dexode::eventbus::Bus>;

	Bus() = default;
	virtual ~Bus() = default;

	virtual std::size_t process() = 0;

	template <typename Event>
	bool postpone(Event&& event)
	{
		using Event_t = std::decay_t<Event>;
		static_assert(internal::validateEvent<Event_t>(), "Invalid event");
		auto postponeCall = PostponeHelper::create<Event_t>();
		return postponeEvent(postponeCall, std::forward<Event>(event));
	}

protected:
	virtual bool postponeEvent(PostponeHelper& postponeCall, std::any&& event) = 0;
	virtual eventbus::stream::EventStream* listen(std::uint32_t listenerID,
												  internal::event_id_t eventID,
												  CreateStreamCallback createStreamCallback) = 0;

	virtual void unlistenAll(std::uint32_t listenerID) = 0;
	virtual void unlisten(std::uint32_t listenerID, internal::event_id_t eventID) = 0;

	virtual eventbus::stream::EventStream* streamForEvent(
		eventbus::internal::event_id_t eventID) const = 0;

private:
	std::atomic<std::uint32_t> _lastID{0};

	std::uint32_t newListenerID()
	{
		return ++_lastID; // used for generate unique listeners ID's
	}

	template <class Event>
	void listen(const std::uint32_t listenerID, std::function<void(const Event&)>&& callback)
	{
		static_assert(internal::validateEvent<Event>(), "Invalid event");
		assert(callback && "callback should be valid"); // Check for valid object

		constexpr auto eventID = internal::event_id<Event>();

		auto* eventStream = listen(listenerID, eventID, createDefaultEventStream<Event>);
		if(eventStream != nullptr) // maybe someone don't want add listener
		{
			eventStream->addListener(listenerID,
									 std::forward<std::function<void(const Event&)>>(callback));
		}
	}
};

template <typename Event>
bool postpone(Bus& bus, std::any event)
{
	return bus.postpone(std::any_cast<Event>(event));
}

} // namespace dexode::eventbus
