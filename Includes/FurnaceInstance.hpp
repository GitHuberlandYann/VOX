#ifndef FURNACEINSTANCE_HPP
# define FURNACEINSTANCE_HPP

# include <glm/glm.hpp>

namespace furnace_state {
	enum {
		NOCHANGE, // same state as previous frame
		ON,	// turned on this frame
		OFF // turned off this frame
	};
}

class FurnaceInstance
{
	private:
		glm::ivec2 _composant;
		glm::ivec2 _fuel;
		glm::ivec2 _production;
		float _fuel_time, _composant_time, _current_fuel_time, _current_time;

	public:
		FurnaceInstance( void );
		~FurnaceInstance( void );

		float getComposantTime( void );
		float getFuelTime( void );
		glm::ivec2 getComposant( void );
		glm::ivec2 getFuel( void );
		glm::ivec2 getProduction( void );
		void setComposant( glm::ivec2 value );
		void setFuel( glm::ivec2 value );
		void setProduction( glm::ivec2 value );
		void removeComposant( void );
		void removeFuel( void );
		glm::ivec2 pickProduction( void );
		glm::ivec2 *pickCompoFuel( bool fuel );
		int updateTimes( float currentTime );

		std::string getInfoString( void );

};

#endif
