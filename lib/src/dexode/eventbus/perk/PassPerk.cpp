//
// Created by gelldur on 24.12.2019.
//
#include "PassPerk.hpp"

#include "dexode/eventbus/Bus.hpp"

namespace dexode::eventbus::perk
{

Flag PassEverythingPerk::onPrePostponeEvent(PostponeHelper& postponeCall, std::any&& event)
{
	postponeCall.postponeCallback(*_passToBus, std::forward<std::any>(event));
	return Flag::postpone_cancel;
}

} // namespace dexode::eventbus::perk
