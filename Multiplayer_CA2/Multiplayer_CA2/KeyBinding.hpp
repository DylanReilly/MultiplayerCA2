#ifndef BOOK_KEYBINDING_HPP
#define BOOK_KEYBINDING_HPP

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <map>
#include <vector>


namespace PlayerAction
{
	enum Type
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Fire,
		LaunchMissile,
		Count
	};
}

class KeyBinding
{
	public:
		typedef PlayerAction::Type Action;


	public:
		explicit				KeyBinding(int controlPreconfiguration);

		void					assignKey(Action action, sf::Keyboard::Key key);
		void					assignJoystickButton(Action action, int buttonNumber);
		sf::Keyboard::Key		getAssignedKey(Action action) const;
		int						getAssignedJoypadButton(Action action) const;

		bool					checkAction(sf::Keyboard::Key key, Action& out) const;
		bool					checkControllerAction(int buttonVal, Action& out) const;
		std::vector<Action>		getRealtimeActions() const;


	private:
		void					initializeActions();


	private:
		std::map<sf::Keyboard::Key, Action>		mKeyMap;
		std::map<int, Action> mControllerBinding; //Holds controller button and its action - Jason Lynch
		int	mController;
};

bool					isRealtimeAction(PlayerAction::Type action);

#endif // BOOK_KEYBINDING_HPP
