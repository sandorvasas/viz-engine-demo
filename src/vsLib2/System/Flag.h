
#ifndef __CR2_FLAG_H__
#define __CR2_FLAG_H__


/*
======================================================
 Ez egy egyszer� kis flag nev� oszt�ly, amivel 
 4 byte hossz�s�g� bit sorozaton �ll�thatunk k�l�nb�z�
 biteket 1-re ( igazra ) vagy 0-ra ( hamisra ).
 Nagyon hasznos, ha valamilyen m�veletet szeretn�nk
 v�gezni �gy, hogy k�l�nb�z� opci�kat kapcsolhatunk be
 �s ki, amik hat�ssal vannak a m�veletre. 
 Ez teljesen �rthetetlen, nem tudom elmagyar�zni, 
 ez�rt itt egy p�lda:

 Ki akarok rajzolni egy g�mb�t a k�vetkez� szempontok szerint:
  - le van text�r�zva
  - nincs megvil�g�tva
  
  Defini�lok k�t konstanst:
  MODEL_TEXTURED   = 0x0001
  MODEL_NOLIGHTING = 0x0010

  vsFlag flag;
  flag.Set( MODEL_TEXTURED );
  flag.Set( MODEL_NOLIGHTING );
  model->Render( flag );

  ilyenkor a Rener() fv-nek persze ellen�riznie kell, hogy melyik
  bitek vannak 1-re �ll�tva:
  Render():
	[..]
		if ( flag.IsSet( MODEL_TEXTURED ) ) { textur�zza }
		if ( flag.IsSet( MODEL_NOLIGHTING ) ) { vil�g�t�s kikapcs }
	[..]


======================================================
*/

class vsFlag {
public:
	/*
	=============================
		Alap konstruktor
			Minden bitet 0-ra �ll�t.
	=============================
	*/
					vsFlag( void ): flag( 0 ) {};
	/*
	=============================
		Destruktor
			�res
	=============================
	*/
					~vsFlag( void ) {};

	/*
	=============================
		Set()
		OR m�veletet hajt v�gre
		a param�ter �s a megl�v� bit sorozaton
	=============================
	*/
	void			Set( unsigned int flag );
	/*
	=============================
		Unset()
		XOR m�veletet hajt v�gre
		a param�ter �s a megl�v� bit sorozaton
	=============================
	*/
	void			Unset( unsigned int flag );
	/*
	=============================
		IsSet()
		AND m�veletet hajt v�gre
		a param�ter �s a megl�v� bit sorozaton
		Visszat�r�si �rt�ke IGAZ, ha a bit sorozat
		tartalmazza a param�tert, HAMIS, ha nem.
	=============================
	*/
	bool			IsSet( unsigned int flag );
	/*
	=============================
		Set()
		minden bit null�ra
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