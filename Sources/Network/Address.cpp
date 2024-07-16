#include "Address.hpp"

Address::Address( void )
	: _address(settings::consts::network::localhost), _port(settings::consts::network::default_port)
{

}

Address::Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port )
	: _address((a << 24) | (b << 16) | (c << 8) | d), _port(port)
{

}

Address::Address( unsigned int address, unsigned short port )
	: _address(address), _port(port)
{

}

bool Address::operator==( const Address & other )
{
	return (_address == other.getAddress() && _port == other.getPort());
}

bool Address::operator!=( const Address & other )
{
	return !(*this == other);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

unsigned int Address::getAddress( void ) const
{
	return (_address);
}

unsigned short Address::getPort( void ) const
{
	return (_port);
}

// ************************************************************************** //
//                                Extern                                      //
// ************************************************************************** //

std::ostream &operator<<( std::ostream &out, const Address &a )
{
	unsigned addr = a.getAddress();
	out << ((addr >> 24) & 0xFF) << '.' << ((addr >> 16) & 0xFF) << '.'
		<< ((addr >> 8) & 0xFF) << '.' << (addr & 0xFF) << ':' << a.getPort();
	return (out);
}
