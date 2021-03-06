//D00137655 - Jason Lynch
//D00194504 - Dylan Reilly
#include "World.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Foreach.hpp"
#include "TextNode.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "Utility.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>


World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mWorldView(outputTarget.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	, mSounds(sounds)
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, mWorldView.getSize().y)
	, mSpawnPosition(0.0f, 0.0f)
	, mScrollSpeed(0.0f)
	, mObstacleSpawnPosition(mWorldView.getSize().x * .25f, mWorldView.getSize().y / 2.f)
	, mScrollSpeedCompensation(0.0f)
	, mPlayerTanks()
	, mObstacles()
	, mPickups()
	, mEnemySpawnPoints()
	, mActiveEnemies()
	, mNetworkedWorld(networked)
	, mNetworkNode(nullptr)
	, mFinishSprite(nullptr)
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mWorldView.getSize().x / 2.f, mWorldView.getSize().y / 2.f);
}

void World::update(sf::Time dt)
{
	FOREACH(Tank * a, mPlayerTanks)
		a->setVelocity(0.f, 0.f);

	// Setup commands to destroy entities, and guide missiles
	destroyEntitiesOutsideView();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);

	// Collision detection and response (may destroy entities)
	handleCollisions();

	// Remove Tanks that were destroyed (World::removeWrecks() only destroys the entities, not the pointers in mPlayerTank)
	auto firstToRemove = std::remove_if(mPlayerTanks.begin(), mPlayerTanks.end(), std::mem_fn(&Tank::isMarkedForRemoval));
	mPlayerTanks.erase(firstToRemove, mPlayerTanks.end());

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);

	adaptPlayerPosition();
	spawnObstacles();
	spawnPickups();
	updateSounds();
}

void World::draw()
{
	if (PostEffect::isSupported())
	{
		mSceneTexture.clear();
		mSceneTexture.setView(mWorldView);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		mBloomEffect.apply(mSceneTexture, mTarget);
	}
	else
	{
		mTarget.setView(mWorldView);
		mTarget.draw(mSceneGraph);
	}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

Tank* World::getTank(int identifier) const
{
	FOREACH(Tank * a, mPlayerTanks)
	{
		if (a->getIdentifier() == identifier)
			return a;
	}

	return nullptr;
}

void World::removeTank(int identifier)
{
	Tank* Tank = getTank(identifier);
	if (Tank)
	{
		Tank->destroy();
		mPlayerTanks.erase(std::find(mPlayerTanks.begin(), mPlayerTanks.end(), Tank));
	}
}

Tank* World::addTank(int identifier)
{
	Tank::Type type;

	std::ifstream fileIn;
	int scores;
	fileIn.open("scores.txt");
	if (!fileIn)
	{
		std::ofstream outputFile("scores.txt");
		scores = 0;
		outputFile << scores;
	}
	fileIn >> scores;
	fileIn.close();

	if (identifier == 1) {
		type = Tank::Type::HostLmg;
	}
	else if (identifier % 2 > 0)
	{
		if (scores >= 50)
		{
			type = Tank::Type::GreenLmg3;
		}
		else if (scores >= 20)
		{
			type = Tank::Type::GreenLmg2;
		}
		else
		{
			type = Tank::Type::GreenLmg;
		}
	}
	else
	{
		if (scores >= 50)
		{
			type = Tank::Type::RedLmg3;
		}
		else if (scores >= 20)
		{
			type = Tank::Type::RedLmg2;
		}
		else
		{
			type = Tank::Type::RedLmg;
		}
	}

	std::unique_ptr<Tank> player(new Tank(type, mTextures, mFonts));
	player->setPosition(mWorldView.getCenter());
	player->setIdentifier(identifier);
	player->setScale(0.6f, 0.6f);

	mPlayerTanks.push_back(player.get());
	mSceneLayers[LowerAir]->attachChild(std::move(player));
	return mPlayerTanks.back();
}

void World::createPickup(sf::Vector2f position, Pickup::Type type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, mTextures));
	pickup->setPosition(position);
	pickup->setVelocity(0.f, 1.f);
	mSceneLayers[LowerAir]->attachChild(std::move(pickup));
}

bool World::pollGameAction(GameActions::Action& out)
{
	return mNetworkNode->pollGameAction(out);
}

void World::setCurrentBattleFieldPosition(float lineY)
{
	mWorldView.setCenter(mWorldView.getCenter().x, lineY - mWorldView.getSize().y / 2);
	mSpawnPosition.y = mWorldBounds.height;
}

void World::setWorldHeight(float height)
{
	mWorldBounds.height = height;
}

bool World::hasAlivePlayer() const
{
	return mPlayerTanks.size() > 0;
}

bool World::hasPlayerReachedEnd() const
{
	if (Tank* Tank = getTank(1))
		return !mWorldBounds.contains(Tank->getPosition());
	else
		return false;
}

void World::loadTextures()
{
	//Both added some new textures - Jason Lynch, Dylan Reilly
	mTextures.load(Textures::ID::Tanks, "Media/Textures/TankSpriteSheet.png");
	mTextures.load(Textures::ID::HostTankLmg, "Media/Textures/HostTank.png");
	mTextures.load(Textures::ID::HostTankHmg, "Media/Textures/HostTankHmg.png");
	mTextures.load(Textures::ID::HostTankGatling, "Media/Textures/HostTankGatling.png");
	mTextures.load(Textures::ID::HostTankTesla, "Media/Textures/HostTankTesla.png");
	mTextures.load(Textures::ID::Entities, "Media/Textures/Entities.png");
	mTextures.load(Textures::ID::Barrel, "Media/Textures/Barell_01.png");
	mTextures.load(Textures::ID::Wall, "Media/Textures/Arena/Blocks/Block_B_01.png");
	mTextures.load(Textures::ID::DestructableWall, "Media/Textures/Arena/Buildings/Building_B_02.png");
	mTextures.load(Textures::ID::Jungle, "Media/Textures/Gamebackground.png");
	mTextures.load(Textures::ID::Explosion, "Media/Textures/Explosion.png");
	mTextures.load(Textures::ID::Particle, "Media/Textures/Particle.png");
	mTextures.load(Textures::ID::FinishLine, "Media/Textures/FinishLine.png");
	mTextures.load(Textures::ID::LmgBullet, "Media/Textures/Bullet.png");
	mTextures.load(Textures::ID::HmgBullet, "Media/Textures/HeavyBullet.png");
	mTextures.load(Textures::ID::GatlingBullet, "Media/Textures/Bullet.png");
	mTextures.load(Textures::ID::TeslaBullet, "Media/Textures/LightningBallScaled.png");
	mTextures.load(Textures::ID::HeavyGunPickup, "Media/Textures/Arena/Props/Dot_A.png");
	mTextures.load(Textures::ID::GatlingGunPickup, "Media/Textures/Arena/Props/Dot_B.png");
	mTextures.load(Textures::ID::TeslaGunPickup, "Media/Textures/Arena/Props/Artifact.png");
	mTextures.load(Textures::ID::Nuke, "Media/Textures/NukeBomb.png");
	mTextures.load(Textures::ID::NukeExplosion, "Media/Textures/Nuke.png");
	mTextures.load(Textures::ID::Repair, "Media/Textures/Health.png");
	mTextures.load(Textures::ID::FireRate, "Media/Textures/Speed.png");
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	FOREACH(Tank * Tank, mPlayerTanks)
	{
		sf::Vector2f position = Tank->getPosition();
		position.x = std::max(position.x, viewBounds.left + borderDistance);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
		position.y = std::max(position.y, viewBounds.top + borderDistance);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
		Tank->setPosition(position);
	}
}

void World::addBuildings()
{
	greenBase();
	redBase();
	hostBase();
	worldWalls();
}

//Sets up obstacles - Jason Lynch 
void World::addObstacle(Obstacle::Type type, float posX, float posY, float rotation, float scaleX, float scaleY, Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale) //Add obstacles to Vector of ObstacleSpawnPoint structs - Jason Lynch
{
	ObstacleSpawnPoint spawn(type, posX, posY, rotation, scaleX, scaleY, deathAnimation, frameSize, numberOfFrames, seconds, scale);
	mObstacles.push_back(spawn);
}

//Popultaes world with obstacles - Jason Lynch 
void World::greenBase() {
	addObstacle(Obstacle::Type::DestructableWall, 100, 140, 90.0f, .72f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 100, 650, 90.0f, .72f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	//addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x - 30, mObstacleSpawnPosition.y + 170, 0, .3f, .1f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	//addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x -30, mObstacleSpawnPosition.y - 170, 0, .3f, .1f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

void World::redBase() {
	addObstacle(Obstacle::Type::DestructableWall, 924, 140, 90.0f, .72f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 924, 650, 90.0f, .72f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	//addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 460, mObstacleSpawnPosition.y, 90.0f, .72f, .1f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	//addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 540, mObstacleSpawnPosition.y + 170, 0, .3f, .1f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	//addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 540, mObstacleSpawnPosition.y - 170, 0, .3f, .1f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

void World::hostBase() {
	addObstacle(Obstacle::Type::DestructableWall, 370, 10, 90.0f, .5f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 660, 10, 90.0f, .5f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 400, 130, 0.0f, .16f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 630, 130, 0.0f, .16f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

}

void World::worldWalls() {
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x - 30, mObstacleSpawnPosition.y - 230, 90.0f, .3f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x - 90, mObstacleSpawnPosition.y - 250, 0.0f, .25f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 30, mObstacleSpawnPosition.y + 250, 0.0f, .2f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 30, mObstacleSpawnPosition.y + 190, 90.0f, .2f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 30, mObstacleSpawnPosition.y + 130, 0.0f, .2f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::DestructableWall, 512, 400, 90.0f, .4f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 512, 400, 0.0f, .3f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::DestructableWall, 450, 768, 90.0f, .3f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, 580, 768, 90.0f, .3f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 480, mObstacleSpawnPosition.y + 250, 0.0f, .2f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 480, mObstacleSpawnPosition.y + 190, 90.0f, .2f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 480, mObstacleSpawnPosition.y + 130, 0.0f, .2f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 540, mObstacleSpawnPosition.y - 230, 90.0f, .3f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::DestructableWall, mObstacleSpawnPosition.x + 600, mObstacleSpawnPosition.y - 250, 0.0f, .25f, .07f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

}

//Spawn obstacles, set scale, rotation, and position - Jason Lynch
void World::spawnObstacles()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!mObstacles.empty())
	{
		ObstacleSpawnPoint spawn = mObstacles.back();

		std::unique_ptr<Obstacle> obstacle(new Obstacle(spawn.type, mTextures, mFonts, spawn.deathAnimation, spawn.frameSize, spawn.numberOfFrames, spawn.seconds, spawn.scale));
		obstacle->setScale(spawn.scaleX, spawn.scaleY);
		obstacle->setPosition(spawn.x, spawn.y);
		obstacle->setRotation(spawn.rotation);
		mSceneLayers[Layer::LowerAir]->attachChild(std::move(obstacle));

		// Object is spawned, remove from the list to spawn
		mObstacles.pop_back();
	}
}

//Spawn pickups, set scale, rotation, and position - Jason Lynch
void World::spawnPickups()//Spawn Tank pickups, set scale, rotation, and position - Jason Lynch
{
	// Spawn all pickups - Jason Lynch
	while (!mPickups.empty())
	{
		PickupSpawnPoint spawn = mPickups.back();

		std::unique_ptr<Pickup> pickup(new Pickup(spawn.type, mTextures));
		pickup->setScale(spawn.scaleX, spawn.scaleY);
		pickup->setRotation(spawn.rotation);
		pickup->setPosition(spawn.x, spawn.y);

		mSceneLayers[static_cast<int>(Layer::LowerAir)]->attachChild(std::move(pickup));

		// Enemy is spawned, remove from the list to spawn
		mPickups.pop_back();
	}
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	FOREACH(SceneNode::Pair pair, collisionPairs)
	{
		if (matchesCategories(pair, Category::AlliedTank, Category::Pickup) || matchesCategories(pair, Category::EnemyTank, Category::Pickup) || matchesCategories(pair, Category::HostTank, Category::Pickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			pickup.destroy();
			player.playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
		}

		//Added new host tank detection - Jason Lynch
		else if (matchesCategories(pair, Category::AlliedTank, Category::EnemyProjectile) || matchesCategories(pair, Category::AlliedTank, Category::HostProjectile) || matchesCategories(pair, Category::EnemyTank, Category::AlliedProjectile) || matchesCategories(pair, Category::EnemyTank, Category::HostProjectile) || matchesCategories(pair, Category::HostTank, Category::AlliedProjectile) || matchesCategories(pair, Category::HostTank, Category::EnemyProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			// Apply projectile damage to Tank, destroy projectile
			tank.damage(projectile.getDamage());
			projectile.destroy();

			if (tank.getHitpoints() <= projectile.getDamage()) //TODO - Recode to only add score for the one person
			{
				std::ifstream fileIn;
				int scores;
				fileIn.open("scores.txt");
				if (!fileIn)
				{
					std::ofstream outputFile("scores.txt");
					scores = 0;
					outputFile << scores;
				}
				fileIn >> scores;
				scores += 5;
				std::ofstream outputFile("scores.txt");
				outputFile << scores;
				fileIn.close();
			}
		}

		//Destroy projectile when it hits a wall - Dylan
		else if (matchesCategories(pair, Category::AlliedProjectile, Category::Collidable) || matchesCategories(pair, Category::EnemyProjectile, Category::Collidable) || matchesCategories(pair, Category::HostProjectile, Category::Collidable))
		{
			auto& projectile = static_cast<Projectile&>(*pair.first);
			auto& obstacle = static_cast<Obstacle&>(*pair.second);

			if (obstacle.getType() == Obstacle::Type::Barrel) {
				obstacle.damage(projectile.getDamage());
			}

			//Destroy projectile when it hits a wall
			projectile.destroy();
		}

		//Collision to stop tanks phasing through walls - Dylan			Added in host detection - Jason Lynch 
		else if (matchesCategories(pair, Category::AlliedTank, Category::Collidable) || matchesCategories(pair, Category::EnemyTank, Category::Collidable) || matchesCategories(pair, Category::HostTank, Category::Collidable))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& obstacle = static_cast<Obstacle&>(*pair.second);

			float borderDistance = 40.f;
			sf::Vector2f position = tank.getPosition();

			if (obstacle.getType() == Obstacle::Type::Barrel) {
				tank.damage(obstacle.getDamage());
				obstacle.destroy();
				tank.playLocalSound(mCommandQueue, SoundEffect::TankHitBullet);
			}
			//Left of object
			if (tank.getPosition().x < obstacle.getBoundingRect().left)
			{
				if (tank.getPosition().y > obstacle.getBoundingRect().top&& tank.getPosition().y < obstacle.getBoundingRect().top + obstacle.getBoundingRect().height)
				{
					position.x = std::min(position.x, obstacle.getBoundingRect().left - borderDistance);
				}
			}

			//Right of object
			if (tank.getPosition().x > obstacle.getBoundingRect().left + obstacle.getBoundingRect().width)
			{
				if (tank.getPosition().y > obstacle.getBoundingRect().top&& tank.getPosition().y < obstacle.getBoundingRect().top + obstacle.getBoundingRect().height)
				{
					position.x = std::max(position.x, obstacle.getBoundingRect().left + obstacle.getBoundingRect().width + borderDistance);
				}
			}

			//Below object
			if (tank.getPosition().y > obstacle.getBoundingRect().top)
			{
				if (tank.getPosition().x > obstacle.getBoundingRect().left&& tank.getPosition().x < obstacle.getBoundingRect().left + obstacle.getBoundingRect().width)
				{
					position.y = std::max(position.y, obstacle.getBoundingRect().top + obstacle.getBoundingRect().height + borderDistance);
				}
			}

			//Above object
			if (tank.getPosition().y < obstacle.getBoundingRect().top + obstacle.getBoundingRect().height)
			{
				if (tank.getPosition().x > obstacle.getBoundingRect().left&& tank.getPosition().x < obstacle.getBoundingRect().left + obstacle.getBoundingRect().width)
				{
					position.y = std::min(position.y, obstacle.getBoundingRect().top - borderDistance);
				}
			}
			tank.setPosition(position);

		}
	}
}

void World::updateSounds()
{
	sf::Vector2f listenerPosition;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (mPlayerTanks.empty())
	{
		listenerPosition = mWorldView.getCenter();
	}

	// 1 or more players -> mean position between all Tanks
	else
	{
		FOREACH(Tank * Tank, mPlayerTanks)
			listenerPosition += Tank->getWorldPosition();

		listenerPosition /= static_cast<float>(mPlayerTanks.size());
	}

	// Set listener's position
	mSounds.setListenerPosition(listenerPosition);

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& jungleTexture = mTextures.get(Textures::Jungle);
	jungleTexture.setRepeated(true);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(jungleTexture, textureRect));
	jungleSprite->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(jungleSprite));

	// Add the finish line to the scene
	sf::Texture& finishTexture = mTextures.get(Textures::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	finishSprite->setPosition(0.f, -76.f);
	mFinishSprite = finishSprite.get();
	mSceneLayers[Background]->attachChild(std::move(finishSprite));

	// Add bulletSmoke particle node to the scene
	std::unique_ptr<ParticleNode> bulletSmokeNode(new ParticleNode(Particle::BulletSmoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(bulletSmokeNode));

	// Add teslaSmoke particle node to the scene
	std::unique_ptr<ParticleNode> teslaSmokeNode(new ParticleNode(Particle::TeslaSmoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(teslaSmokeNode));

	// Add tankSmoke particle node to the scene
	std::unique_ptr<ParticleNode> tankDustNode(new ParticleNode(Particle::TankDust, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(tankDustNode));

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
	mSceneGraph.attachChild(std::move(soundNode));

	// Add network node, if necessary
	if (mNetworkedWorld)
	{
		std::unique_ptr<NetworkNode> networkNode(new NetworkNode());
		mNetworkNode = networkNode.get();
		mSceneGraph.attachChild(std::move(networkNode));
	}

	addBuildings();
	addObstacles();
	addPickups();
}

void World::addObstacles() //Set up obstacles - Jason Lynch
{
	addBarrels();
}

void World::addBarrels() {
	addObstacle(Obstacle::Type::Barrel, 225, 260, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::Barrel, 285, 480, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::Barrel, 800, 260, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::Barrel, 735, 480, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::Barrel, 512, 270, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::Barrel, 512, 530, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacle::Type::Barrel, 450, 670, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacle::Type::Barrel, 580, 670, 0.f, 0.25f, 0.25f, Textures::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

//Populates world with pickups - Jason Lynch
void World::addPickups()
{
	addPickup(Pickup::Type::HeavyGun, 165, 90, 0.f, .3f, .3f);
	addPickup(Pickup::Type::HealthRefill, 165, 180, 0.f, 1.0f, 1.0f);

	addPickup(Pickup::Type::HeavyGun, 860, 90, 0.f, .3f, .3f);
	addPickup(Pickup::Type::HealthRefill, 860, 180, 0.f, 1.0f, 1.0f);

	addPickup(Pickup::Type::GatlingGun, 250, 580, 0.f, .3f, .3f);
	addPickup(Pickup::Type::FireRate, 320, 580, 0.f, 1.0f, 1.0f);

	addPickup(Pickup::Type::GatlingGun, 770, 580, 0.f, .3f, .3f);
	addPickup(Pickup::Type::FireRate, 700, 580, 0.f, 1.0f, 1.0f);

	addPickup(Pickup::Type::HeavyGun, 470, 440, 0.f, .3f, .3f);
	addPickup(Pickup::Type::HeavyGun, 550, 360, 0.f, .3f, .3f);
	addPickup(Pickup::Type::GatlingGun, 470, 360, 0.f, .3f, .3f);
	addPickup(Pickup::Type::GatlingGun, 550, 440, 0.f, .3f, .3f);

	addPickup(Pickup::Type::TeslaGun, 512, 740, 0.f, .3f, .3f);

	addPickup(Pickup::Type::HealthRefill, 290, 740, 0.f, 1.2f, 1.2f);
	addPickup(Pickup::Type::HealthRefill, 740, 740, 0.f, 1.2f, 1.2f);


	//addPickup(Pickup::Type::HealthRefill, 50, 50, 0.f, 1.2f, 1.2f);
}

//Sets up Pickups, set scale, rotation, and position - Jason Lynch
void World::addPickup(Pickup::Type type, float posX, float posY, float rotation, float scaleX, float scaleY)//Add Tank Pickups to Vector of PickupSpawnPoint structs - Jason Lynch
{
	PickupSpawnPoint spawn(type, posX, posY, rotation, scaleX, scaleY);
	mPickups.push_back(spawn);
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
				e.remove();
		});

	mCommandQueue.push(command);
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top;
	bounds.height;

	return bounds;
}
