#ifndef __VS_SHADERMANAGER_H__
#define __VS_SHADERMANAGER_H__


#include "OpenGL\GLexp.h"
#include "..\Math\Math.h"
#include "..\System\String.h"
#include "..\System\List.h"

#define UNIFORM_CACHING 1
#undef UNIFORM_CACHING

/*
===========================
	shaderAttrib_t
===========================
*/
typedef struct _shaderAttrib_t {
	vsStr			name;
	unsigned int	loc;
} shaderAttrib_t;

/*
===========================
	Shader Unform structs
===========================
*/
template<class type>
struct uniformVal_t {
public:
	type	value;
	vsStr	name;

	uniformVal_t<type>& operator=( uniformVal_t<type> r ) {
		value = r.value;
		name = r.name;
		return( *this );
	}

	uniformVal_t<type>& operator==( uniformVal_t<type> r ) {
		return( value == r.value && name == r.name );
	}

	uniformVal_t<type>& operator!=( uniformVal_t<type> r ) {
		return !( *this == r );
	}
};

/*
==============================================

	vsShader

==============================================
*/
class vsShader {
public:
	vsShader( void ) : uniformCaching( false ) {};
	~vsShader( void ) {};

	vsShader&				operator=( const vsShader& b );
	bool					operator==( const vsShader& b );
	bool					operator!=( const vsShader& b );

	int						LoadFromText( const char* name, char** text, int numLines, int geomInputType, int geomOutputType );
	int						LoadFromFile( const char* name, char* shaderFile, int geomInputType, int geomOutputType );
	void					Bind( bool bind );
	void					Delete( void );

	void					Uniform1f( char* uniname, float f );
	void					Uniform1i( char* uniname, int f );
//	void					Uniform2f( char* uniname, float f1, float f2 );
	void					Uniform2v( char* uniname, const vsVec2f& v );
	void					Uniform2i( char* uniname, int f1, int f2 );
//	void					Uniform3f( char* uniname, float f1, float f2, float f3 );
	void					Uniform3v( char* uniname, const vsVec3f& v );
	void					Uniform3i( char* uniname, int f1, int f2, int f3 );
//	void					Uniform4f( char* uniname, float f1, float f2, float f3, float f4 );
	void					Uniform4v( char* uniname, const vsVec4f& v );
	void					Uniform4i( char* uniname, int f1, int f2, int f3, int f4 );
	void					UniformMatrix2x2( char* uniname, vsMat2* mat );
	void					UniformMatrix3x3( char* uniname, vsMat3* mat );
	void					UniformMatrix4x4( char* uniname, vsMat4* mat );
/*	void					SetGeometryShaderInputType( int type );
	void					SetGeometryShaderOutputType( int type );
*/
	unsigned int			AttribLoc( char* attribname );
	void					BindAttrib( int index, char* attribname );
	unsigned int			UniformLoc( char* uniformname );
	const unsigned int&		ProgramObject( void ) const;

	#ifdef UNIFORM_CACHING
	template<typename T>
	bool					UpdateUniformCache( vsList< uniformVal_t<T> > list, char* name, T value );
	#endif

public:
	vsStr					name;
	unsigned int			program;
	bool					uniformCaching;

	#ifdef UNIFORM_CACHING
	vsList<uniformVal_t<float>>		u_1f;
	vsList<uniformVal_t<vsVec2f>>	u_2f;
	vsList<uniformVal_t<vsVec3f>>	u_3f;
	vsList<uniformVal_t<vsVec4f>>	u_4f;
	vsList<uniformVal_t<int>>		u_1i;
	vsList<uniformVal_t<vsVec2i>>	u_2i;
	vsList<uniformVal_t<vsVec3i>>	u_3i;
	vsList<uniformVal_t<vsVec4i>>	u_4i;
	vsList<uniformVal_t<vsMat2>>	u_mat2f;
	vsList<uniformVal_t<vsMat3>>	u_mat3f;
	vsList<uniformVal_t<vsMat4>>	u_mat4f;
	#endif
};

/*
================================
	UpdateUniformCache
		Returns TRUE if uniformvalue has to be sent to shader
				FALSE otherwise
================================
*/
#ifdef UNIFORM_CACHING
template<typename T>
VS_INLINE
bool vsShader::UpdateUniformCache( vsList< uniformVal_t<T> > list, char* name, T value )
{

	if ( !uniformCaching ) 
		return true;

	bool found=false;
	for ( uint i=0; i<list.Length(); ++i )
		if ( vsStr( name ) == list[i].name && list[i].value != value ) 
		{
			   list[i].value = value;
			   found=true;
			   return false;
		}
	if ( !found ) {
		uniformVal_t<T> a;
		a.name = vsStr( name );
		a.value = value;
		list.Append( a );
	}
	
	return true;
}
#endif

/*
================================
		Bind
================================
*/
VS_INLINE void vsShader::Bind( bool bind ) {
	glUseProgram( ( bind )? program : 0 );
}

/*
================================
		Uniform1f
================================
*/
VS_INLINE void vsShader::Uniform1f( char* uniname, float f ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_1f, uniname, f ) )
	#endif
	glUniform1f( UniformLoc( uniname ), f );
}

/*
================================
		Uniform1i
================================
*/
VS_INLINE void vsShader::Uniform1i( char* uniname, int f ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_1i, uniname, f ) )
	#endif
	glUniform1i( UniformLoc( uniname ), f );
}

/*
================================
		Uniform2f
================================

VS_INLINE void vsShader::Uniform2f( char* uniname, float f1, float f2 ) {
	glUniform2f( UniformLoc( uniname ), f1, f2 );
}*/

/*
================================
		Uniform2v
================================
*/
VS_INLINE void vsShader::Uniform2v( char* uniname, const vsVec2f& v ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_2f, uniname, v ) )
	#endif
	glUniform2f( UniformLoc( uniname ),  v.x, v.y );
}

/*
================================
		Uniform2i
================================
*/
VS_INLINE void vsShader::Uniform2i( char* uniname, int f1, int f2 ) {
	glUniform2i( UniformLoc( uniname ), f1, f2 );
}

/*
================================
		Uniform3f
================================

VS_INLINE void vsShader::Uniform3f( char* uniname, float f1, float f2, float f3 ) {
	glUniform3f( UniformLoc( uniname ), f1, f2, f3 );
}*/

/*
================================
		Uniform3v
================================
*/
VS_INLINE void vsShader::Uniform3v( char* uniname, const vsVec3f& v  ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_3f, uniname, v ) )
	#endif
	glUniform3f( UniformLoc( uniname ), v.x, v.y, v.z );
}

/*
================================
		Uniform3i
================================
*/
VS_INLINE void vsShader::Uniform3i( char* uniname, int f1, int f2, int f3 ) {
	glUniform3i( UniformLoc( uniname ), f1, f2, f3 );
}

/*
================================
		Uniform4f
================================

VS_INLINE void vsShader::Uniform4f( char* uniname, float f1, float f2, float f3, float f4 ) {
	glUniform4f( UniformLoc( uniname ), f1, f2, f3, f4 );
}*/

/*
================================
		Uniform4v
================================
*/
VS_INLINE void vsShader::Uniform4v( char* uniname, const vsVec4f& v ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_4f, uniname, v ) )
	#endif
	glUniform4f( UniformLoc( uniname ), v.x, v.y, v.z, v.w );
}

/*
================================
		Uniform4i
================================
*/
VS_INLINE void vsShader::Uniform4i( char* uniname, int f1, int f2, int f3, int f4 ) {
	glUniform4i( UniformLoc( uniname ), f1, f2, f3, f4 );
}

/*
================================
		UniformMatrix2x2
================================
*/
VS_INLINE void vsShader::UniformMatrix2x2( char* uniname, vsMat2* mat ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_mat2f, uniname, *mat ) )
	#endif
	glUniformMatrix2fv( UniformLoc( uniname ), 4, 0, mat->ToTypePtr() );
}

/*
================================
		UniformMatrix3x3
================================
*/
VS_INLINE void vsShader::UniformMatrix3x3( char* uniname, vsMat3* mat ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_mat3f, uniname, *mat ) )
	#endif
	glUniformMatrix3fv( UniformLoc( uniname ), 9, 0, mat->ToTypePtr() );
}

/*
================================
		UniformMatrix4x4
================================
*/
VS_INLINE void vsShader::UniformMatrix4x4( char* uniname, vsMat4* mat ) {
	#ifdef UNIFORM_CACHING
	if ( UpdateUniformCache( u_mat4f, uniname, *mat ) )
	#endif
	glUniformMatrix4fv( UniformLoc( uniname ), 16*4, 0, mat->ToTypePtr() );
}

/*
================================
  SetGeometryShaderInputType
================================

VS_INLINE void vsShader::SetGeometryShaderInputType( int type ) {
	glProgramParameteriEXT( program, GL_GEOMETRY_INPUT_TYPE_EXT, type );
}*/

/*
================================
  SetGeometryShaderOutputType
================================

VS_INLINE void vsShader::SetGeometryShaderOutputType( int type ) {
	glProgramParameteriEXT( program, GL_GEOMETRY_OUTPUT_TYPE_EXT, type );
}*/

/*
================================
		   AttribLoc
================================
*/
VS_INLINE unsigned int vsShader::AttribLoc( char* attribname ) {
	return glGetAttribLocation( program, attribname );
}

/*
================================
		   BindAttrib
================================
*/
VS_INLINE void vsShader::BindAttrib( int index, char* attribname ) {
	glBindAttribLocation( program, index, attribname );
}

/*
================================
		  UniformLoc
================================
*/
VS_INLINE unsigned int vsShader::UniformLoc( char* uniformname ) {
	return glGetUniformLocation( program, uniformname );
}

/*
================================
		  ProgramObject
================================
*/
VS_INLINE const unsigned int& vsShader::ProgramObject( void ) const {
	return program;
}

#endif