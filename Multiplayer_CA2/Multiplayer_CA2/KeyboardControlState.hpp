//Jason Lynch - D00137655
#ifndef BOOK_KEYBOARDCONTROLSTATE_HPP
#define BOOK_KEYBOARDCONTROLSTATE_HPP

#include "State.hpp"
#include "KeyBinding.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include "Label.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>


class KeyboardControlState : public State
{
public:
	KeyboardControlState(StateStack& stack, Context context);

	virtual void					draw();
	virtual bool					update(sf::Time dt);
	virtual bool					handleEvent(const sf::Event& event);


private:
	void							updateLabels();
	void							addButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context);


private:
	sf::Sprite											mBackgroundSprite;
	GUI::Container										mGUIContainer;
	std::array<GUI::Button::Ptr, 2 * PlayerAction::Count>	mBindingButtons;
	std::array<GUI::Label::Ptr, 2 * PlayerAction::Count> 	mBindingLabels;
};

#endif // BOOK_KEYBOARDCONTROLSTATE_HPP
