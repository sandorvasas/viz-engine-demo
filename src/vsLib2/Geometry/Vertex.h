#pragma once

#include "../Math/Math.h"


/*
=====================================================

	vsSimpleVertex

=====================================================
*/ 
class vsSimpleVertex {
public:
	EMPTY_TRUCTORS ( vsSimpleVertex );
	MINIMUM_OPERATORS ( vsSimpleVertex );

public:
	vsVec3f		xyz;
	vsVec2f		st;
};

/*
=====================================================

	vsVertex

=====================================================
*/ 
class vsVertex {
public:
						vsVertex( void );
	explicit			vsVertex( const vsVec3f& xyz );
	explicit			vsVertex( const vsVec3f& xyz, const vsVec2f& st );
	explicit			vsVertex( const vsVec3f& xyz, const vsVec3f& normal, const vsVec2f& st, const vsVec4f& color );
			 			~vsVertex( void );

	 vsVertex&	operator=( const vsVertex& v );
	 bool		operator==( const vsVertex& v );
	 bool		operator!=( const vsVertex& v );
	 vsVertex&	operator+=( const vsVertex& v );
	 vsVertex&	operator-=( const vsVertex& v );

	 vsVertex&	CalculateNormal( const vsVertex& neighbor1, const vsVertex& neighbor2 );
	 vsVertex&	CalculateTangent( const vsVertex& neighbor1, const vsVertex& neighbor2 );

public:
	vsVec3f				xyz;		// at offset: 0
	vsVec3f				Normal;		// at offset: 12
	vsVec3f				Tangent;	// at offset: 24
	vsVec4f				Color;		// at offset: 36
	vsVec2f				st;			// at offset: 52
	byte				reserved[4];// at offset: 60
	//vsVec3f			stu[3];		// at offset: 60
	// Total Size: 64 Byte

	enum {
		Offset_xyz = 0,
		Offset_Normal = 12,
		Offset_Tangent = 24,
		Offset_Color = 36,
		Offset_st = 52,
		Offset_reserved = 60
	};
};