/*
 * (C) Copyright IBM Corporation 2006
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * IBM AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file vao-01.c
 *
 * Simple test of APPLE_vertex_array_object functionality.  This test creates
 * a VAO, pushed it (via \c glPushClientAttrib), modifies the VAO, then pops
 * it (via \c glPopClientAttrib).  After popping, the state of the VAO is
 * examined.
 * 
 * According to the APPLE_vertex_array_object spec, the contents of the VAO
 * should be restored to the values that they had when pushed.
 * 
 * \author Ian Romanick <idr@us.ibm.com>
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    400 /*window_width*/,
    200 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE)

enum piglit_result
piglit_display(void)
{
	return PIGLIT_PASS;
}

void
piglit_init(int argc, char **argv)
{
	GLuint obj;
	void * ptr;

	piglit_require_extension("GL_APPLE_vertex_array_object");

	glGenVertexArraysAPPLE(1, & obj);
	glBindVertexArrayAPPLE(obj);
	glVertexPointer(4, GL_FLOAT, sizeof(GLfloat) * 4, (void *) 0xDEADBEEF);
	glEnableClientState(GL_VERTEX_ARRAY);

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	glVertexPointer(4, GL_FLOAT, sizeof(GLfloat) * 4, (void *) 0xBADDC0DE);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopClientAttrib();

	if (! glIsEnabled(GL_VERTEX_ARRAY)) {
		printf("Array state is incorrectly disabled.\n");
		piglit_report_result(PIGLIT_FAIL);
	}

	glGetPointerv(GL_VERTEX_ARRAY_POINTER, & ptr);
	if (ptr != (void *) 0xDEADBEEF) {
		printf("Array pointer is incorrectly set to 0x%p.\n", ptr);
		piglit_report_result(PIGLIT_FAIL);
	}

	piglit_report_result(PIGLIT_PASS);
}
