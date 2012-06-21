/*
 * Copyright © 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 * \file glsl-invalid-asm-01.c
 * Attempting to render with an invalid ARB_fp shader should generate an error.
 *
 * Based on code inspection it was found that Mesa does not correctly generate
 * the draw-time error if a GLSL shader is used with an invalid assembly
 * (GL_ARB_vertex_program or GL_ARB_fragment_program) program.  This test
 * attempts to reproduce this failure using an assembly fragment program.
 */
#include "piglit-util-gl-common.h"

static const char vs_text[] =
	"void main() { gl_Position = gl_Vertex;"
	"gl_FrontColor = vec4(1.0, 0.0, 0.0, 1.0); }";

static const char fp_text[] =
	"this won't compile";

PIGLIT_GL_TEST_MAIN(
    100 /*window_width*/,
    100 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE)

enum piglit_result
piglit_display(void)
{
	static GLboolean logged = GL_FALSE;
	GLboolean pass = GL_TRUE;
	GLenum err;

	glClearColor(0.5, 0.5, 0.5, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	piglit_draw_rect(-1.0, -1.0, 2.0, 2.0);

        err = glGetError();
        if (err != GL_INVALID_OPERATION) {
		if (!logged) {
			printf("Unexpected OpenGL error state 0x%04x with bad "
			       "fragment program at draw (expected 0x%04x).\n",
			       err, GL_INVALID_OPERATION);
			logged = GL_TRUE;
		}

		pass = GL_FALSE;
        }
	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint vs;
	GLuint fp;
	GLuint prog;
	GLenum err;

	if (piglit_get_gl_version() < 20) {
		printf("Requires OpenGL 2.0\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_extension("GL_ARB_fragment_program");

	vs = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_text);
	prog = piglit_link_simple_program(vs, 0);
	glUseProgram(prog);

	glGenProgramsARB(1, &fp);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fp);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
                           GL_PROGRAM_FORMAT_ASCII_ARB,
                           strlen(fp_text),
                           (const GLubyte *) fp_text);

        err = glGetError();
        if (err != GL_INVALID_OPERATION) {
                printf("Unexpected OpenGL error state 0x%04x with bad "
		       "fragment program (expected 0x%04x).\n",
		       err, GL_INVALID_OPERATION);
		piglit_report_result(PIGLIT_FAIL);
        }

	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	/* Clear all GL error state.
	 */
	while (glGetError() != 0)
		/* empty */ ;
}
