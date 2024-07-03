#include "ATag.hpp"

ATag::ATag( void )
	: _type(tags::none)
{

}

ATag::~ATag( void )
{

}

ToolTag::ToolTag( int toolDura )
	: _dura(toolDura), _toolDura(toolDura)
{
	_type = tags::tool_tag;
}

ToolTag::ToolTag( int dura, int toolDura )
	: _dura(dura), _toolDura(toolDura)
{
	_type = tags::tool_tag;
}

ToolTag::~ToolTag( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

int ATag::getType( void )
{
	return (_type);
}

/**
 * @brief decrement dura by one and return resulting dura
 */
int ToolTag::decrementDura( void )
{
	--_dura;
	return (_dura);
}

int ToolTag::getDura( void )
{
	return (_dura);
}

float ToolTag::getDuraPercent( void )
{
	return (1.f * _dura / _toolDura);
}
