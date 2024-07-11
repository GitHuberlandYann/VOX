#ifndef ATAG_HPP
# define ATAG_HPP

# include <vector>
# include <string>

namespace tags {
	enum {
		none,
		name_tag,
		tool_tag,
		book_tag,
	};
};

class ATag
{
	public:
		virtual ~ATag( void );

		void setType( int type );
		int getType( void );

		void setName( std::string name );
		std::string getName( void );

		virtual bool compare( ATag* other );
		virtual std::shared_ptr<ATag> duplicate( void ) = 0;

	protected:
		ATag( void );

		int _type;
		std::string _name;
};

class NameTag : public ATag
{
	public:
		NameTag( void );
		NameTag( std::string name );
		~NameTag( void );

		std::shared_ptr<ATag> duplicate( void ) override;
};

class ToolTag : public ATag
{
	public:
		ToolTag( int toolDura );
		ToolTag( int dura, int toolDura );
		~ToolTag( void );

		int decrementDura( void );
		int getDura( void );
		float getDuraPercent( void );

		std::shared_ptr<ATag> duplicate( void ) override;
	
	private:
		int _dura;
		int _toolDura;
};

class BookTag : public ATag
{
	public:
		BookTag( void );
		BookTag( std::vector<std::string>& content );
		~BookTag( void );

		void pushPage( std::string page );
		std::vector<std::string>& getContent( void );

		bool compare( ATag* other ) override;
		std::shared_ptr<ATag> duplicate( void ) override;
	
	private:
		std::vector<std::string> _content;
};

#endif
