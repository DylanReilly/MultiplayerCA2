#ifndef BOOK_PICKUP_HPP
#define BOOK_PICKUP_HPP

#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Tank;

class Pickup : public Entity
{
	public:
		enum Type
		{
			HeavyGun, //Heavy gun pickup to change tank texture - Jason Lynch
			GatlingGun,//Gatling gun pickup to change tank texture - Jason Lynch
			TeslaGun,//Tesla gun pickup to change tank texture - Jason Lynch
			Nuke,//Nuke pickup to KILL EM ALL!!!!! - Jason Lynch
			HealthRefill,
			MissileRefill,
			FireSpread,
			FireRate,
			TypeCount
		};


	public:
								Pickup(Type type, const TextureHolder& textures);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;

		void 					apply(Tank& player) const;


	protected:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		Type 					mType;
		sf::Sprite				mSprite;
};

#endif // BOOK_PICKUP_HPP