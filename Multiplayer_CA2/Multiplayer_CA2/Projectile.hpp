#ifndef BOOK_Projectiles_HPP
#define BOOK_Projectiles_HPP

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Projectile : public Entity
{
	public:
								Projectile(Projectiles::ID type, const TextureHolder& textures);

		void					guideTowards(sf::Vector2f position);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		float					getMaxSpeed() const;
		int						getDamage() const;

	
	private:
		virtual void			updateCurrent(sf::Time dt, CommandQueue& commands);
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		Projectiles::ID			mType;
		sf::Sprite				mSprite;
		sf::Vector2f			mTargetDirection;
};

#endif // BOOK_Projectiles_HPP
