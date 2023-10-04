#ifndef FURNACEINSTANCE_HPP
# define FURNACEINSTANCE_HPP

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

		int count( void );
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
		void updateTimes( float currentTime );

		std::string getInfoString( void );

};

#endif
