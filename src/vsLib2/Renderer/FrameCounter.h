#pragma once


class vsFrameCounter {
public:
					vsFrameCounter( void );
					~vsFrameCounter( void );

	void			Update( void );
	int				GetCurrent( void ) const;
	int				GetMin( void ) const;
	int				GetMax( void ) const;
	int				GetAvarage( void ) const;
	float			GetInverseCurrent( void ) const;

private:
	int				current;
	float			inverseCurrent;
	int				min;
	int				max;
	int				avarage;
};
