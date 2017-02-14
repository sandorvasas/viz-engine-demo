
#ifndef __CR2_FLAG_H__
#define __CR2_FLAG_H__


/*
======================================================
 Ez egy egyszerû kis flag nevû osztály, amivel 
 4 byte hosszúságú bit sorozaton állíthatunk különbözõ
 biteket 1-re ( igazra ) vagy 0-ra ( hamisra ).
 Nagyon hasznos, ha valamilyen mûveletet szeretnénk
 végezni úgy, hogy különbözõ opciókat kapcsolhatunk be
 és ki, amik hatással vannak a mûveletre. 
 Ez teljesen érthetetlen, nem tudom elmagyarázni, 
 ezért itt egy példa:

 Ki akarok rajzolni egy gömböt a következõ szempontok szerint:
  - le van textúrázva
  - nincs megvilágítva
  
  Definiálok két konstanst:
  MODEL_TEXTURED   = 0x0001
  MODEL_NOLIGHTING = 0x0010

  vsFlag flag;
  flag.Set( MODEL_TEXTURED );
  flag.Set( MODEL_NOLIGHTING );
  model->Render( flag );

  ilyenkor a Rener() fv-nek persze ellenõriznie kell, hogy melyik
  bitek vannak 1-re állítva:
  Render():
	[..]
		if ( flag.IsSet( MODEL_TEXTURED ) ) { texturázza }
		if ( flag.IsSet( MODEL_NOLIGHTING ) ) { világítás kikapcs }
	[..]


======================================================
*/

class vsFlag {
public:
	/*
	=============================
		Alap konstruktor
			Minden bitet 0-ra állít.
	=============================
	*/
					vsFlag( void ): flag( 0 ) {};
	/*
	=============================
		Destruktor
			Üres
	=============================
	*/
					~vsFlag( void ) {};

	/*
	=============================
		Set()
		OR mûveletet hajt végre
		a paraméter és a meglévõ bit sorozaton
	=============================
	*/
	void			Set( unsigned int flag );
	/*
	=============================
		Unset()
		XOR mûveletet hajt végre
		a paraméter és a meglévõ bit sorozaton
	=============================
	*/
	void			Unset( unsigned int flag );
	/*
	=============================
		IsSet()
		AND mûveletet hajt végre
		a paraméter és a meglévõ bit sorozaton
		Visszatérési értéke IGAZ, ha a bit sorozat
		tartalmazza a paramétert, HAMIS, ha nem.
	=============================
	*/
	bool			IsSet( unsigned int flag );
	/*
	=============================
		Set()
		minden bit nullára
	=============================
	*/
	void			Zero( void );


public:
	unsigned int	flag;
};

VS_INLINE
void vsFlag::Set( unsigned int flag ) {
	this->flag |= flag;
}

VS_INLINE
void vsFlag::Unset( unsigned int flag ) {
	this->flag ^= flag;
}

VS_INLINE
bool vsFlag::IsSet( unsigned int flag ) {
	return( ( this->flag & flag ) == flag );
}

VS_INLINE
void vsFlag::Zero( void ) {
	this->flag = 0;
}

#endif