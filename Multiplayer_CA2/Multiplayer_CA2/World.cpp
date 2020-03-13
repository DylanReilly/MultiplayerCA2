#include "World.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Foreach.hpp"
#include "TextNode.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "Utility.hpp"
//#include "Obstacle.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>


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
, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y / 2.f)
, mScrollSpeed(-50.f)
, mScrollSpeedCompensation(1.f)
, mPlayerTanks()
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
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	FOREACH(Tank* a, mPlayerTanks)
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
	FOREACH(Tank* a, mPlayerTanks)
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
	std::unique_ptr<Tank> player(new Tank(Category::AlliedTank, Tanks::ID::GreenGatling1, mTextures, mFonts));
	player->setPosition(mWorldView.getCenter());
	player->setIdentifier(identifier);

	mPlayerTanks.push_back(player.get());
	mSceneLayers[UpperAir]->attachChild(std::move(player));
	return mPlayerTanks.back();
}

void World::createPickup(sf::Vector2f position, Pickup::Type type)
{	
	std::unique_ptr<Pickup> pickup(new Pickup(type, mTextures));
	pickup->setPosition(position);
	pickup->setVelocity(0.f, 1.f);
	mSceneLayers[UpperAir]->attachChild(std::move(pickup));
}

bool World::pollGameAction(GameActions::Action& out)
{
	return mNetworkNode->pollGameAction(out);
}

void World::setCurrentBattleFieldPosition(float lineY)
{
	mWorldView.setCenter(mWorldView.getCenter().x, lineY - mWorldView.getSize().y/2);
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
	mTextures.load(Textures::ID::Entities, "Media/Textures/Entities.png");
	mTextures.load(Textures::ID::Barrel, "Media/Textures/Barell_01.png");
	mTextures.load(Textures::ID::Wall, "Media/Textures/Arena/Blocks/Block_B_01.png");
	mTextures.load(Textures::ID::DestructableWall, "Media/Textures/Arena/Blocks/Block_B_01.png");
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

	FOREACH(Tank* Tank, mPlayerTanks)
	{
		sf::Vector2f position = Tank->getPosition();
		position.x = std::max(position.x, viewBounds.left + borderDistance);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
		position.y = std::max(position.y, viewBounds.top + borderDistance);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
		Tank->setPosition(position);
	}
}

void World::adaptPlayerVelocity()
{
	FOREACH(Tank* Tank, mPlayerTanks)
	{
		sf::Vector2f velocity = Tank->getVelocity();

		// If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
			Tank->setVelocity(velocity / std::sqrt(2.f));

		// Add scrolling velocity
		Tank->accelerate(0.f, mScrollSpeed);
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
		if (matchesCategories(pair, Category::PlayerTank, Category::EnemyTank))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& enemy = static_cast<Tank&>(*pair.second);

			// Collision: Player damage = enemy's remaining HP
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}

		else if (matchesCategories(pair, Category::PlayerTank, Category::Pickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy Projectiles
			pickup.apply(player);
			pickup.destroy();
			player.playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
		}

		else if (matchesCategories(pair, Category::EnemyTank, Category::AlliedProjectiles)
			  || matchesCategories(pair, Category::PlayerTank, Category::EnemyProjectiles))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& Projectiles = static_cast<Projectile&>(*pair.second);

			// Apply Projectiles damage to Tank, destroy Projectiles
			tank.damage(Projectiles.getDamage());
			Projectiles.destroy();
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
		FOREACH(Tank* Tank, mPlayerTanks)
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

	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Smoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	// Add propellant particle node to the scene
	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Propellant, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));

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
}

void World::addObstacles() //Set up obstacles - Jason Lynch
{
	playerOneBase();
	playerTwoBase();
	teslaobstacles();
	NukeObstacles();
	borderObstacles();
}

//Popultaes world with obstacles - Jason Lynch 
void World::NukeObstacles() {
	//DA BOMB
	addObstacle(Obstacles::ID::Nuke, 500, 290, 0.f, 0.05f, 0.05f, Textures::ID::NukeExplosion, sf::Vector2i(323, 182), 9, 2, sf::Vector2f(100.f, 100.f));

	//addObstacle(Obstacles::ID::Barrel, mObstacleSpawnPosition.x + 80, mObstacleSpawnPosition.y + 140, 0.f, 0.25f, 0.25f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	//addObstacle(Obstacles::ID::Barrel, mObstacleSpawnPosition.x + 290, mObstacleSpawnPosition.y + 140, 0.f, 0.25f, 0.25f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 160, mObstacleSpawnPosition.y + 140, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 160, mObstacleSpawnPosition.y + 20, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 160, mObstacleSpawnPosition.y - 100, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 350, mObstacleSpawnPosition.y + 140, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 350, mObstacleSpawnPosition.y + 20, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 350, mObstacleSpawnPosition.y - 100, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 185, mObstacleSpawnPosition.y - 200, 0.0f, .4f, .4f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 315, mObstacleSpawnPosition.y - 200, 0.0f, .4f, .4f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 260, mObstacleSpawnPosition.y, 0.0f, .4f, .4f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

//Popultaes world with obstacles - Jason Lynch 
void World::playerOneBase() {
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 50, mObstacleSpawnPosition.y + 100, 90.0f, .3f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 50, mObstacleSpawnPosition.y, 90.0f, .3f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 50, mObstacleSpawnPosition.y - 100, 90.0f, .3f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 20, mObstacleSpawnPosition.y + 180, 0, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 20, mObstacleSpawnPosition.y - 180, 0, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

//Popultaes world with obstacles - Jason Lynch 
void World::playerTwoBase() {
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 460, mObstacleSpawnPosition.y + 100, 90.0f, .3f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 460, mObstacleSpawnPosition.y, 90.0f, .3f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 460, mObstacleSpawnPosition.y - 100, 90.0f, .3f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 500, mObstacleSpawnPosition.y + 180, 0, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, mObstacleSpawnPosition.x + 500, mObstacleSpawnPosition.y - 180, 0, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

//Popultaes world with obstacles - Jason Lynch 
void World::teslaobstacles() {
	addObstacle(Obstacles::ID::Wall, 460, 740, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, 510, 665, 0.f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, 560, 740, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
}

//Popultaes world with obstacles - Jason Lynch 
void World::borderObstacles() {
	addObstacle(Obstacles::ID::Wall, 120, 10, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, 120, 740, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

	addObstacle(Obstacles::ID::Wall, 920, 740, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));
	addObstacle(Obstacles::ID::Wall, 920, 10, 90.0f, .4f, .2f, Textures::ID::Explosion, sf::Vector2i(256, 256), 16, 1, sf::Vector2f(1.f, 1.f));

}

//Sets up obstacles - Jason Lynch 
void World::addObstacle(Obstacles::ID type, float posX, float posY, float rotation, float scaleX, float scaleY, Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale) //Add obstacles to Vector of ObstacleSpawnPoint structs - Jason Lynch
{
	ObstacleSpawnPoint spawn(type, posX, posY, rotation, scaleX, scaleY, deathAnimation, frameSize, numberOfFrames, seconds, scale);
	mObstacles.push_back(spawn);
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
		//obstacle->setRotation(180.f);

		if (obstacle->getType() == static_cast<int>(Obstacles::ID::Nuke))
			mSceneLayers[static_cast<int>(Layer::UpperAir)]->attachChild(std::move(obstacle));
		else
			mSceneLayers[static_cast<int>(Layer::LowerAir)]->attachChild(std::move(obstacle));

		// Enemy is spawned, remove from the list to spawn
		mObstacles.pop_back();
	}
}


void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectiles | Category::EnemyTank;
	command.action = derivedAction<Entity>([this] (Entity& e, sf::Time)
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
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}
