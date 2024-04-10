//
// Created by gelldur on 24.12.2019.
//
#pragma once

#include <any>
#include <functional>
#include <map>
#include <string>

#include "Perk.hpp"
#include "dexode/eventbus/Bus.hpp"
#include "dexode/eventbus/internal/event_id.hpp"

namespace dexode::eventbus::perk
{

class TagPerk : public Perk
{
public:
	TagPerk(std::string tag, dexode::eventbus::Bus* owner)
		: _tag{std::move(tag)}
		  , _ownerBus{owner} {}

	Flag onPrePostponeEvent(PostponeHelper& postponeCall, std::any&& event);

	template <typename TagEvent>
	TagPerk& wrapTag()
	{
		static_assert(internal::validateEvent<TagEvent>(), "Invalid tag event");
		static_assert(internal::validateEvent<typename TagEvent::Event>(), "Invalid event");
		constexpr auto eventID = internal::event_id<typename TagEvent::Event>();

		_eventsToWrap[eventID] = [this](std::any&& event) {
			_ownerBus->postpone<TagEvent>(
				TagEvent{
					_tag, std::any_cast<typename TagEvent::Event>(std::forward<std::any>(event))});
		};
		return *this;
	}

private:
	std::map<internal::event_id_t, std::function<void(std::any&&)>> _eventsToWrap;
	std::string _tag;
	dexode::eventbus::Bus* _ownerBus;
};

} // namespace dexode::eventbus::perk
