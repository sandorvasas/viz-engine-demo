#include "matrix.h"
#include "quaternion.h"
/*=========
    vsMat2
  =========*/
vsMat2::vsMat2( void ) {
	LoadIdentity();
}

vsMat2::vsMat2( float m11,	float m12,
			 float m21, float m22 ) {
	_mat[0] = m11;
	_mat[1] = m21;
	_mat[2] = m12;
	_mat[3] = m22;
}

vsMat2::vsMat2( float mat[4] ) {
	for ( int i=0; i<4; ++i ) 
		_mat[i] = mat[i];
}

//==============
// vsMat2::operator[]
//==============
float&	vsMat2::operator[]( int i ) {
	static float err = 0;
	return ( i<4 && i>-1 )? _mat[i] : err;
}

//==============
// vsMat2::operator+
//==============
vsMat2	vsMat2::operator+( const vsMat2 &mat ) const {
	vsMat2 m = vsMat2( mat._mat[0] + _mat[0],    mat._mat[2] + _mat[2],
				  mat._mat[1] + _mat[1],    mat._mat[3] + _mat[3] );

	return m;

}

vsMat2 vsMat2::operator+( float m ) const {
	vsMat2 b = *this;
	for ( int i=0; i<4; ++i )
		b._mat[i] += m;
	return b;
}

//==============
// vsMat2::operator-
//==============
vsMat2	vsMat2::operator-( const vsMat2& mat ) const {
	vsMat2 m = vsMat2( mat._mat[0] - _mat[0],    mat._mat[2] - _mat[2],
				  mat._mat[1] - _mat[1],    mat._mat[3] - _mat[3] );

	return m;
}

//==============
// vsMat2::operator-
//==============
vsMat2 vsMat2::operator-( void ) const {
	vsMat2 m = vsMat2( - _mat[0],     - _mat[2],
					   - _mat[1],     - _mat[3] );
	return m;
}

//==============
// vsMat2::operator* ( vsMat2 )
//==============
vsMat2	vsMat2::operator*( const vsMat2& mat ) const {
	vsMat2 m;
	m._mat[0] = _mat[0] * mat._mat[0] + _mat[2] * mat._mat[1];
	m._mat[1] = _mat[0] * mat._mat[2] + _mat[2] * mat._mat[3];
	m._mat[2] = _mat[1] * mat._mat[0] + _mat[3] * mat._mat[1];
	m._mat[3] = _mat[1] * mat._mat[2] + _mat[3] * mat._mat[3];

	return m;
}

//==============
// vsMat2::operator* ( float )
//==============
vsMat2 vsMat2::operator*( float scalar ) const {
	vsMat2 m = vsMat2( _mat[0] * scalar,    _mat[2] * scalar,
					  _mat[1] * scalar,    _mat[3] * scalar );

	return m;
}

vsVec2f vsMat2::operator*( const vsVec2f& v ) const {
	return vsVec2f( 	_mat[0] * v.x + _mat[2] * v.x,
					_mat[1] * v.y + _mat[3] * v.y );
}

vsMat2 vsMat2::operator/( float s ) const {
	vsMat2 m = vsMat2( _mat[0] / s,    _mat[2] / s,
					  _mat[1] / s,    _mat[3] / s );

	return m;
}

vsMat2 vsMat2::operator/( const vsMat2& m ) const {
	return this->Transpose()*m;
}

/* FIXME URGENT! */
vsVec2f vsMat2::operator/( const vsVec2f& v ) const {
	return vsVec2f( 	_mat[0] * v.x + _mat[1] * v.x,
					_mat[2] * v.y + _mat[3] * v.y ) * 0.0f/*!!!!!!!!!!*/;
}

vsMat2& vsMat2::operator/=( const float& s ) {
	*this = vsMat2( _mat[0] / s,    _mat[2] / s,
				   _mat[1] / s,    _mat[3] / s );

	return *this;
}

vsMat2& vsMat2::operator/=( const vsMat2& m ) {
	*this = this->Transpose()*m;
	return *this;
}


vsMat2& vsMat2::operator=( const vsMat2& m ) {
	for ( int i=0; i<4; ++i ) 
		this->_mat[i] = m._mat[i];
	return * this;
}

vsMat2& vsMat2::operator*=( const vsMat2& m ) {
	*this = *this * m;
	return * this;
}

vsMat2& vsMat2::operator*=( const float& s ) {
	*this = *this * s;
	return * this;
}

vsMat2& vsMat2::operator+=( const float& s ) {
	*this = *this + s;
	return * this;
}

vsMat2& vsMat2::operator+=( const vsMat2& m ) {
	*this = *this + m;
	return * this;
}

vsMat2& vsMat2::operator-=( const float& s ) {
	*this = *this - s;
	return * this;
}

vsMat2& vsMat2::operator-=( const vsMat2& m ) {
	*this = *this - m;
	return * this;
}

vsMat2 vsMat2::operator-( float s ) const {
	vsMat2 m;
	for ( int i=0; i<4; ++i )
		m._mat[i] = this->_mat[i] - s;
	return m;
}

bool vsMat2::operator==( const vsMat2& m ) const {
	for ( int i=0; i<4; ++i )
		if ( this->_mat[i] != m._mat[i] ) {
			return false;
		}

	return true;
}

bool vsMat2::operator!=( const vsMat2& m ) const {
	return !( *this == m );
}


//==============
// vsMat2::Transpose
//==============
vsMat2	vsMat2::Transpose( void ) const {
	vsMat2 transpose = vsMat2( _mat[0], _mat[1],
						  _mat[2], _mat[3] );
	return transpose;
}

//==============
// vsMat2::Adjung
//==============
vsMat2	vsMat2::Adjung( void ) const {
	vsMat2 adjung;
	adjung._mat[0] =   _mat[3];
	adjung._mat[1] = - _mat[2];
	adjung._mat[2] = - _mat[1];
	adjung._mat[3] =   _mat[0];

	return adjung;
}

//==============
// vsMat2::Inverse
//==============
vsMat2	vsMat2::Inverse( void ) const {
	vsMat2 mat = Adjung() * ( 1/Determinant() );
	return mat;
}

//==============
// vsMat2::Determinant
//==============
float	vsMat2::Determinant( void ) const {
	float det = ( _mat[0]*_mat[3] + _mat[2]*_mat[1] ) -
				( _mat[2]*_mat[1] + _mat[1]*_mat[3] );
	return det;
}

// vsMat2::LoadIdentity
//==============
void vsMat2::LoadIdentity( void ) {
	_mat[0] = 1.0f;		_mat[2] = 0;
	_mat[1] = 0;		_mat[3] = 1.0f;
}

//==============
// vsMat2::TovsMat3
//==============
vsMat3	vsMat2::ToMat3( void ) {
	return vsMat3( _mat[0], _mat[2], 0,
		        _mat[1], _mat[3], 0,
		  			  0,       0, 1 );
}

//==============
// vsMat2::TovsMat4
//==============
vsMat4	vsMat2::ToMat4( void ) {
	return vsMat4( _mat[0], _mat[2], 0, 0,
		          _mat[1], _mat[3], 0, 0,
		  			  0,         0, 1, 0,
		              0,         0, 0, 1 );
}

//==============
// vsMat2::IsIdentity
//==============
bool vsMat2::IsIdentity( void ) {
	vsMat2 Diagonal;
	Diagonal.LoadIdentity();
	return ( *_mat == *Diagonal._mat )? true : false;
}

//==============
// vsMat2::IsProjective
//==============
bool vsMat2::IsProjective( void ) {
	vsMat2 projective;
	projective = *this * *this;
	return ( *_mat == *projective._mat )? true : false;
}

//==============
// vsMat2::Rotate
//==============
vsMat2& vsMat2::Rotate( float angle ) {
	float cosT = cosf( angle ),
		  sinT = sinf( angle );
	*this = ( *this ) * vsMat2 ( cosT, -sinT,
							sinT,  cosT );
	return *this;			  
}

float* vsMat2::ToTypePtr( void ) {
	return ( &_mat[0] );
}



/*=========
    vsMat3
  =========*/

vsMat3::vsMat3( void ) {
	LoadIdentity();
}

vsMat3::vsMat3( float m11, float m21, float m31, 
		   float m12, float m22, float m32, 
		   float m13, float m23, float m33 ) {
	_mat[0] = m11;			_mat[3] = m12;				_mat[6] = m13;
	_mat[1] = m21;			_mat[4] = m22;				_mat[7] = m23;
	_mat[2] = m31;			_mat[5] = m32;				_mat[8] = m33;
}

vsMat3::vsMat3( float degX, float degY, float degZ ) {
	vsQuaternion rx;
	rx.CreateFromEulerAnglesDEG( degX, degY, degZ );

	*this = rx.ToMat3();
}

vsMat3::vsMat3( float mat[] ) {
	for ( int i=0; i<9; ++i ) 
		_mat[i] = mat[i];
}

//==============
// vsMat3::Operator []
//==============
float&   vsMat3::operator[]( int i ) {
	 static float e = 0.0f;
	return ( ( i<0 ) || ( i>8 ) )? e  : _mat[i];
}

//==============
// vsMat3::Operator +
//==============
vsMat3	vsMat3::operator+( const vsMat3 &m ) const {
	vsMat3 mat;
	for ( int i=0; i<9; ++i )
		mat._mat[i] = _mat[i] + m._mat[i];
	return mat;
}

vsMat3 vsMat3::operator+( float m ) const {
	vsMat3 b = *this;
	for ( int i=0; i<9; ++i )
		b._mat[i] += m;
	return b;
}

//==============
// vsMat3::Operator -
//==============
vsMat3 vsMat3::operator-( const vsMat3& m ) const {
	vsMat3 mat;
	for ( int i=0; i<9; ++i )
		mat._mat[i] =  _mat[i] - m._mat[i];

	return mat;
}


//==============
// vsMat3::Operator -
//==============
vsMat3 vsMat3::operator-( void ) const {
	vsMat3 mat;
	for ( int i=0; i<9; ++i )
		mat._mat[i] =  - _mat[i];

	return mat;
}

//==============
// vsMat3::Operator *( vsMat3 )
//==============
vsMat3 vsMat3::operator* ( const vsMat3& m ) const {
	vsMat3 mat;

	mat[0] = m._mat[0] * _mat[0] + m._mat[3] * _mat[1] + m._mat[6] * _mat[2]; 
	mat[1] = m._mat[1] * _mat[0] + m._mat[4] * _mat[1] + m._mat[7] * _mat[2]; 
	mat[2] = m._mat[2] * _mat[0] + m._mat[5] * _mat[1] + m._mat[8] * _mat[2]; 

	mat[3] = m._mat[0] * _mat[3] + m._mat[3] * _mat[4] + m._mat[6] * _mat[5]; 
	mat[4] = m._mat[1] * _mat[3] + m._mat[4] * _mat[4] + m._mat[7] * _mat[5]; 
	mat[5] = m._mat[2] * _mat[3] + m._mat[5] * _mat[4] + m._mat[8] * _mat[5]; 

	mat[6] = m._mat[0] * _mat[6] + m._mat[3] * _mat[7] + m._mat[6] * _mat[8]; 
	mat[7] = m._mat[1] * _mat[6] + m._mat[4] * _mat[7] + m._mat[7] * _mat[8]; 
	mat[8] = m._mat[2] * _mat[6] + m._mat[5] * _mat[7] + m._mat[8] * _mat[8]; 

	return mat;
}

//==============
// vsMat3::Operator *( scalar )
//==============
vsMat3 vsMat3::operator*( float s ) const {
	vsMat3 mat;
	for ( int i=0; i<9; ++i )
		mat._mat[i] = _mat[i] * s;

	return mat;	
}

//==============
// vsMat3::Operator *( vsVec3f )
//==============
vsVec3f vsMat3::operator *( const vsVec3f& v ) const {
	return vsVec3f( _mat[0] * v.x + _mat[3] * v.y + _mat[6]  * v.z,
				  _mat[1] * v.x + _mat[4] * v.y + _mat[7]  * v.z ,
				  _mat[2] * v.x + _mat[5] * v.y + _mat[8]  * v.z    );
}

vsMat3& vsMat3::operator=( const vsMat3& m ) {
	for ( int i=0; i<9; ++i ) 
		this->_mat[i] = m._mat[i];
	return * this;
}

vsMat3& vsMat3::operator*=( const vsMat3& m ) {
	*this = *this * m;
	return * this;
}

vsMat3& vsMat3::operator*=( const float& s ) {
	*this = *this * s;
	return * this;
}

vsMat3 vsMat3::operator/( float s ) const {
	vsMat3 m = vsMat3( _mat[0] / s,    _mat[3] / s,    _mat[6] / s,
					  _mat[1] / s,    _mat[4] / s,    _mat[7] / s,
					  _mat[2] / s,	  _mat[5] / s,    _mat[8] / s );

	return m;
}

vsMat3 vsMat3::operator/( const vsMat3& m ) const {
	return this->Transpose()*m;
}

vsVec3f vsMat3::operator/( const vsVec3f& v ) const {
	return vsVec3f( 	_mat[0] * v.x + _mat[1] * v.x + _mat[2] * v.x,
					_mat[3] * v.y + _mat[4] * v.y + _mat[5] * v.y,
					_mat[6] * v.z + _mat[7] * v.z + _mat[8] * v.z );
}

vsMat3& vsMat3::operator/=( const float& s ) {
	*this = *this / s;
	return *this;
}

vsMat3& vsMat3::operator/=( const vsMat3& m ) {
	*this=this->Transpose()*m;
	return * this;
}

vsMat3& vsMat3::operator+=( const float& s ) {
	*this = *this + s;
	return * this;
}

vsMat3& vsMat3::operator+=( const vsMat3& m ) {
	*this = *this + m;
	return * this;
}

vsMat3& vsMat3::operator-=( const float& s ) {
	*this = *this - s;
	return * this;
}

vsMat3& vsMat3::operator-=( const vsMat3& m ) {
	*this = *this - m;
	return * this;
}

vsMat3 vsMat3::operator-( float s ) const {
	vsMat3 m;
	for ( int i=0; i<9; ++i )
		m._mat[i] = this->_mat[i] - s;
	return m;
}

bool vsMat3::operator==( const vsMat3& m ) const {
	for ( int i=0; i<9; ++i )
		if ( this->_mat[i] != m._mat[i] ) {
			return false;
		}

	return true;
}

bool vsMat3::operator!=( const vsMat3& m ) const {
	return !( *this == m );
}

//==============
// vsMat3::LoadIdentity
//==============
void vsMat3::LoadIdentity( void ) {
	*_mat = *vsMat3( 1,0,0,
			   	  0,1,0,
				  0,0,1 )._mat;
}

//==============
// vsMat3::Transpose
//==============
vsMat3	vsMat3::Transpose( void ) const {
	vsMat3 trans;

	trans[0] = _mat[0];
	trans[1] = _mat[3];
	trans[2] = _mat[6];

	trans[3] = _mat[1];
	trans[4] = _mat[4];
	trans[5] = _mat[7];

	trans[6] = _mat[2];
	trans[7] = _mat[5];
	trans[8] = _mat[8];

	return trans;
}

//==============
// vsMat3::Inverse
//==============
vsMat3	vsMat3::Inverse( void ) const {
	vsMat3 mat = Adjung() * ( 1/Determinant() );
	return mat;
}

//==============
// vsMat3::Determinant
//==============
float vsMat3::Determinant( void ) const {
	return ( _mat[0]  *  _mat[4] * _mat[8] + 
			_mat[3]  *  _mat[7] * _mat[2] + 
			_mat[6]  *  _mat[1] * _mat[5]   ) 
		                  - 
		   ( _mat[6]  *  _mat[4] * _mat[2] + 
			_mat[3]  *  _mat[1] * _mat[8] + 
			_mat[0]  *  _mat[7] * _mat[5]   );
}

//==============
// vsMat3::Adjung
//==============
vsMat3 vsMat3::Adjung( void ) const {
	vsMat3	adjung;
	float	det[9];

	det[0] = vsMat2( _mat[4], _mat[7], _mat[5], _mat[8] ).Determinant();
	det[1] = vsMat2( _mat[3], _mat[6], _mat[5], _mat[8] ).Determinant();
	det[2] = vsMat2( _mat[3], _mat[6], _mat[4], _mat[7] ).Determinant();

	det[3] = vsMat2( _mat[1], _mat[7], _mat[2], _mat[8] ).Determinant();
	det[4] = vsMat2( _mat[0], _mat[6], _mat[2], _mat[8] ).Determinant();
	det[5] = vsMat2( _mat[0], _mat[6], _mat[1], _mat[7] ).Determinant();

	det[6] = vsMat2( _mat[1], _mat[4], _mat[2], _mat[5] ).Determinant();
	det[7] = vsMat2( _mat[0], _mat[3], _mat[2], _mat[5] ).Determinant();
	det[8] = vsMat2( _mat[0], _mat[3], _mat[1], _mat[4] ).Determinant();

	adjung = vsMat3 ( det[0],   -det[0],    det[0],
				   -det[0],    det[0],   -det[0],
					det[0],   -det[0],    det[0] );

	return adjung;
}

//==============
// vsMat3::IsProjective
//==============
bool vsMat3::IsProjective( void ) {
	//vsMat3 sqr_mat = *this * *this;
	return ( *( *this * *this )._mat == *_mat )? true : false;
}

//==============
// vsMat3::IsIdentity
//==============
bool vsMat3::IsIdentity( void ) {
	vsMat3 Diagonal_mat;
	Diagonal_mat.LoadIdentity();
	return ( *_mat == *Diagonal_mat._mat )? true : false;
}

//==============
// vsMat3::TovsMat4
//==============
vsMat4	vsMat3::ToMat4( void ) {
	return vsMat4( _mat[0], _mat[3], _mat[6], 0, 
				_mat[1], _mat[4], _mat[7], 0,
				_mat[2], _mat[5], _mat[8], 0, 
				0,		0,		 0,		   1  );
}

//==============
// vsMat3::Rotate()
//==============
vsMat3& vsMat3::Rotate( float x, float y, float z ) {
	vsQuaternion rx;
	rx.CreateFromEulerAnglesRAD( x, y, z );

	*this = ( rx ).ToMat3();
	return( *this );
}

vsMat3& vsMat3::Rotate( const vsVec3f& r ) {
	return Rotate( r.x, r.y, r.z );
}

float* vsMat3::ToTypePtr( void ){
	return ( &_mat[0] );
}

/*==============
    vsMat4
  ==============*/

vsMat4::vsMat4( void ) {
	LoadIdentity();
}

vsMat4::vsMat4( float m11,	 float m12,  float m13,	 float m14,
   		   float m21,	 float m22,  float m23,	 float m24,
		   float m31,	 float m32,  float m33,	 float m34,
		   float m41,	 float m42,  float m43,	 float m44 ) {
	_mat[0] = m11;	_mat[4] = m12;	_mat[8]  = m13;	_mat[12] = m14;
	_mat[1] = m21;	_mat[5] = m22;	_mat[9]  = m23;	_mat[13] = m24;
	_mat[2] = m31;	_mat[6] = m32;	_mat[10] = m33;	_mat[14] = m34;
	_mat[3] = m41;	_mat[7] = m42;	_mat[11] = m43;	_mat[15] = m44;
}

vsMat4::vsMat4( float mat[] ) {
	for ( int i=0; i<16; ++i ) 
		_mat[i] = mat[i];
}

vsMat4::vsMat4( const vsMat3& mat ) {
	_mat[0] = mat._mat[0];	_mat[4] = mat._mat[3];	_mat[8]  = mat._mat[6];	_mat[12] = 0.0f;
	_mat[1] = mat._mat[1];	_mat[5] = mat._mat[4];	_mat[9]  = mat._mat[7];	_mat[13] = 0.0f;
	_mat[2] = mat._mat[2];	_mat[6] = mat._mat[5];	_mat[10] = mat._mat[8];	_mat[14] = 0.0f;
	_mat[3] = 0.0f;			_mat[7] = 0.0f;			_mat[11] = 0.0f;		_mat[15] = 1.0f;
}

//==============
// vsMat4::operator+
//==============
vsMat4	vsMat4::operator+( const vsMat4& m ) const {
	vsMat4 mat;
	for ( int i=0; i<16; ++i ) {
		mat[i] = m._mat[i] + this->_mat[i];
	}
	return mat;
}

vsMat4 vsMat4::operator+( float m ) const {
	vsMat4 b = *this;
	for ( int i=0; i<16; ++i )
		b._mat[i] += m;
	return b;
}

//==============
// vsMat4::operator-
//==============
vsMat4	vsMat4::operator-( const vsMat4& m ) const {
	vsMat4 mat;
	for ( int i=0; i<16; ++i ) {
		mat[i] = m._mat[i] - this->_mat[i];
	}
	return mat;
}

//==============
// vsMat3::Operator -
//==============
vsMat4 vsMat4::operator-( void ) const {
	vsMat4 mat;
	for ( int i=0; i<16; ++i )
		mat._mat[i] =  - _mat[i];

	return mat;
}

//==============
// vsMat4::operator[]
//==============
float&	vsMat4::operator[]( int i ) {
    static float e = 0.0f;
	return ( ( i<0 ) || ( i>15 ) )? e  : _mat[i];
}

//==============
// vsMat4::operator*( vsMat4 )
//==============
vsMat4	vsMat4::operator*( const vsMat4 &m ) const {
	vsMat4 mat;

	mat[0]  = _mat[0] * m._mat[0]  + _mat[4] * m._mat[1]  + _mat[8]  * m._mat[2] + _mat[12] * m._mat[3];
	mat[1]  = _mat[1] * m._mat[0]  + _mat[5] * m._mat[1]  + _mat[9]  * m._mat[2] + _mat[13] * m._mat[3];
	mat[2]  = _mat[2] * m._mat[0]  + _mat[6] * m._mat[1]  + _mat[10] * m._mat[2] + _mat[14] * m._mat[3];
	mat[3]  = _mat[3] * m._mat[0]  + _mat[7] * m._mat[1]  + _mat[11] * m._mat[2] + _mat[15] * m._mat[3];

	mat[4]  = _mat[0] * m._mat[4]  + _mat[4] * m._mat[5] + _mat[8]  * m._mat[6] + _mat[12] * m._mat[7];
	mat[5]  = _mat[1] * m._mat[4]  + _mat[5] * m._mat[5] + _mat[9]  * m._mat[6] + _mat[13] * m._mat[7];
	mat[6]  = _mat[2] * m._mat[4]  + _mat[6] * m._mat[5] + _mat[10] * m._mat[6] + _mat[14] * m._mat[7];
	mat[7]  = _mat[3] * m._mat[4]  + _mat[7] * m._mat[5] + _mat[11] * m._mat[6] + _mat[15] * m._mat[7];

	mat[8]  = _mat[0] * m._mat[8]  + _mat[4] * m._mat[9] + _mat[8]  * m._mat[10] + _mat[12] * m._mat[11];
	mat[9]  = _mat[1] * m._mat[8]  + _mat[5] * m._mat[9] + _mat[9]  * m._mat[10] + _mat[13] * m._mat[11];
	mat[10] = _mat[2] * m._mat[8]  + _mat[6] * m._mat[9] + _mat[10] * m._mat[10] + _mat[14] * m._mat[11];
	mat[11] = _mat[3] * m._mat[8]  + _mat[7] * m._mat[9] + _mat[11] * m._mat[10] + _mat[15] * m._mat[11];

	mat[12] = _mat[0] * m._mat[12] + _mat[4] * m._mat[13] + _mat[8]  * m._mat[14] + _mat[12] * m._mat[15];
	mat[13] = _mat[1] * m._mat[12] + _mat[5] * m._mat[13] + _mat[9]  * m._mat[14] + _mat[13] * m._mat[15];
	mat[14] = _mat[2] * m._mat[12] + _mat[6] * m._mat[13] + _mat[10] * m._mat[14] + _mat[14] * m._mat[15];
	mat[15] = _mat[3] * m._mat[12] + _mat[7] * m._mat[13] + _mat[11] * m._mat[14] + _mat[15] * m._mat[15];

	return mat;
}

//==============
// vsMat4::operator*( float )
//==============
vsMat4 vsMat4::operator*( const float s ) const {
	vsMat4 mat;
	for ( int i=0; i<15; ++i )
		mat._mat[i] = _mat[i] * s;

	return mat;	
}

//==============
// vsMat4::operator*( vsVec4f )
//==============
vsVec4f	vsMat4::operator*( const vsVec4f& v ) const {
	return vsVec4f( _mat[0] * v.x + _mat[4] * v.y + _mat[8]  * v.z + _mat[12] * v.w,
				  _mat[1] * v.x + _mat[5] * v.y + _mat[9]  * v.z + _mat[13] * v.w,
				  _mat[2] * v.x + _mat[6] * v.y + _mat[10] * v.z + _mat[14] * v.w,
				  _mat[3] * v.x + _mat[7] * v.y + _mat[11] * v.z + _mat[15] * v.w  );
}

vsMat4& vsMat4::operator=( const vsMat4& m ) {
	for ( int i=0; i<16; ++i ) 
		this->_mat[i] = m._mat[i];
	return * this;
}

vsMat4& vsMat4::operator*=( const vsMat4& m ) {
	*this = *this * m;
	return * this;
}

vsMat4& vsMat4::operator*=( const float& s ) {
	*this = *this * s;
	return * this;
}

vsMat4 vsMat4::operator/( float s ) const {
	return *this * ( 1.0f/s );
}

vsMat4 vsMat4::operator/( const vsMat4& m ) const {
	return this->Transpose() * m;
}

vsVec4f vsMat4::operator/( const vsVec4f& v ) const {
   return vsVec4f( _mat[0] / v.x + _mat[4] / v.y + _mat[8]  / v.z + _mat[12] / v.w,
				  _mat[1] / v.x + _mat[5] / v.y + _mat[9]  / v.z + _mat[13] / v.w,
				  _mat[2] / v.x + _mat[6] / v.y + _mat[10] / v.z + _mat[14] / v.w,
				  _mat[3] / v.x + _mat[7] / v.y + _mat[11] / v.z + _mat[15] / v.w  );
}

vsMat4& vsMat4::operator/=( const float& s ) {
	*this = *this / s;
	return *this;
}

vsMat4& vsMat4::operator/=( const vsMat4& m ) {
	*this = *this / m;
	return *this;
}


vsMat4& vsMat4::operator+=( const float& s ) {
	*this = *this + s;
	return * this;
}

vsMat4& vsMat4::operator+=( const vsMat4& m ) {
	*this = *this + m;
	return * this;
}

vsMat4& vsMat4::operator-=( const float& s ) {
	*this = *this - s;
	return * this;
}

vsMat4& vsMat4::operator-=( const vsMat4& m ) {
	*this = *this - m;
	return * this;
}

vsMat4 vsMat4::operator-( float s ) const {
	vsMat4 m;
	for ( int i=0; i<16; ++i )
		m._mat[i] = this->_mat[i] - s;
	return m;
}

bool vsMat4::operator==( const vsMat4& m ) const{
	for ( int i=0; i<16; ++i )
		if ( this->_mat[i] != m._mat[i] ) {
			return false;
		}

	return true;
}

bool vsMat4::operator!=( const vsMat4& m ) const {
	return !( *this == m );
}

//==============
// vsMat4::LoadIdentity
//==============
void vsMat4::LoadIdentity( void ) {
	vsMat4 mat = vsMat4( 1,0,0,0,
			   		0,1,0,0,
					0,0,1,0,
					0,0,0,1 );
	*this = mat;
}

//==============
// vsMat4::Transpose
//==============
vsMat4 vsMat4::Transpose( void ) const{
	vsMat4 mat = vsMat4( _mat[0],	_mat[1],	_mat[2],	_mat[3],
					 _mat[4],	_mat[5],	_mat[6],	_mat[7], 
					 _mat[8],	_mat[9],	_mat[10],	_mat[11], 
					 _mat[12],	_mat[13],	_mat[14],   _mat[15] );
	return mat;
}

//==============
// vsMat4::Inverse
//==============
vsMat4 vsMat4::Inverse( void ) const {
	vsMat4 mat = Adjung() * ( 1/Determinant() );
	return mat;
}

//==============
// vsMat4::Determinant
//==============
float vsMat4::Determinant( void ) const {
	return ( _mat[0]  * _mat[5]  * _mat[10] * _mat[15] + 
		    _mat[4]  * _mat[9]  * _mat[14] * _mat[3]  +
			_mat[8]  * _mat[13] * _mat[2]  * _mat[7]  +
			_mat[12] * _mat[1]  * _mat[6]  * _mat[11]   ) 
			                   -
		   ( _mat[12] * _mat[9]  * _mat[6]  * _mat[3]  + 
		    _mat[8]  * _mat[5]  * _mat[2]  * _mat[15] +
			_mat[4]  * _mat[1]  * _mat[14] * _mat[11] +
			_mat[0]  * _mat[13] * _mat[10] * _mat[7]		 );
}


//==============
// vsMat4::Adjung
//==============
vsMat4	vsMat4::Adjung( void ) const {
	vsMat4	adjung;
	float	det[16];

	det[0]  =  vsMat3(  _mat[5],    _mat[9],    _mat[13],
					  _mat[6],    _mat[10],   _mat[14],
					  _mat[7],    _mat[11],   _mat[15] ).Determinant();

	det[1]  =  vsMat3(  _mat[4],    _mat[8],    _mat[12],
					  _mat[6],    _mat[10],   _mat[14],
					  _mat[7],    _mat[11],   _mat[15] ).Determinant();

	det[2]  =  vsMat3(  _mat[4],    _mat[8],    _mat[12],
					  _mat[5],    _mat[9],    _mat[13],
					  _mat[7],    _mat[11],   _mat[15] ).Determinant();

	det[3]  =  vsMat3(  _mat[4],    _mat[8],    _mat[12],
					  _mat[5],    _mat[9],    _mat[13],
					  _mat[6],    _mat[10],   _mat[14] ).Determinant();

	det[4]  =  vsMat3(  _mat[1],    _mat[9],     _mat[13],
					  _mat[2],    _mat[10],    _mat[14],
					  _mat[3],    _mat[11],    _mat[15] ).Determinant();

	det[5]  =  vsMat3(  _mat[0],    _mat[8],    _mat[12],
					  _mat[2],    _mat[10],   _mat[14],
					  _mat[3],    _mat[11],   _mat[15] ).Determinant();

	det[6]  =  vsMat3(  _mat[0],    _mat[8],    _mat[12],
					  _mat[1],    _mat[9],    _mat[13],
					  _mat[3],    _mat[11],   _mat[15] ).Determinant();

	det[7]  =  vsMat3(  _mat[0],    _mat[8],    _mat[12],
					  _mat[1],    _mat[9],    _mat[13],
					  _mat[2],    _mat[10],   _mat[14] ).Determinant();

	det[8]  =  vsMat3(  _mat[1],    _mat[5],    _mat[13],
					  _mat[2],    _mat[6],    _mat[14],
					  _mat[3],    _mat[7],    _mat[15] ).Determinant();

	det[9]  =  vsMat3(  _mat[0],    _mat[4],    _mat[12],
					  _mat[2],    _mat[6],    _mat[14],
					  _mat[3],    _mat[7],    _mat[15] ).Determinant();

	det[10] =  vsMat3(  _mat[0],    _mat[4],    _mat[12],
					  _mat[1],    _mat[5],    _mat[13],
					  _mat[3],    _mat[7],    _mat[15] ).Determinant();

	det[11] =  vsMat3(  _mat[0],    _mat[4],    _mat[12],
					  _mat[1],    _mat[5],    _mat[13],
					  _mat[2],    _mat[6],    _mat[14] ).Determinant();

	det[12] =  vsMat3(  _mat[1],    _mat[5],    _mat[9],
					  _mat[2],    _mat[6],    _mat[10],
					  _mat[3],    _mat[7],    _mat[11] ).Determinant();

	det[13] =  vsMat3(  _mat[0],    _mat[4],    _mat[8],
					  _mat[2],    _mat[6],    _mat[10],
					  _mat[3],    _mat[7],    _mat[11] ).Determinant();

	det[14] =  vsMat3(  _mat[0],    _mat[4],    _mat[8],
					  _mat[1],    _mat[5],    _mat[9],
					  _mat[3],    _mat[7],    _mat[11] ).Determinant();

	det[15] =  vsMat3(  _mat[0],    _mat[4],    _mat[8],
					  _mat[1],    _mat[5],    _mat[9],
					  _mat[2],    _mat[6],    _mat[10] ).Determinant();

	adjung = vsMat4( 
			det[0],   -det[4],    det[8],    -det[12],
		   -det[1],    det[5],   -det[9],     det[13],
		    det[2],   -det[6],    det[10],   -det[14],
		   -det[3],    det[7],   -det[11],    det[15]
	 );

	return adjung;
}

//==============
// vsMat4::IsProjective
//==============
bool vsMat4::IsProjective( void ) {
	vsMat4 sqr_mat = *this * *this;

	return ( *sqr_mat._mat == *_mat )? true : false;
}

//==============
// vsMat4::IsIdentity
//==============
bool vsMat4::IsIdentity( void ) {
	vsMat4 Diagonal_mat;
	Diagonal_mat.LoadIdentity();
	return ( *_mat == *Diagonal_mat._mat )? true : false;
}

//==============
// vsMat4::Scale
//==============
void vsMat4::Scale( float x, float y, float z )  {
	_mat[0] = x;
	_mat[5] = y;
	_mat[10] = z;
}

void vsMat4::Scale( const vsVec3f& factor ) {
	Scale( factor.x, factor.y, factor.z );
}

//==============
// vsMat4::Translate
//==============
void vsMat4::Translate( float x, float y, float z ) {
	vsMat4 trans = vsMat4( 1, 0, 0, x,
					  0, 1, 0, y,
					  0, 0, 1, z,
					  0, 0, 0, 1  );
	*this = *this * trans;
}

//==============
// vsMat4::Translate
//==============
void vsMat4::Translate( const vsVec3f& pos ) {
	Translate( pos.x, pos.y, pos.z );
}

//==============
// vsMat4::Rotate
//==============
vsMat4& vsMat4::Rotate( float x, float y, float z ) {
	
	vsQuaternion R;
	R.CreateFromEulerAnglesDEG( x, y, z );

	*this = R.ToMat4();
	return( *this );
}

vsMat4& vsMat4::Rotate( const vsVec3f& r ) {
	return Rotate( r.x, r.y, r.z );
}

float* vsMat4::ToTypePtr( void ) {
	return ( &_mat[0] );
}

vsMat3 vsMat4::ToMat3( void ) {
	return vsMat3( _mat[0], _mat[4], _mat[8],
				   _mat[1], _mat[5], _mat[9],
				   _mat[2], _mat[6], _mat[10] );
}


/*
===============================
  Sets up an OpenGL projection matrix
  without a far plane.

  fielOfView in RAD!
===============================
*/
void vsMat4::SetupInfProjMatrix( float fieldOfView, float aspect, float nearPlane ) 
{	
	float f  = 1.0f / tanf( fieldOfView * 0.5f );

	ZeroMemory( _mat, sizeof( float )*16 );
	_mat[0] = f;
	_mat[5] = f/aspect;
	_mat[10] = -1.0f;
	_mat[11] = -1.0f;
	_mat[14] = -2.0f * nearPlane;
}