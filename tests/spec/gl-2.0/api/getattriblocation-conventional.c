/*
 * Copyright © 2011 Intel Corporation
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
 * \file getattriblocation-conventional.c
 * Verify that glGetAttribLocation on a conventional attribute returns -1.
 *
 * \author Ian Romanick
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    100 /*window_width*/,
    100 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE)

static const char *vs_code =
	"attribute vec4 not_used;\n"
	"void main() { gl_Position = gl_Vertex; }"
	;

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}

void piglit_init(int argc, char **argv)
{
	GLint vert;
	GLint prog;
	GLint loc;

	piglit_require_vertex_shader();
	vert = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_code);
	prog = piglit_link_simple_program(vert, 0);

	loc = piglit_GetAttribLocation(prog, "gl_Vertex");
	printf("Attribute location reported for gl_Vertex is %d.\n", loc);

	piglit_report_result(loc == -1 ? PIGLIT_PASS : PIGLIT_FAIL);
}
