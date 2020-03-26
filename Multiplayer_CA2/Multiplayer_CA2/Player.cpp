#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Tank.hpp"
#include "Utility.hpp"
#include "Foreach.hpp"
#include "NetworkProtocol.hpp"

#include <SFML/Network/Packet.hpp>

#include <map>
#include <string>
#include <algorithm>


using namespace std::placeholders;

//Dylan Reilly
//Added to allow the tank to rotate rather than move left and right
struct TankRotator
{
	TankRotator(float r, int identifier)
		: rotation(r)
		, TankID(identifier)
	{
	}

	void operator() (Tank& Tank, sf::Time) const
	{
		if (Tank.getIdentifier() == TankID)
			Tank.rotate(rotation);
	}

	float rotation;
	int TankID;
};

//Dylan Reilly
//Modified to move in the direction the tank is facing, not just up and down
struct TankMover
{
	TankMover(int x, int identifier)
	: direction(x)
	, TankID(identifier)
	{
	}

	void operator() (Tank& Tank, sf::Time) const
	{
		if (Tank.getIdentifier() == TankID)
		{
			if (direction == 1)
			{
				Tank.move(Tank.getMaxSpeed() * sin(toRadian(Tank.getRotation())), Tank.getMaxSpeed() * -cos(toRadian(Tank.getRotation())));
			}
			else
			{
				Tank.move(Tank.getMaxSpeed() * -sin(toRadian(Tank.getRotation())), Tank.getMaxSpeed() * cos(toRadian(Tank.getRotation())));
			}
		}
	}

	int direction;
	int TankID;
};

struct TankFireTrigger
{
	TankFireTrigger(int identifier)
	: TankID(identifier)
	{
	}

	void operator() (Tank& Tank, sf::Time) const
	{
		if (Tank.getIdentifier() == TankID)
			Tank.fire();
	}

	int TankID;
};

struct TankMissileTrigger
{
	TankMissileTrigger(int identifier)
	: TankID(identifier)
	{
	}

	void operator() (Tank& Tank, sf::Time) const
	{
		if (Tank.getIdentifier() == TankID)
			Tank.launchMissile();
	}

	int TankID;
};


Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding)
	: mKeyBinding(binding)
	, mCurrentMissionStatus(MissionRunning)
	, mIdentifier(identifier)
	, mSocket(socket)
{
	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's Tank
	FOREACH(auto & pair, mActionBinding)
		pair.second.category = Category::Tank;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	// Event
	if (event.type == sf::Event::KeyPressed)
	{
		Action action;
		if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && !isRealtimeAction(action))
		{
			// Network connected -> send event over network
			if (mSocket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Client::PlayerEvent);
				packet << mIdentifier;
				packet << static_cast<sf::Int32>(action);		
				mSocket->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				commands.push(mActionBinding[action]);
			}
		}
	}

	if (event.type == sf::Event::JoystickButtonPressed)
	{
		Action action;
		if (mKeyBinding && mKeyBinding->checkControllerAction(event.joystickButton.button, action) && !isRealtimeAction(action))
		{
			// Network connected -> send event over network
			if (mSocket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Client::PlayerEvent);
				packet << mIdentifier;
				packet << static_cast<sf::Int32>(action);
				mSocket->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				commands.push(mActionBinding[action]);
			}
		}
	}

	// Realtime change (network connected)
	if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && mSocket)
	{
		Action action;
		if (mKeyBinding && mKeyBinding->checkAction(event.key.code, action) && isRealtimeAction(action))
		{
			// Send realtime change over network
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
			packet << mIdentifier;
			packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::KeyPressed);
			mSocket->send(packet);
		}
	}

	if ((event.type == sf::Event::JoystickButtonPressed || event.type == sf::Event::JoystickButtonReleased) && mSocket)
	{
		Action action;
		if (mKeyBinding && mKeyBinding->checkControllerAction(event.joystickButton.button, action) && isRealtimeAction(action))
		{
			// Send realtime change over network
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
			packet << mIdentifier;
			packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::JoystickButtonPressed);
			mSocket->send(packet);
		}
	}
}

bool Player::isLocal() const
{
	// No key binding means this player is remote
	return mKeyBinding != nullptr;
}

void Player::disableAllRealtimeActions()
{
	FOREACH(auto& action, mActionProxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
		packet << mIdentifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		mSocket->send(packet);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a networked game and local player or just a single player game
	if ((mSocket && isLocal()) || !mSocket)
	{
		// Lookup all actions and push corresponding commands to queue
		std::vector<Action> activeActions = mKeyBinding->getRealtimeActions();
		FOREACH(Action action, activeActions)
			commands.push(mActionBinding[action]);
	}
}

void Player::handleRealtimeNetworkInput(CommandQueue& commands)
{
	if (mSocket && !isLocal())
	{
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		FOREACH(auto pair, mActionProxies)
		{
			if (pair.second && isRealtimeAction(pair.first))
				commands.push(mActionBinding[pair.first]);
		}
	}
}

void Player::handleNetworkEvent(Action action, CommandQueue& commands)
{
	commands.push(mActionBinding[action]);
}

void Player::handleNetworkRealtimeChange(Action action, bool actionEnabled)
{
	mActionProxies[action] = actionEnabled;
}

void Player::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	mActionBinding[PlayerAction::MoveLeft].action      = derivedAction<Tank>(TankRotator(-5.f, mIdentifier));
	mActionBinding[PlayerAction::MoveRight].action     = derivedAction<Tank>(TankRotator(5.f, mIdentifier));
	mActionBinding[PlayerAction::MoveUp].action        = derivedAction<Tank>(TankMover(0, mIdentifier));
	mActionBinding[PlayerAction::MoveDown].action      = derivedAction<Tank>(TankMover(1, mIdentifier));
	mActionBinding[PlayerAction::Fire].action          = derivedAction<Tank>(TankFireTrigger(mIdentifier));
	mActionBinding[PlayerAction::LaunchMissile].action = derivedAction<Tank>(TankMissileTrigger(mIdentifier));
}

//mActionBinding[ActionID::MoveUp].action = derivedAction<Tank>([](Tank& a, sf::Time) { a.move(1.5f * -sin(toRadian(a.getRotation())), 1.5f * cos(toRadian(a.getRotation()))); });
//mActionBinding[ActionID::MoveDown].action = derivedAction<Tank>([](Tank& a, sf::Time) { a.move(1.5f * sin(toRadian(a.getRotation())), 1.5f * -cos(toRadian(a.getRotation()))); });
