//Jason Lynch - D00137655
#ifndef BOOK_HOWTOPLAYSTATE_HPP
#define BOOK_HOWTOPLAYSTATE_HPP

#include "State.hpp"
#include "Container.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class HowToPlayState : public State
{
public:
	HowToPlayState(StateStack& stack, Context context);

	virtual void			draw();
	virtual bool			update(sf::Time dt);
	virtual bool			handleEvent(const sf::Event& event);


private:
	sf::Sprite				mBackgroundSprite;
	GUI::Container			mGUIContainer;
};

#endif // BOOK_HOWTOPLAYSTATE_HPP