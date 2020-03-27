#include "Application.hpp"
#include "IpConfigs.hpp"
#include "Utility.hpp"
#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "TitleState.hpp"
#include "GameState.hpp"
#include "MultiplayerGameState.hpp"
#include "HostIpEntryState.hpp"
#include "JoinIpEntryState.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "HowToPlayState.hpp"
#include "SettingsState.hpp"
#include "KeyboardControlState.hpp"
#include "GameOverState.hpp"
#include "NetworkProtocol.hpp"

const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application()
: mWindow(sf::VideoMode(1024, 768), "Network", sf::Style::Close)
, mTextures()
, mFonts()
, mMusic()
, mSounds()
, mKeyBinding1(1)
, mKeyBinding2(2)
, mStateStack(State::Context(mWindow, mTextures, mFonts, mMusic, mSounds, mKeyBinding1, mKeyBinding2))
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);
	mWindow.setVerticalSyncEnabled(true);

	mFonts.load(Fonts::Main, 	"Media/EvilEmpire.otf");//Added font - Jason Lynch 

	mTextures.load(Textures::TitleScreen,	"Media/Textures/Title.png");
	mTextures.load(Textures::HowToPlay,		"Media/Textures/HowToPlay.png");//Added background - Jason Lynch 
	mTextures.load(Textures::Buttons,		"Media/Textures/Buttons.png");
	
	mStatisticsText.setFont(mFonts.get(Fonts::Main));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(States::Title);

	mMusic.setVolume(3.f);
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (mWindow.isOpen())
	{
		sf::Time dt = clock.restart();
		timeSinceLastUpdate += dt;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			// Check inside this loop, because stack might be empty before update() call
			if (mStateStack.isEmpty())
				mWindow.close();
		}

		updateStatistics(dt);
		render();
	}
}

void Application::processInput()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		mStateStack.handleEvent(event);

		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Application::update(sf::Time dt)
{
	mStateStack.update(dt);
}

void Application::render()
{
	mWindow.clear();

	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);

	mWindow.display();
}

void Application::updateStatistics(sf::Time dt)
{
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString("FPS: " + toString(mStatisticsNumFrames));

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates()
{
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<HowToPlayState>(States::HowToPlay); //Added state - Jason Lynch 
	mStateStack.registerState<GameState>(States::Game);
	mStateStack.registerState<MultiplayerGameState>(States::HostGame, true, HostIp);
	mStateStack.registerState<HostIpEntryState>(States::HostIpEntry);
	mStateStack.registerState<MultiplayerGameState>(States::JoinGame, false, JoinIp);
	mStateStack.registerState<JoinIpEntryState>(States::JoinIpEntry);
	mStateStack.registerState<PauseState>(States::Pause);
	mStateStack.registerState<PauseState>(States::NetworkPause, true);
	mStateStack.registerState<SettingsState>(States::Settings);
	mStateStack.registerState<KeyboardControlState>(States::Keyboardcontrol);
	mStateStack.registerState<GameOverState>(States::GameOver, "Mission Failed!");
	mStateStack.registerState<GameOverState>(States::MissionSuccess, "Mission Successful!");
}
