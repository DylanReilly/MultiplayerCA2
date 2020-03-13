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


		Tank*							getTank(int identifier) const;
		sf::FloatRect						getBattlefieldBounds() const;

		void								createPickup(sf::Vector2f position, Pickup::Type type);
		bool								pollGameAction(GameActions::Action& out);


	private:
		void								loadTextures();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();
		void								handleCollisions();
		void								updateSounds();

		void								addObstacles();
		void								buildScene();
		void								destroyEntitiesOutsideView();

		void								addObstacle(Obstacles::ID type, float posX, float posY, float rotation, float scaleX, float scaleY, Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale); //Info for adding an obstacle - Jason Lynch
		void								addPickups(); //Adds pickups to scene - Jason Lynch 
		void								playerOneBase(); //Adds obstacles near player one to scene - Jason Lynch 
		void								playerTwoBase(); //Adds obstacles near player two to scene - Jason Lynch 
		void								teslaobstacles();//Adds obstacles near tesla pickup to scene - Jason Lynch 
		void								NukeObstacles();//Adds obstacles near nuke pickup to scene - Jason Lynch 
		void								borderObstacles();//Adds obstacles around edge of map near other pickups - Jason Lynch 
		void								addPickup(TankPickup::ID type, float x, float y, float rotation, float scaleX, float scaleY); //Info for creating a pickup - Jason Lynch 
		void								spawnObstacles(); //Spawns obstacles into scene - Jason Lynch 
		void								spawnPickups(); //Spawns pickups into scene - Jason Lynch 


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
			ObstacleSpawnPoint(Obstacles::ID type, float x, float y, float rotation, float scaleX, float scaleY, Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale)
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

			Obstacles::ID type;
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

		struct PickupSpawnPoint //Pickup spawn point and relevant info. Based off above struct - Jason Lynch
		{
			PickupSpawnPoint(TankPickup::ID type, float x, float y, float rotation, float scaleX, float scaleY)
				: type(type)
				, x(x)
				, y(y)
				, rotation(rotation)
				, scaleX(scaleX)
				, scaleY(scaleY)
			{
			}

			TankPickup::ID type;
			float x;
			float y;
			float rotation;
			float scaleX;
			float scaleY;
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
		float								mScrollSpeed;
		float								mScrollSpeedCompensation;
		std::vector<Tank*>				mPlayerTanks;

		std::vector<SpawnPoint>				mEnemySpawnPoints;
		std::vector<Tank*>				mActiveEnemies;

		BloomEffect							mBloomEffect;

		bool								mNetworkedWorld;
		NetworkNode*						mNetworkNode;
		SpriteNode*							mFinishSprite;

		sf::Vector2f						mObstacleSpawnPosition;
		std::vector<ObstacleSpawnPoint>		mObstacles; //Holds obstacle spawn points - Jason Lynch
};

#endif // BOOK_WORLD_HPP
