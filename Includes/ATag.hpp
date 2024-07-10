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
		written_book_tag,
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

	protected:
		ATag( void );

		int _type;
		std::string _name;
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

		bool compare( ATag* other ) override;
	
	private:
		std::vector<std::string> _content;
};

class WrittenBookTag : public ATag
{
	public:
		WrittenBookTag( void );
		WrittenBookTag( std::string title, std::vector<std::string>& content );
		~WrittenBookTag( void );

		void setTitle( std::string title );
		std::string getTitle( void );
		void pushPage( std::string page );
		std::vector<std::string>& getContent( void );
	
	private:
		std::string _title;
		std::vector<std::string> _content;

		bool compare( ATag* other ) override;
};

#endif
