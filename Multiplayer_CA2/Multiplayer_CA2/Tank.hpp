#ifndef BOOK_Tank_HPP
#define BOOK_Tank_HPP

#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Projectile.hpp"
#include "TextNode.hpp"
#include "Animation.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Tank : public Entity
{
	public:
								Tank(Category::Type entity, Tanks::ID type, const TextureHolder& textures, const FontHolder& fonts);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		virtual void			remove();
		virtual bool 			isMarkedForRemoval() const;
		Projectiles::ID			getProjectile() const;
		bool					isAllied() const;
		float					getMaxSpeed() const;
		void					disablePickups();

		void					increaseFireRate();
		void					increaseSpread();
		void					setTankTexture(unsigned int val);
		void					collectMissiles(unsigned int count);

		void 					fire();
		void					launchMissile();
		void					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);
		int						getIdentifier();
		void					setIdentifier(int identifier);
		int						getMissileAmmo() const;
		void					setMissileAmmo(int ammo);


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
		//void					updateMovementPattern(sf::Time dt);
		void					checkPickupDrop(CommandQueue& commands);
		void					checkProjectilesLaunch(sf::Time dt, CommandQueue& commands);

		void					createBullets(SceneNode& node, const TextureHolder& textures) const;
		void					createProjectile(SceneNode& node, Projectiles::ID type, float xOffset, float yOffset, const TextureHolder& textures) const;
		void					createPickup(SceneNode& node, const TextureHolder& textures) const;

		void					updateTexts();


	private:
		Tanks::ID				mType;
		sf::Sprite				mSprite;
		Animation				mExplosion;
		Command 				mFireCommand;
		Command					mMissileCommand;
		sf::Time				mFireCountdown;
		bool 					mIsFiring;
		bool					mIsLaunchingMissile;
		bool 					mShowExplosion;
		bool					mExplosionBegan;
		bool					mSpawnedPickup;
		bool					mPickupsEnabled;

		int						mFireRateLevel;
		int						mSpreadLevel;
		int						mMissileAmmo;

		const TextureHolder&	mTextures; //Hold texture for tank changes - Jason Lynch
		Command 				mDropPickupCommand;
		float					mTravelledDistance;
		std::size_t				mDirectionIndex;
		TextNode*				mHealthDisplay;
		TextNode*				mMissileDisplay;

	
		int						mIdentifier;};

#endif // BOOK_Tank_HPP
