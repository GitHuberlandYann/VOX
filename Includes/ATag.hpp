#ifndef ATAG_HPP
# define ATAG_HPP

namespace tags {
	enum {
		none,
		tool_tag,
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

#endif
