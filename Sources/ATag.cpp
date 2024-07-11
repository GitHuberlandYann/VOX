#include <memory>
#include "ATag.hpp"

ATag::ATag( void )
	: _type(tags::none)
{

}

ATag::~ATag( void )
{

}

NameTag::NameTag( void )
{
	_type = tags::name_tag;
}

NameTag::NameTag( std::string name )
{
	_type = tags::name_tag;
	_name = name;
}

NameTag::~NameTag( void )
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

BookTag::BookTag( std::vector<std::string>& content )
	: _content(content)
{
	_type = tags::book_tag;
}

BookTag::~BookTag( void )
{

}

// ************************************************************************** //
//                                ATag                                        //
// ************************************************************************** //

void ATag::setType( int type )
{
	_type = type;
}

int ATag::getType( void )
{
	return (_type);
}

void ATag::setName( std::string name )
{
	_name = name;
}

std::string ATag::getName( void )
{
	return (_name);
}

/**
 * @brief compare with given tag and return false if identical
 */
bool ATag::compare( ATag* other )
{
	if (!other) {
		return (true);
	}
	if (_type != other->getType() || _type == tags::tool_tag) {
		return (true);
	}
	if (_name != other->getName()) {
		return (true);
	}
	return (false);
}

// ************************************************************************** //
//                                ToolTag                                     //
// ************************************************************************** //

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

// ************************************************************************** //
//                                BookTag                                     //
// ************************************************************************** //

void BookTag::pushPage( std::string page )
{
	_content.push_back(page);
}

std::vector<std::string>& BookTag::getContent( void )
{
	return (_content);
}

bool BookTag::compare( ATag* other )
{
	if (!other) {
		return (true);
	}
	if (_type != other->getType() || _type == tags::tool_tag) {
		return (true);
	}
	if (_name != other->getName()) {
		return (true);
	}
	std::vector<std::string>& oContent = static_cast<BookTag*>(other)->getContent();
	if (_content.size() != oContent.size()) {
		return (true);
	}
	for (size_t index = 0; index < _content.size(); ++index) {
		if (_content[index] != oContent[index]) {
			return (true);
		}
	}
	return (false);
}

// ************************************************************************** //
//                                Duplicate                                   //
// ************************************************************************** //

std::shared_ptr<ATag> NameTag::duplicate( void )
{
	std::shared_ptr<ATag> res = std::make_shared<NameTag>();

	res->setType(_type);
	res->setName(_name);
	return (res);
}

std::shared_ptr<ATag> ToolTag::duplicate( void )
{
	std::shared_ptr<ATag> res = std::make_shared<ToolTag>(_dura, _toolDura);

	res->setType(_type);
	res->setName(_name);
	return (res);
}

std::shared_ptr<ATag> BookTag::duplicate( void )
{
	std::shared_ptr<ATag> res = std::make_shared<BookTag>(_content);

	res->setType(_type);
	res->setName(_name);
	return (res);
}
