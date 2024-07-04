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

BookTag::BookTag( void )
{
	_type = tags::book_tag;
	// _content.push_back("\n\nHello \"World\"!");
}

BookTag::~BookTag( void )
{

}

WrittenBookTag::WrittenBookTag( void )
{
	_type = tags::written_book_tag;
}

WrittenBookTag::WrittenBookTag( std::string title, std::vector<std::string>& content )
	: _title(title), _content(content)
{
	_type = tags::written_book_tag;
	// _content.push_back("\n\nHello \"World\"!");
}

WrittenBookTag::~WrittenBookTag( void )
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

void BookTag::pushPage( std::string page )
{
	_content.push_back(page);
}

void WrittenBookTag::pushPage( std::string page )
{
	_content.push_back(page);
}

std::vector<std::string>& BookTag::getContent( void )
{
	return (_content);
}

void WrittenBookTag::setTitle( std::string title )
{
	_title = title;
}

std::string WrittenBookTag::getTitle( void )
{
	return (_title);
}

std::vector<std::string>& WrittenBookTag::getContent( void )
{
	return (_content);
}