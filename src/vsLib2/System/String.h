#ifndef __VS_STR_H__
#define __VS_STR_H__

#include <Windows.h>

class vsStr {
public:
						vsStr( void );
						vsStr( const char* str );
	explicit			vsStr( char* format, ... );
			//			vsStr( char* str );
	//explicit			vsStr( const vsStr& str );
						~vsStr( void );

	vsStr&				operator=( const vsStr& b );
	bool				operator==( const vsStr& b );
	bool				operator!=( const vsStr& b );
	vsStr&				operator=( const char* b );
	bool				operator==( const char* b );
	bool				operator!=( const char* b );
	vsStr&				operator=( char* b );
	bool				operator==( char* b );
	bool				operator!=( char* b );
	bool				operator>( const vsStr& b );
	bool				operator<( const vsStr& b );
	bool				operator>=( const vsStr& b );
	bool				operator<=( const vsStr& b );
	char&				operator[]( int num );
	char*				operator()( void );
	const char*			operator()( void ) const;
	vsStr				operator+( const vsStr& b ) const;
	const vsStr&		operator+=( const vsStr& b );

	void				Zero( void );
	void				CorrectLength( void );
	void				Reserve( int length );
	int					Length( void ) const;
	void				TruncateTo( int maxLen );
	int					Find( char ch ) const;
	int					Find( char* str ) const;				// NOT WORKING
	int					Find( const vsStr& str ) const;			// NOT WORKING
	int					FindFromBack( char ch ) const;			
	int					FindFromBack( char* str ) const;		// NOT WORKING
	int					FindFromBack( const vsStr& str ) const; // NOT WORKING
	char				CharAt( int index ) const;
	void				ReplaceFirst( char ch, char with );
	void				ReplaceAll( char ch, char with );
	void				ReplaceFirst( const vsStr& str, const vsStr& with );	// NOT WORKING
	void				ReplaceAll( const vsStr& str, const vsStr& with );		// NOT WORKING
	void				DeleteAll( char ch );
	vsStr				SubStr( int start, int length );
	void				Insert( int to, char ch );
	void				Insert( int to, char* str );
	void				Insert( int to, const vsStr& str );
	void				Reverse( void );
	void				ToLowerCase( void );
	void				ToUpperCase( void );
	void				Append( char ch );
	void				Append( char* str );
	void				Append( const vsStr& str );
	void				Expand( void );
	void				Narrow( void );
//	int					Contains( const vsStr& str );
//	int					Contains( char* str );
	char*				ToCharPtr( void );
	const char*			ToCharPtr( void ) const;

private:
	int					len;
	char*				string;
};


#endif