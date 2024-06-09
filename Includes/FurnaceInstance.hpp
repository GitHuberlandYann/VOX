#ifndef FURNACEINSTANCE_HPP
# define FURNACEINSTANCE_HPP

# include <glm/glm.hpp>
# include "Inventory.hpp"

namespace furnace_state {
	enum {
		NOCHANGE, // same state as previous frame
		ON,	// turned on this frame
		OFF // turned off this frame
	};
}

class FurnaceInstance
{
	public:
		FurnaceInstance( void );
		~FurnaceInstance( void );

		float getComposantTime( void );
		float getFuelTime( void );
		t_item getComposant( void );
		t_item getFuel( void );
		t_item getProduction( void );
		void setComposant( t_item value );
		void setFuel( t_item value );
		void setProduction( t_item value );
		void removeComposant( void );
		void removeFuel( void );
		t_item pickProduction( void );
		t_item *pickCompoFuel( bool fuel );
		int updateTimes( float currentTime );

		std::string getInfoString( void );

	private:
		t_item _composant;
		t_item _fuel;
		t_item _production;
		float _fuel_time, _composant_time, _current_fuel_time, _current_time;
};

#endif
