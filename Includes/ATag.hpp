#ifndef ATAG_HPP
# define ATAG_HPP

# include <vector>
# include <string>

namespace tags {
	enum {
		none,
		tool_tag,
		book_tag,
		written_book_tag,
	};
};

class ATag
{
	public:
		virtual ~ATag( void );

		int getType( void );

	protected:
		ATag( void );

		int _type;
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
	
	private:
		int _dura;
		int _toolDura;
};

class BookTag : public ATag
{
	public:
		BookTag( void );
		~BookTag( void );

		void pushPage( std::string page );
		std::vector<std::string>& getContent( void );
	
	private:
		std::vector<std::string> _content;
};

class WrittenBookTag : public ATag
{
	public:
		WrittenBookTag( std::string title, std::vector<std::string>& content );
		~WrittenBookTag( void );

		std::string getTitle( void );
		std::vector<std::string>& getContent( void );
	
	private:
		std::string _title;
		std::vector<std::string> _content;
};

#endif
