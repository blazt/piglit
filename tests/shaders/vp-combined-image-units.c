/*
 * Copyright © 2009 Intel Corporation
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
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

/** @file vp-combined-image-units.c
 *
 * Tests that ARB_shader_program returns a not-impossible result for
 * GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS.  Catches a regression in wine caused
 * by bug #25518.
 */

#include "piglit-util-gl-common.h"

int piglit_width = 100, piglit_height = 100;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE;

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	GLint combined, fragment, vertex;

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combined);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vertex);

	if (combined < vertex) {
		fprintf(stderr,
			"Max vertex units (%d) > combined units (%d)\n",
			vertex, combined);
		pass = GL_FALSE;
	}

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &fragment);
	if (combined < fragment) {
		fprintf(stderr,
			"Max fragment units (%d) > "
			"combined units (%d)\n",
			fragment, combined);
		pass = GL_FALSE;
	}

	pass = pass && glGetError() == 0;

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);

	return PIGLIT_FAIL; /* NOTREACHED */
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_extension("GL_ARB_vertex_shader");
}

