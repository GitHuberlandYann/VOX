#ifndef ADDRESS_HPP
# define ADDRESS_HPP

# include <iostream>

namespace settings {
	namespace consts {
		namespace network {
			// 127.0.0.1
			const unsigned localhost = (127 << 24) + 1;
			const unsigned short default_port = 31415;
		};
	};
};

class Address
{
	public:
		Address( void );
		Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d,
				unsigned short port = settings::consts::network::default_port );
		Address( unsigned int address,
				unsigned short port = settings::consts::network::default_port );
		
		bool operator==( const Address & other );
		bool operator!=( const Address & other );

		unsigned int getAddress( void ) const;
		unsigned short getPort( void ) const;

	private:
		unsigned int _address;
		unsigned short _port;
};

std::ostream &operator<<( std::ostream &out, const Address &a );

#endif
