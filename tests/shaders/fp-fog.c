/*
 * Copyright © 2008 Intel Corporation
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file
 * Test passing fog coordinates into a fragment program.
 *
 * \author Ian Romanick <ian.d.romanick@intel.com>
 */

#include "piglit-util-gl-common.h"

static GLint prog = 0;

static const char* const program_text =
	"!!ARBfp1.0\n"
	"MOV result.color, fragment.fogcoord;\n"
	"END\n"
	;

int piglit_width = 50, piglit_height = 50;
int piglit_window_mode = GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE;

static PFNGLFOGCOORDFPROC pglFogCoordf = NULL;

enum piglit_result
piglit_display(void)
{
	static const struct {
		float x, y, r;
	}
	probes[4] = {
		{ 0.5, 1.5, 0.3 },
		{ 1.5, 1.5, 0.6 },
		{ 0.5, 0.5, 0.8 },
		{ 1.5, 0.5, 0.4 },
	};
	int pass = 1;
	unsigned i;

	glClear(GL_COLOR_BUFFER_BIT);

	pglFogCoordf(0.3);
	glBegin(GL_QUADS);
	glVertex2f(0, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 2);
	glVertex2f(0, 2);
	glEnd();

	pglFogCoordf(0.6);
	glBegin(GL_QUADS);
	glVertex2f(1, 1);
	glVertex2f(2, 1);
	glVertex2f(2, 2);
	glVertex2f(1, 2);
	glEnd();

	pglFogCoordf(0.8);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glVertex2f(1, 1);
	glVertex2f(0, 1);
	glEnd();

	pglFogCoordf(0.4);
	glBegin(GL_QUADS);
	glVertex2f(1, 0);
	glVertex2f(2, 0);
	glVertex2f(2, 1);
	glVertex2f(1, 1);
	glEnd();

	for (i = 0; i < 4; i++) {
		float expected_color[4];

		expected_color[0] = probes[i].r;
		expected_color[1] = 0.0;
		expected_color[2] = 0.0;
		expected_color[3] = 1.0;

		pass &= piglit_probe_pixel_rgba(probes[i].x * piglit_width / 2,
						probes[i].y * piglit_height / 2,
						expected_color);
	}

	glutSwapBuffers();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}


static void Reshape(int width, int height)
{
	piglit_width = width;
	piglit_height = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 2.0, 0.0, 2.0, -2.0, 6.0);
	glScalef(1.0, 1.0, -1.0); // flip z-axis
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void
piglit_init(int argc, char **argv)
{
	printf("GL_RENDERER = %s\n", (char *) glGetString(GL_RENDERER));

	glutReshapeFunc(Reshape);

	glClearColor(0.3, 0.3, 0.3, 0.3);

	if (piglit_get_gl_version() >= 14) {
		pglFogCoordf = glFogCoordf;
	} else if (piglit_is_extension_supported("GL_EXT_fog_coord")) {
		pglFogCoordf = glFogCoordfEXT;
	} else {
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_fragment_program();
	prog = piglit_compile_program(GL_FRAGMENT_PROGRAM_ARB, program_text);

	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, prog);

	glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

	Reshape(piglit_width, piglit_height);
}
