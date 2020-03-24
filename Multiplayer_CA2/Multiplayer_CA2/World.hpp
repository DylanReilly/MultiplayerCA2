#ifndef BOOK_WORLD_HPP
#define BOOK_WORLD_HPP

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Tank.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"
#include "Pickup.hpp"
#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"
#include "NetworkProtocol.hpp"
#include "Obstacle.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>


// Forward declaration
namespace sf
{
	class RenderTarget;
}

class NetworkNode;

class World : private sf::NonCopyable
{
	public:
											World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked = false);
		void								update(sf::Time dt);
		void								draw();

		sf::FloatRect						getViewBounds() const;		
		CommandQueue&						getCommandQueue();
		Tank*							addTank(int identifier);
		void								removeTank(int identifier);
		void								setCurrentBattleFieldPosition(float lineY);
		void								setWorldHeight(float height);

		bool 								hasAlivePlayer() const;
		bool 								hasPlayerReachedEnd() const;


		Tank*								getTank(int identifier) const;
		sf::FloatRect						getBattlefieldBounds() const;

		void								createPickup(sf::Vector2f position, Pickup::Type type);
		bool								pollGameAction(GameActions::Action& out);


	private:
		void								loadTextures();
		void								adaptPlayerPosition();
		void								handleCollisions();
		void								updateSounds();
		void								playerOneBase(); //Adds obstacles near player one to scene - Jason Lynch 
		void								addObstacle(Obstacle::Type type, float posX, float posY, float rotation, float scaleX, float scaleY, Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale); //Info for adding an obstacle - Jason Lynch
		void								spawnObstacles();
		void								addBuildings();

		void								buildScene();
		void								destroyEntitiesOutsideView();


	private:
		enum Layer
		{
			Background,
			LowerAir,
			UpperAir,
			LayerCount
		};

		struct SpawnPoint 
		{
			SpawnPoint(Tank::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
			{
			}

			Tank::Type type;
			float x;
			float y;
		};

		struct ObstacleSpawnPoint //Spawn point for obstacles and all other needed info. Based off above struct - Jason Lynch 
		{
			ObstacleSpawnPoint(Obstacle::Type type, float x, float y, float rotation, float scaleX, float scaleY, Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale)
				: type(type)
				, x(x)
				, y(y)
				, rotation(rotation)
				, scaleX(scaleX)
				, scaleY(scaleY)
				, deathAnimation(deathAnimation)
				, frameSize(frameSize)
				, numberOfFrames(numberOfFrames)
				, seconds(seconds)
				, scale(scale)
			{
			}

			Obstacle::Type type;
			float x;
			float y;
			float rotation;
			float scaleX;
			float scaleY;
			Textures::ID deathAnimation;
			sf::Vector2i frameSize;
			int numberOfFrames;
			int seconds;
			sf::Vector2f scale;
		};


	private:
		sf::RenderTarget&					mTarget;
		sf::RenderTexture					mSceneTexture;
		sf::View							mWorldView;
		TextureHolder						mTextures;
		FontHolder&							mFonts;
		SoundPlayer&						mSounds;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		sf::Vector2f						mObstacleSpawnPosition;
		float								mScrollSpeed;
		float								mScrollSpeedCompensation;
		std::vector<Tank*>					mPlayerTanks;

		std::vector<ObstacleSpawnPoint>		mObstacles; //Holds obstacle spawn points - Jason Lynch
		std::vector<SpawnPoint>				mEnemySpawnPoints;
		std::vector<Tank*>					mActiveEnemies;

		BloomEffect							mBloomEffect;

		bool								mNetworkedWorld;
		NetworkNode*						mNetworkNode;
		SpriteNode*							mFinishSprite;
};

#endif // BOOK_WORLD_HPP
