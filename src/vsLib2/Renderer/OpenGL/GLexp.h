// Copyright ( C ) 2006-2007 by sonka

#ifndef __CRITEC_GLEXP_H__
#define __CRITEC_GLEXP_H__

#include <windows.h>
#include "GL.h"
#include "GL3.h"
#include "GLext.h"
#include "WGLext.h"

#pragma comment ( lib, "Opengl32.lib" )


namespace vsGL {

	char*	GetOpenGLVersion( void );
	bool	LoadOpenGLCore_2_0( void );
	bool	LoadOpenGLCore_3_0( void );
	bool	LoadOpenGLCore_3_2( void );
	bool	LoadOpenGLCore_4_0( void );
	bool	LoadOpenGLCore_4_2( void );

	bool	IsGLExtensionSupported( char* extname );
	bool	IsWGLExtensionSupported( char* extname );

	bool	LoadEXT_WGL_ARB_extensions_string( void );
	bool	LoadEXT_WGL_EXT_swap_control( void );
	bool	LoadEXT_WGL_ARB_pixel_format( void );

	bool	LoadEXT_GL_EXT_framebuffer_object( void );
	bool	LoadEXT_GL_EXT_framebuffer_blit( void );
	bool	LoadEXT_GL_EXT_blend_minmax( void );
	bool	LoadEXT_GL_EXT_framebuffer_multisample( void );
	bool	LoadEXT_GL_NV_framebuffer_multisample_coverage( void );
	bool	LoadEXT_GL_EXT_geometry_shader4( void );
	bool	LoadEXT_GL_EXT_direct_state_access( void );

}

/* CORE 2.0*/
extern PFNGLMULTITEXCOORD1FPROC									glMultiTexCoord1f			;
extern PFNGLMULTITEXCOORD2FPROC									glMultiTexCoord2f			;
extern PFNGLMULTITEXCOORD2FVPROC								glMultiTexCoord2fv			;
extern PFNGLMULTITEXCOORD3FPROC									glMultiTexCoord3f			;
extern PFNGLMULTITEXCOORD4FPROC									glMultiTexCoord4f			;
extern PFNGLACTIVETEXTUREPROC								    glActiveTexture				;
extern PFNGLCLIENTACTIVETEXTUREPROC								glClientActiveTexture		;
extern PFNGLUSEPROGRAMPROC										glUseProgram				;
extern PFNGLCREATEPROGRAMPROC									glCreateProgram				;
extern PFNGLDELETEPROGRAMPROC									glDeleteProgram				;
extern PFNGLCREATESHADERPROC									glCreateShader				;
extern PFNGLDELETESHADERPROC									glDeleteShader				;
extern PFNGLSHADERSOURCEPROC									glShaderSource				;
extern PFNGLATTACHSHADERPROC									glAttachShader				;
extern PFNGLLINKPROGRAMPROC										glLinkProgram				;
extern PFNGLCOMPILESHADERPROC									glCompileShader				;
extern PFNGLGENQUERIESPROC										glGenQueries				;
extern PFNGLDELETEQUERIESPROC									glDeleteQueries				;
extern PFNGLISQUERYPROC											glIsQuery					;
extern PFNGLBEGINQUERYPROC										glBeginQuery				;
extern PFNGLENDQUERYPROC										glEndQuery					;
extern PFNGLGETQUERYOBJECTIVPROC								glGetQueryObjectiv;
extern PFNGLBEGINCONDITIONALRENDERPROC							glBeginConditionalRender;
extern PFNGLENDCONDITIONALRENDERPROC							glEndConditionalRender;
extern PFNGLPOINTPARAMETERFPROC									glPointParameterf			;
extern PFNGLPOINTPARAMETERFVPROC								glPointParameterfv			;
extern PFNGLGETUNIFORMLOCATIONPROC								glGetUniformLocation		;
extern PFNGLGETSHADERSOURCEPROC									glGetShaderSource;
extern PFNGLUNIFORM1FPROC										glUniform1f;
extern PFNGLUNIFORM2FPROC										glUniform2f;
extern PFNGLUNIFORM3FPROC										glUniform3f;
extern PFNGLUNIFORM4FPROC										glUniform4f;
extern PFNGLUNIFORM1IPROC										glUniform1i;
extern PFNGLUNIFORM2IPROC										glUniform2i;
extern PFNGLUNIFORM3IPROC										glUniform3i;
extern PFNGLUNIFORM4IPROC										glUniform4i;
extern PFNGLUNIFORM2FVPROC										glUniform2fv;
extern PFNGLUNIFORM3FVPROC										glUniform3fv;
extern PFNGLUNIFORM4FVPROC										glUniform4fv;
extern PFNGLUNIFORMMATRIX2FVPROC								glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC								glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC								glUniformMatrix4fv;
extern PFNGLISPROGRAMPROC										glIsProgram;
extern PFNGLBLENDFUNCSEPARATEPROC								glBlendFuncSeparate;
extern PFNGLFOGCOORDFPROC										glFogCoordf;
extern PFNGLFOGCOORDFVPROC										glFogCoordfv;
extern PFNGLFOGCOORDDPROC										glFogCoordd;
extern PFNGLFOGCOORDDVPROC										glFogCoordddv;
extern PFNGLGENBUFFERSPROC										glGenBuffers;
extern PFNGLBINDBUFFERPROC										glBindBuffer;
extern PFNGLBUFFERDATAPROC										glBufferData;
extern PFNGLMAPBUFFERPROC										glMapBuffer;
extern PFNGLUNMAPBUFFERPROC										glUnmapBuffer;
extern PFNGLBUFFERSUBDATAPROC									glBufferSubData;
extern PFNGLDELETEBUFFERSPROC									glDeleteBuffers;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC							glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC							glCompressedTexImage3D;
extern PFNGLTEXIMAGE3DPROC										glTexImage3D;
extern PFNGLDRAWBUFFERSPROC										glDrawBuffers;
extern PFNGLBINDATTRIBLOCATIONPROC								glBindAttribLocation;
extern PFNGLVERTEXATTRIB3FPROC									glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC									glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB2FPROC									glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC									glVertexAttrib2fv;
extern PFNGLGETATTRIBLOCATIONPROC								glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC								glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC							glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC						glDisableVertexAttribArray;
extern PFNGLMULTIDRAWELEMENTSPROC								glMultiDrawElements;
extern PFNGLISBUFFERPROC										glIsBuffer;

/*
=========================================================================================================

		WGL EXTENSIONS

=========================================================================================================
*/
/* WGL_ARB_extensions_string */
extern PFNWGLGETEXTENSIONSSTRINGARBPROC							wglGetExtensionsStringARB;
/* WGL_ARB_pixelformat */
extern PFNWGLCHOOSEPIXELFORMATARBPROC							wglChoosePixelFormatARB;
/* WGL_ARB_create_context_attribs */
extern PFNWGLCREATECONTEXTATTRIBSARBPROC						wglCreateContextAttribsARB;

/* WGL_EXT_swap_control */
extern PFNWGLSWAPINTERVALEXTPROC								wglSwapIntervalEXT;


/*
=========================================================================================================

		GL EXTENSIONS

=========================================================================================================
*/
/* GL_EXT_framebuffer_blit */
extern PFNGLBLITFRAMEBUFFEREXTPROC								glBlitFramebufferEXT;

/* GL_EXT_framebuffer_multisample */
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC				glRenderbufferStorageMultisampleEXT;

/* GL_NV_framebuffer_multisample_coverage */
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC		glRenderbufferStorageMultisampleCoverageNV;

/* GL_EXT_blend_minmax */
extern PFNGLBLENDEQUATIONEXTPROC								glBlendEquationEXT;

/* GL_EXT_framebuffer_object */
extern PFNGLRENDERBUFFERSTORAGEPROC						glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC					glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC								glIsFrameBuffer;
extern PFNGLBINDFRAMEBUFFERPROC							glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC							glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC							glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC						glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC							glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC							glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC							glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC					glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC		glGetFramebufferAttachmentParameteriv;
extern PFNGLGENRENDERBUFFERSPROC							glGenRenderbuffers;
extern PFNGLDELETERENDERBUFFERSPROC						glDeleteRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC							glBindRenderbuffer;
extern PFNGLISRENDERBUFFERPROC								glIsRenderbuffer;


/*GL_EXT_geometry_shader4*/
extern PFNGLPROGRAMPARAMETERIEXTPROC							glProgramParameteriEXT;
extern PFNGLFRAMEBUFFERTEXTUREEXTPROC							glFramebufferTextureEXT;
extern PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC						glFramebufferTextureLayerEXT;
extern PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC						glFramebufferTextureFaceEXT;

/*GL_EXT_direct_state_access*/
extern PFNGLBINDMULTITEXTUREEXTPROC								glBindMultiTextureEXT;

#endif