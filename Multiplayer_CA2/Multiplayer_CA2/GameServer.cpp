#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include "Foreach.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "Tank.hpp"

#include <SFML/Network/Packet.hpp>

GameServer::RemotePeer::RemotePeer() 
: ready(false)
, timedOut(false)
{
	socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f battlefieldSize)
: mThread(&GameServer::executionThread, this)
, mListeningState(false)
, mClientTimeoutTime(sf::seconds(3.f))
, mMaxConnectedPlayers(10)
,mMaxSpawnPoints(0)
, mConnectedPlayers(0)
, mWorldHeight(768.f)
, mBattleFieldRect(0.f, mWorldHeight - battlefieldSize.y, battlefieldSize.x, battlefieldSize.y)
,mSpawnPoints()
, mBattleFieldScrollSpeed(0.0f)
, mTankCount(0)
, mPeers(1)
, mTankIdentifierCounter(1)
, mWaitingThreadEnd(false)
, mLastSpawnTime(sf::Time::Zero)
, mTimeForNextSpawn(sf::seconds(5.f))
{
	mListenerSocket.setBlocking(false);
	mPeers[0].reset(new RemotePeer());
	mThread.launch();

	mSpawnPoints.push_back(sf::Vector2f(512, 80));
	mSpawnPoints.push_back(sf::Vector2f(50,80));
	mSpawnPoints.push_back(sf::Vector2f(974,80));
	mSpawnPoints.push_back(sf::Vector2f(50, 205));
	mSpawnPoints.push_back(sf::Vector2f(974, 205));
	mSpawnPoints.push_back(sf::Vector2f(50, 330));
	mSpawnPoints.push_back(sf::Vector2f(974, 330));
	mSpawnPoints.push_back(sf::Vector2f(50, 455));
	mSpawnPoints.push_back(sf::Vector2f(974, 455));
	mSpawnPoints.push_back(sf::Vector2f(50, 580));
	mSpawnPoints.push_back(sf::Vector2f(974, 580));
	mSpawnPoints.push_back(sf::Vector2f(50, 705));
	mSpawnPoints.push_back(sf::Vector2f(974, 705));
}

GameServer::~GameServer()
{
	mWaitingThreadEnd = true;
	mThread.wait();
}

void GameServer::notifyPlayerRealtimeChange(sf::Int32 TankIdentifier, sf::Int32 action, bool actionEnabled)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
			packet << TankIdentifier;
			packet << action;
			packet << actionEnabled;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerEvent(sf::Int32 TankIdentifier, sf::Int32 action)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerEvent);
			packet << TankIdentifier;
			packet << action;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerSpawn(sf::Int32 TankIdentifier)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerConnect);
			packet << TankIdentifier << mTankInfo[TankIdentifier].position.x << mTankInfo[TankIdentifier].position.y;
			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{	
		if (!mListeningState)
			mListeningState = (mListenerSocket.listen(ServerPort) == sf::TcpListener::Done);
	}
	else
	{
		mListenerSocket.close();
		mListeningState = false;
	}
}

void GameServer::executionThread()
{
	setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{	
		handleIncomingPackets();
		handleIncomingConnections();

		stepTime += stepClock.getElapsedTime();
		stepClock.restart();

		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Fixed update step
		while (stepTime >= stepInterval)
		{
			mBattleFieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}	
}

void GameServer::tick()
{
	updateClientState();

	// Check for mission success = all planes with position.y < offset
	bool allTanksDone = true;
	FOREACH(auto pair, mTankInfo)
	{
		// As long as one player has not crossed the finish line yet, set variable to false
		if (pair.second.position.y > 0.f)
			allTanksDone = false;
	}
	if (allTanksDone)
	{
		sf::Packet missionSuccessPacket;
		missionSuccessPacket << static_cast<sf::Int32>(Server::MissionSuccess);
		sendToAll(missionSuccessPacket);
	}

	// Remove IDs of Tank that have been destroyed (relevant if a client has two, and loses one)
	for (auto itr = mTankInfo.begin(); itr != mTankInfo.end(); )
	{
		if (itr->second.hitpoints <= 0)
			mTankInfo.erase(itr++);
		else
			++itr;
	}
}

sf::Time GameServer::now() const
{
	return mClock.getElapsedTime();
}

void GameServer::handleIncomingPackets()
{
	bool detectedTimeout = false;
	
	FOREACH(PeerPtr& peer, mPeers)
	{
		if (peer->ready)
		{
			sf::Packet packet;
			while (peer->socket.receive(packet) == sf::Socket::Done)
			{
				// Interpret packet and react to it
				handleIncomingPacket(packet, *peer, detectedTimeout);

				// Packet was indeed received, update the ping timer
				peer->lastPacketTime = now();
				packet.clear();
			}

			if (now() >= peer->lastPacketTime + mClientTimeoutTime)
			{
				peer->timedOut = true;
				detectedTimeout = true;
			}
		}
	}

	if (detectedTimeout)
		handleDisconnections();
}

void GameServer::handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout)
{
	sf::Int32 packetType;
	packet >> packetType;

	switch (packetType)
	{
		case Client::Quit:
		{
			receivingPeer.timedOut = true;
			detectedTimeout = true;
		} break;

		case Client::PlayerEvent:
		{
			sf::Int32 TankIdentifier;
			sf::Int32 action;
			packet >> TankIdentifier >> action;

			notifyPlayerEvent(TankIdentifier, action);
		} break;

		case Client::PlayerRealtimeChange:
		{
			sf::Int32 TankIdentifier;
			sf::Int32 action;
			bool actionEnabled;
			packet >> TankIdentifier >> action >> actionEnabled;
			mTankInfo[TankIdentifier].realtimeActions[action] = actionEnabled;
			notifyPlayerRealtimeChange(TankIdentifier, action, actionEnabled);
		} break;

		case Client::RequestCoopPartner:
		{
			receivingPeer.TankIdentifiers.push_back(mTankIdentifierCounter);
			mTankInfo[mTankIdentifierCounter].position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
			mTankInfo[mTankIdentifierCounter].hitpoints = 100;
			mTankInfo[mTankIdentifierCounter].rotation = 0.f;

			sf::Packet requestPacket;
			requestPacket << static_cast<sf::Int32>(Server::AcceptCoopPartner);
			requestPacket << mTankIdentifierCounter;
			requestPacket << mTankInfo[mTankIdentifierCounter].position.x;
			requestPacket << mTankInfo[mTankIdentifierCounter].position.y;

			receivingPeer.socket.send(requestPacket);
			mTankCount++;

			// Inform every other peer about this new plane
			FOREACH(PeerPtr& peer, mPeers)
			{
				if (peer.get() != &receivingPeer && peer->ready)
				{
					sf::Packet notifyPacket;
					notifyPacket << static_cast<sf::Int32>(Server::PlayerConnect);
					notifyPacket << mTankIdentifierCounter;
					notifyPacket << mTankInfo[mTankIdentifierCounter].position.x;
					notifyPacket << mTankInfo[mTankIdentifierCounter].position.y;
					peer->socket.send(notifyPacket);
				}
			}
			mTankIdentifierCounter++;
		} break;

		case Client::PositionUpdate:
		{
			sf::Int32 numTanks;
			packet >> numTanks;

			for (sf::Int32 i = 0; i < numTanks; ++i)
			{
				sf::Int32 TankIdentifier;
				sf::Int32 TankHitpoints;
				float TankRotation;
				sf::Vector2f TankPosition;
				packet >> TankIdentifier >> TankPosition.x >> TankPosition.y >> TankHitpoints >> TankRotation;
				mTankInfo[TankIdentifier].position = TankPosition;
				mTankInfo[TankIdentifier].hitpoints = TankHitpoints;
				mTankInfo[TankIdentifier].rotation = TankRotation;
			}
		} break;

		case Client::GameEvent:
		{
			sf::Int32 action;
			float x;
			float y;

			packet >> action;
			packet >> x;
			packet >> y;

			// Enemy explodes: With certain probability, drop pickup
			// To avoid multiple messages spawning multiple pickups, only listen to first peer (host)
			if (action == GameActions::EnemyExplode && randomInt(3) == 0 && &receivingPeer == mPeers[0].get())
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Server::SpawnPickup);
				packet << static_cast<sf::Int32>(randomInt(Pickup::TypeCount));
				packet << x;
				packet << y;

				sendToAll(packet);
			}
		}
	}
}

void GameServer::updateClientState()
{
	sf::Packet updateClientStatePacket;
	updateClientStatePacket << static_cast<sf::Int32>(Server::UpdateClientState);
	updateClientStatePacket << static_cast<float>(mBattleFieldRect.top + mBattleFieldRect.height);
	updateClientStatePacket << static_cast<sf::Int32>(mTankInfo.size());

	FOREACH(auto Tank, mTankInfo)
		updateClientStatePacket << Tank.first << Tank.second.position.x << Tank.second.position.y << Tank.second.rotation;

	sendToAll(updateClientStatePacket);
}

void GameServer::handleIncomingConnections()
{
	if (!mListeningState)
		return;

	if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket) == sf::TcpListener::Done)
	{
		// order the new client to spawn its own plane ( player 1 )
		mTankInfo[mTankIdentifierCounter].position = sf::Vector2f(mSpawnPoints[mMaxSpawnPoints].x, mSpawnPoints[mMaxSpawnPoints].y);//sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
		mTankInfo[mTankIdentifierCounter].hitpoints = 100;
		mTankInfo[mTankIdentifierCounter].rotation = 0.f;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(Server::SpawnSelf);
		packet << mTankIdentifierCounter;
		packet << mTankInfo[mTankIdentifierCounter].position.x;
		packet << mTankInfo[mTankIdentifierCounter].position.y;
		
		mPeers[mConnectedPlayers]->TankIdentifiers.push_back(mTankIdentifierCounter);
		
		broadcastMessage("New player!");
		informWorldState(mPeers[mConnectedPlayers]->socket);
		notifyPlayerSpawn(mTankIdentifierCounter++);

		mPeers[mConnectedPlayers]->socket.send(packet);
		mPeers[mConnectedPlayers]->ready = true;
		mPeers[mConnectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
		mTankCount++;
		mConnectedPlayers++;

		if (mConnectedPlayers >= mMaxConnectedPlayers)
			setListening(false);
		else // Add a new waiting peer
			mPeers.push_back(PeerPtr(new RemotePeer()));

		mMaxSpawnPoints++;

		if (mMaxSpawnPoints >= mSpawnPoints.size()) {
			mMaxSpawnPoints = 1;
		}
	}
}

void GameServer::handleDisconnections()
{
	for (auto itr = mPeers.begin(); itr != mPeers.end(); )
	{
		if ((*itr)->timedOut)
		{
			// Inform everyone of the disconnection, erase 
			FOREACH(sf::Int32 identifier, (*itr)->TankIdentifiers)
			{
				sendToAll(sf::Packet() << static_cast<sf::Int32>(Server::PlayerDisconnect) << identifier);

				mTankInfo.erase(identifier);
			}

			mConnectedPlayers--;
			mTankCount -= (*itr)->TankIdentifiers.size();

			itr = mPeers.erase(itr);

			// Go back to a listening state if needed
			if (mConnectedPlayers < mMaxConnectedPlayers)
			{
				mPeers.push_back(PeerPtr(new RemotePeer()));
				setListening(true);
			}
				
			broadcastMessage("An ally has disconnected.");
		}
		else
		{
			++itr;
		}
	}
}

// Tell the newly connected peer about how the world is currently
void GameServer::informWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::InitialState);
	packet << mWorldHeight << mBattleFieldRect.top + mBattleFieldRect.height;
	packet << static_cast<sf::Int32>(mTankCount);

	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			FOREACH(sf::Int32 identifier, mPeers[i]->TankIdentifiers)
				packet << identifier << mTankInfo[identifier].position.x << mTankInfo[identifier].position.y << mTankInfo[identifier].hitpoints << mTankInfo[identifier].rotation;
		}
	}

	socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::BroadcastMessage);
			packet << message;

			mPeers[i]->socket.send(packet);
		}	
	}
}

void GameServer::sendToAll(sf::Packet& packet)
{
	FOREACH(PeerPtr& peer, mPeers)
	{
		if (peer->ready)
			peer->socket.send(packet);
	}
}
