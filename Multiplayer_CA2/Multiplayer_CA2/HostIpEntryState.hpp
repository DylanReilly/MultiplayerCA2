#ifndef BOOK_HOSTENTRYSTATE_HPP
#define BOOK_HOSTENTRYSTATE_HPP

#include "State.hpp"
#include "KeyBinding.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include "Label.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>


class HostIpEntryState : public State
{
public:
	HostIpEntryState(StateStack& stack, Context context);

	virtual void			draw();
	virtual bool			update(sf::Time dt);
	virtual bool			handleEvent(const sf::Event& event);

private:
	sf::Sprite						mBackgroundSprite;
	GUI::Container					mGUIContainer;
	std::array<GUI::Label::Ptr, 4> 	mBindingLabels;
	std::array<GUI::Button::Ptr, 2> mBindingButtons;
	std::string						mIpAddress;
	std::string						mUserName;
};

#endif // BOOK_SETTINGSSTATE_HPP
