#ifndef BOOK_PROJECTILE_HPP
#define BOOK_PROJECTILE_HPP

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "Animation.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Projectile : public Entity
{
	public:
		enum Type
		{
			GreenLmgBullet,
			GreenHmgBullet,
			GreenGatlingBullet,
			GreenTeslaBullet,
			RedLmgBullet,
			RedHmgBullet,
			RedGatlingBullet,
			RedTeslaBullet,
			HostLmgBullet,
			HostHmgBullet,
			HostGatlingBullet,
			HostTeslaBullet,
			AlliedBullet = GreenLmgBullet | GreenHmgBullet | GreenGatlingBullet | GreenTeslaBullet,
			EnemyBullet = RedLmgBullet | RedHmgBullet | RedGatlingBullet | RedTeslaBullet,
			HostBullet = HostLmgBullet | HostHmgBullet | HostGatlingBullet | HostTeslaBullet,
			Missile,
			TypeCount
		};


	public:
								Projectile(Type type, const TextureHolder& textures);

		void					guideTowards(sf::Vector2f position);
		bool					isGuided() const;

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		float					getMaxSpeed() const;
		int						getDamage() const;

	
	private:
		virtual void			updateCurrent(sf::Time dt, CommandQueue& commands);
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		Type					mType;
		sf::Sprite				mSprite;
		sf::Vector2f			mTargetDirection;
		Animation				mFiringAnimation;
};

#endif // BOOK_PROJECTILE_HPP
