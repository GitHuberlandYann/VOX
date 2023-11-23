namespace Random {
	/**
	 * @brief generates random number from input
	 * @param input  unsigned number representing seed
	 * @return unsigned random number
	 * 
	 * https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
	 */
	static unsigned PCG_Hash( unsigned input )
	{
		unsigned state = input * 747796405u + 2891336453u;
		unsigned word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return ((word >> 22u) ^ word);
	}

	/**
	 * @brief generates random float in range 0-1
	*/
	float randomFloat( unsigned &seed )
	{
		seed = PCG_Hash(seed);
		return (static_cast<float>(seed) / static_cast<float>(0xFFFFFFFF));
	}

	/**
	 * @brief generates random integer in range start-end, generated number can't be 'end'
	 */
	int rangedNumber( unsigned &seed, int start, int end )
	{
		int range = (end - 1) - start;
		return (start + randomFloat(seed) * range);
	}
}
/*
#include <iostream>

int main( void )
{
	unsigned seed = (16 * 19516 + 64 * 56849) * 1698598149u;
	for (int i = 0; i < 20; i++) {
		std::cout << seed << " gives " << Random::rangedNumber(seed, 0, 1000) << std::endl;
	}
	return (0);
}*/
