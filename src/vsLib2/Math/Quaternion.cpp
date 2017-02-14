#include "Quaternion.h"					

/*
==========================
     operator*
==========================
*/
vsQuaternion vsQuaternion::operator*( const vsQuaternion& q ) {
	vsQuaternion yb;
	yb.x = q.x * w + q.w * x + y * q.z - z * q.y;
	yb.y = q.y * w + q.w * y + z * q.x - x * q.z;
	yb.z = q.z * w + q.w * z + x * q.y - y * q.x;
	yb.w = q.w * w - q.x*x - q.y*y - q.z*z;
	return yb;
}

/*
==========================
     operator*
==========================
*/
vsVec3f vsQuaternion::operator*( const vsVec3f& v ) {
	return( ToMat3() * v );
}

/*
==========================
     CreateFromEulerAnglesDEG
==========================
*/
void vsQuaternion::CreateFromEulerAnglesDEG( float x, float y, float z ) {
	// convert degrees to radians
	x *=  _PIOVER180;
	y *=  _PIOVER180;
	z *=  _PIOVER180;
	CreateFromEulerAnglesRAD( x, y, z );
}

/*
==========================
     CreateFromEulerAnglesDEG
==========================
*/
void vsQuaternion::CreateFromEulerAnglesDEG( const vsVec3f& v ) {
	CreateFromEulerAnglesDEG( v.x, v.y, v.z );
}

/*
==========================
     CreateFromEulerAnglesRAD
==========================
*/
void vsQuaternion::CreateFromEulerAnglesRAD( float x, float y, float z ) {
	vsQuaternion rx,ry,rz;
	x *= 0.5f; y *= 0.5f; z*= 0.5f;
	rx = vsQuaternion( sinf( x ), 0.0f, 0.0f, cosf( x ) );
	ry = vsQuaternion( 0.0f, sinf( y ), 0.0f, cosf( y ) );
	rz = vsQuaternion( 0.0f, 0.0f, sinf( z ), cosf( z ) );

	*this = rx * ry * rz;
}

/*
==========================
     CreateFromEulerAnglesRAD
==========================
*/
void vsQuaternion::CreateFromEulerAnglesRAD( const vsVec3f& v ) {
	CreateFromEulerAnglesRAD( v.x, v.y, v.z );
}
/*
void vsQuaternion::CreateFromAxisAngles( float a, float x, float y, float z ) {
//	*this = vsQuaternion( 
}

void vsQuaternion::CreateFromAxisAngles( float a, vsVec3f v ) {
}*/

/*
==========================
     ToMat3
==========================
*/
vsMat3 vsQuaternion::ToMat3( void ) const {
	vsMat3 m;
	m._mat[0] = 1.f -	2.f * ( y * y + z * z ); 
	m._mat[1] =		2.f * ( x * y + w * z );
	m._mat[2] =		2.f * ( x * z - w * y );

	m._mat[3] =		2.f * ( x * y - w * z );
	m._mat[4] = 1.f -	2.f * ( x * x + z * z );
	m._mat[5] =		2.f * ( y * z + w * x );

	m._mat[6] =		2.f * ( x * z + w * y );
	m._mat[7] =		2.f * ( y * z - w * x );
	m._mat[8] = 1.f - 2.f * ( x * x + y * y );
	return m;
}

/*
==========================
     ToMat4
==========================
*/
vsMat4 vsQuaternion::ToMat4( void ) const {
	return ToMat3().ToMat4();
}

/*
==========================
     Inverse
==========================
*/
vsQuaternion vsQuaternion::Inverse( void ) const {
	return( Conjugate() / LengthSqr() );
}

/*
==========================
     Normalize
==========================
*/
void vsQuaternion::Normalize( void ) {
	float len = Length();

	if ( len ) {
		float ilen = 1.0f / len;
		x *= ilen;
		y *= ilen;
		z *= ilen;
		w *= ilen;
	}
}



/*
==========================
     Lerp
==========================
*/
vsQuaternion& vsQuaternion::Lerp( const vsQuaternion& qa, const vsQuaternion& qb, float t ) {
	if ( t>=1.0f ) {
		*this = qb;
	} else if ( t<=0.0f ) {
		*this = qa;
	} else 
		*this = ( 1.0f-t )*qa + t*qb;
	return *this;
}

/*
==========================
     SLerp
==========================
*/
vsQuaternion& vsQuaternion::Slerp( const vsQuaternion& qa, const vsQuaternion& qb, float t ) {
	vsQuaternion temp;
	float scale0, scale1, cosom, omega, sinom;

	if ( t>=1.0f ) {
		*this = qb;
		return *this;
	}

	if ( t<=0.0f ) {
		*this = qa;
		return *this;
	}

	if ( qa==qb ) {
		*this = qb;
		return *this;
	}
	
		
	cosom = qa.Dot( qb );
	if ( cosom < 0.0f ) {
		temp = -qb;
		cosom = -cosom;
	} else {
		temp = qb;
	}
	
	if ( cosom < 0.9999f ) {
		
		omega = acosf( cosom );
		sinom = 1.0f / sinf( omega );
		scale0 = sinf( ( 1.0f-t )*omega ) * sinom;
		scale1 = sinf( t*omega ) * sinom;

	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	*this = scale0 * qa + scale1 * temp;

	return *this;
}
