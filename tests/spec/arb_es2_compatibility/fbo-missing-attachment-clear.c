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
 * \file fbo-missing-attachment-clear.c
 * Verify that a color clear on a depth-only FBO doesn't crash
 *
 * From the ARB_ES2_compatibility spec:
 *
 *     (8) How should we handle draw buffer completeness?
 *
 *     RESOLVED: Remove draw/readbuffer completeness checks, and treat
 *     drawbuffers referring to missing attachments as if they were NONE.
 *
 * It is valid to have a depth-only FBO that has the draw buffer set to values
 * other than \c GL_NONE.  However, doing operations that would read from or
 * draw to these missing attachments should treat them as though they were
 * \c GL_NONE (i.e., don't crash).
 *
 * See also https://bugs.freedesktop.org/show_bug.cgi?id=37739.
 *
 * \author Ian Romanick <ian.d.romanick@intel.com>
 */
#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    32 /*window_width*/,
    32 /*window_height*/,
    GLUT_RGB)

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint tex;
	GLuint fb;
	GLenum status;
	GLenum err;
	bool pass;

	piglit_require_extension("GL_ARB_framebuffer_object");
	piglit_require_extension("GL_ARB_ES2_compatibility");

	/* Create a depth-only FBO.
	 */
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 32, 32, 0,
		     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			       GL_TEXTURE_2D, tex, 0);

	err = glGetError();
	if (err != 0) {
		fprintf(stderr, "Unexpected GL error state 0x%04x\n", err);
		piglit_report_result(PIGLIT_FAIL);
	}

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "FBO erroneously incomplete: 0x%04x\n",
			status);
		piglit_report_result(PIGLIT_FAIL);
	}

	/* Clear the depth buffer to a known value.
	 */
	printf("Clearing depth buffer to 0.0...\n");
	glClearDepth(0.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	pass = piglit_probe_rect_depth(0, 0, piglit_width, piglit_height, 0.0);

	/* Clear the depth buffer and the color buffer to different values.
	 */
	printf("Clearing depth buffer to 0.5...\n");
	glClearDepth(0.5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pass = piglit_probe_rect_depth(0, 0, piglit_width, piglit_height, 0.5)
		&& pass;

	err = glGetError();
	if (err != 0) {
		fprintf(stderr, "Unexpected GL error state 0x%04x\n", err);
		piglit_report_result(PIGLIT_FAIL);
	}

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}
