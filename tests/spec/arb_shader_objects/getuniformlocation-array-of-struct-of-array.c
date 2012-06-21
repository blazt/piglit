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
 * \file getuniformlocation-array-of-struct-of-array.c
 * Verify that the locations of members of an array of struct of array can be
 * queried using the glGetUniformLocation API.
 *
 * \author Ian Romanick
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    100 /*window_width*/,
    100 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE)

static const char *vs_code =
	"struct S { mat4 m; vec4 v[10]; };\n"
	"uniform S s[10];\n"
	"uniform int i, j;\n"
	"void main() { gl_Position = s[i].m * s[i].v[j]; }\n"
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
	char name[256];
	bool pass = true;
	unsigned i;
	unsigned j;

	piglit_require_vertex_shader();
	vert = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_code);
	prog = piglit_link_simple_program(vert, 0);

	/* From page 80 of the OpenGL 2.1 spec:
	 *
	 *     "A valid name cannot be a structure, an array of structures, or
	 *     any portion of a single vector or a matrix."
	 */
	loc = piglit_GetUniformLocation(prog, "s");
	if (loc != -1) {
		printf("s location = %d (should be -1)\n", loc);
		pass = false;
	}

	for (i = 0; i < 10; i++) {
		/* From page 80 of the OpenGL 2.1 spec:
		 *
		 *     "A valid name cannot be a structure, an array of
		 *     structures, or any portion of a single vector or a
		 *     matrix."
		 */
		snprintf(name, sizeof(name), "s[%d]", i);
		loc = piglit_GetUniformLocation(prog, name);

		if (loc != -1) {
			printf("%s location = %d (should be -1)\n", name, loc);
			pass = false;
		}

		snprintf(name, sizeof(name), "s[%d].m", i);
		loc = piglit_GetUniformLocation(prog, name);

		if (loc == -1) {
			printf("%s location = %d (should not be -1)\n",
			       name, loc);
			pass = false;
		}

		for (j = 0; j < 10; j++) {
			snprintf(name, sizeof(name), "s[%d].v[%d]", i, j);
			loc = piglit_GetUniformLocation(prog, name);

			if (loc == -1) {
				printf("%s location = %d (should not be -1)\n",
				       name, loc);
				pass = false;
			}
		}
	}

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}
