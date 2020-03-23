#ifndef BOOK_STATEIDENTIFIERS_HPP
#define BOOK_STATEIDENTIFIERS_HPP


namespace States
{
	enum ID
	{
		None,
		Title,
		Menu,
		HowToPlay,
		Game,
		Loading,
		Pause,
		NetworkPause,
		Settings,
		Keyboardcontrol,
		GameOver,
		MissionSuccess,
		HostGame,
		HostIpEntry,
		JoinGame,
		JoinIpEntry
	};
}

#endif // BOOK_STATEIDENTIFIERS_HPP
