#pragma once

// Minimal OpenGL include shim for the SDL2 port.
// - GLES builds use the platform GLES2 headers directly.
// - Desktop builds use SDL's OpenGL header with extension prototypes enabled.
// This removes the runtime/header dependency on GLEW while keeping the game's
// existing OpenGL-style call sites intact.

#ifdef USE_GLES
# if defined(__APPLE__)
#  include <OpenGLES/ES2/gl.h>
# else
#  include <GLES2/gl2.h>
# endif

// OpenGL ES 2.0 has no core VAO support. The GLES/NOVA renderer path binds
// vertex attributes explicitly at draw time, so these are safe compatibility
// no-ops for any stale guarded calls that may remain.
# ifndef GL_VERTEX_ARRAY_BINDING
#  define GL_VERTEX_ARRAY_BINDING 0x85B5
# endif
static inline void yug_glBindVertexArray(GLuint) {}
static inline void yug_glGenVertexArrays(GLsizei n, GLuint* arrays) {
    if (!arrays) return;
    for (GLsizei i = 0; i < n; ++i) arrays[i] = 0;
}
static inline void yug_glDeleteVertexArrays(GLsizei, const GLuint*) {}
# define glBindVertexArray yug_glBindVertexArray
# define glGenVertexArrays yug_glGenVertexArrays
# define glDeleteVertexArrays yug_glDeleteVertexArrays
#else
# ifndef GL_GLEXT_PROTOTYPES
#  define GL_GLEXT_PROTOTYPES 1
# endif
# include <SDL2/SDL_opengl.h>
# ifndef GL_BGR
#  define GL_BGR 0x80E0
# endif
#endif
