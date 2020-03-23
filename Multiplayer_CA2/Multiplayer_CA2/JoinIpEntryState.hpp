#ifndef BOOK_JOINIPENTRYSTATE_HPP
#define BOOK_JOINIPENTRYSTATE_HPP

#include "State.hpp"
#include "KeyBinding.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include "Label.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>


class JoinIpEntryState : public State
{
public:
	JoinIpEntryState(StateStack& stack, Context context);

	virtual void			draw();
	virtual bool			update(sf::Time dt);
	virtual bool			handleEvent(const sf::Event& event);

private:
	sf::Sprite						mBackgroundSprite;
	GUI::Container					mGUIContainer;
	std::array<GUI::Label::Ptr, 2> 	mBindingLabels;
	std::array<GUI::Button::Ptr, 1> mBindingButtons;
	std::string						mIpAddress;
};

#endif // BOOK_SETTINGSSTATE_HPP
