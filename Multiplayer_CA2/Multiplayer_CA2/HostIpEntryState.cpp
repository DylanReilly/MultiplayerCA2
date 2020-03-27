#include "HostIpEntryState.hpp"
#include "Button.hpp"
#include "Utility.hpp"
#include "MusicPlayer.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

//extern std::string* HostIp;
extern std::string HostIpAddress;
extern std::string userName;

HostIpEntryState::HostIpEntryState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
	, mIpAddress("")
	, mUserName("")
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);

	/*auto ipDisplayLabel = std::make_shared<GUI::Label>("",*context.fonts);
	ipDisplayLabel->setPosition(100, 250);*/
	//ipDisplayLabel->setText("");

	//auto editIpButton = std::make_shared<GUI::Button>(context);
	//editIpButton->setPosition(100, 300);
	//editIpButton->setText("Enter IP");
	//editIpButton->setCallback([this]()
	//	{
	//		
	//	});

	auto connectButton = std::make_shared<GUI::Button>(context);
	connectButton->setPosition(100, 350);
	connectButton->setText("Start");
	connectButton->setCallback([this]()
		{
			HostIpAddress = mIpAddress;
			userName = mUserName;

			requestStackPop();
			requestStackPush(States::HostGame);
		});

	auto backButton = std::make_shared<GUI::Button>(context);
	backButton->setPosition(100, 400);
	backButton->setText("Back");
	backButton->setCallback([this]()
		{
			requestStackPop();
			requestStackPush(States::Menu);
		});

	mBindingLabels[0] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[0]->setPosition(100, 200);

	mBindingLabels[1] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[1]->setPosition(100, 150);
	mBindingLabels[1]->setText("Your Computers IP Address");

	mBindingLabels[2] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[2]->setPosition(400, 200);

	mBindingLabels[3] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[3]->setPosition(400, 150);
	mBindingLabels[3]->setText("Your Username");

	mBindingButtons[0] = std::make_shared<GUI::Button>(context);
	mBindingButtons[0]->setPosition(100, 250);
	mBindingButtons[0]->setText("Enter Name");
	mBindingButtons[0]->setToggle(true);

	mBindingButtons[1] = std::make_shared<GUI::Button>(context);
	mBindingButtons[1]->setPosition(100, 300);
	mBindingButtons[1]->setText("Enter IP");
	mBindingButtons[1]->setToggle(true);

	mGUIContainer.pack(mBindingLabels[0]);
	mGUIContainer.pack(mBindingLabels[1]);
	//mGUIContainer.pack(mBindingLabels[2]);
	//mGUIContainer.pack(mBindingLabels[3]);
	//mGUIContainer.pack(mBindingButtons[0]);
	mGUIContainer.pack(mBindingButtons[1]);
	mGUIContainer.pack(connectButton);
	mGUIContainer.pack(backButton);

	// Play menu theme
	//context.music->play(Music::MenuTheme);
}

void HostIpEntryState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool HostIpEntryState::update(sf::Time)
{
	return true;
}

bool HostIpEntryState::handleEvent(const sf::Event& event)
{
	bool isIpInput = false;

	if (mBindingButtons[1]->isActive())
	{
		if (event.key.code == sf::Keyboard::Key::Escape) {
			mBindingButtons[1]->deactivate();
			event;
		}
		else if (event.key.code == sf::Keyboard::Key::BackSpace) {
			if (mIpAddress.length() > 0) {
				mIpAddress.pop_back();
			}
			mBindingLabels[0]->setText(mIpAddress);
		}
		else {
			if (event.type == sf::Event::TextEntered)
			{
				//sf::Keyboard::Key key = event.key.code;
				sf::String input = event.text.unicode;
				mIpAddress += input;
				mBindingLabels[0]->setText(mIpAddress);
			}
		}
	}
	//else if (mBindingButtons[0]->isActive())
	//{
	//	if (event.key.code == sf::Keyboard::Key::Escape) {
	//		mBindingButtons[0]->deactivate();
	//		event;
	//	}
	//	else if (event.key.code == sf::Keyboard::Key::BackSpace) {
	//		if (mUserName.length() > 0) {
	//			mUserName.pop_back();
	//		}
	//		mBindingLabels[2]->setText(mUserName);
	//	}
	//	else {
	//		if (event.type == sf::Event::TextEntered)
	//		{
	//			//sf::Keyboard::Key key = event.key.code;
	//			sf::String input = event.text.unicode;
	//			mUserName += input;
	//			mBindingLabels[2]->setText(mUserName);
	//		}
	//	}
	//}

	mGUIContainer.handleEvent(event);

	return false;
}