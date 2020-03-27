//D00137655 - Jason Lynch
//D00194504 - Dylan
#include "SettingsState.hpp"
#include "Button.hpp"
#include "Utility.hpp"
#include "MusicPlayer.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);

	auto keyboardControlsButton = std::make_shared<GUI::Button>(context);
	keyboardControlsButton->setPosition(100, 300);
	keyboardControlsButton->setText("Keyboard Controls");
	keyboardControlsButton->setCallback([this]()
		{
			requestStackPush(States::Keyboardcontrol);
		});

	auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition(100, 350);
	exitButton->setText("Back");
	exitButton->setCallback([this]()
		{
			requestStackPop();
		});

	mGUIContainer.pack(keyboardControlsButton);
	mGUIContainer.pack(exitButton);

	// Play menu theme
	context.music->play(Music::MenuTheme);
}

void SettingsState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool SettingsState::update(sf::Time)
{
	return true;
}

bool SettingsState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}
