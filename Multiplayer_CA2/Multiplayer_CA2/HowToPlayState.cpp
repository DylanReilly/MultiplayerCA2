//D00137655 - Jason Lynch
//D00194504 - Dylan
#include "HowToPlayState.hpp"
#include "Button.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


HowToPlayState::HowToPlayState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::HowToPlay);
	mBackgroundSprite.setTexture(texture);

	//Back button for exiting 
	auto backButton = std::make_shared<GUI::Button>(context);
	backButton->setPosition(412.f, 680.f);
	backButton->setText("Back");
	backButton->setCallback(std::bind(&HowToPlayState::requestStackPop, this));

	mGUIContainer.pack(backButton);
}

void HowToPlayState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());
	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool HowToPlayState::update(sf::Time dt)
{
	return true;
}

bool HowToPlayState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}