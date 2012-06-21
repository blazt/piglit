/* © 2011 Intel Corporation
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

#include "piglit-util-gl-common.h"

/**
 * @file getactiveuniform-beginend.c
 *
 * Tests for a missing error condition in Mesa:
 *
 *     "Executing any other GL command between the execution of Begin
 *      and the corresponding execution of End results in the error
 *      INVALID OPERATION."
 */

PIGLIT_GL_TEST_MAIN(
    32 /*window_width*/,
    32 /*window_height*/,
    GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA)

enum piglit_result
piglit_display(void)
{
	/* Not reached */
	return PIGLIT_FAIL;
}

const char *vs_source =
	"uniform vec4 u;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = u;\n"
	"}\n";

void
piglit_init(int argc, char **argv)
{
	GLuint vs, prog;
	char name[4];
	GLint size, len;
	GLenum type;

	piglit_require_vertex_shader();
	vs = piglit_compile_shader_text(GL_VERTEX_SHADER, vs_source);

	prog = piglit_link_simple_program(vs, 0);
	glUseProgram(prog);
	if (!vs || !prog)
		piglit_report_result(PIGLIT_FAIL);

	glGetActiveUniformARB(prog, 0, sizeof(name), &len, &size, &type, name);

	if (!piglit_check_gl_error(0))
		piglit_report_result(PIGLIT_FAIL);

	glBegin(GL_TRIANGLES);
	glGetActiveUniformARB(prog, 0, sizeof(name), &len, &size, &type, name);
	glEnd();

	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);

	piglit_report_result(PIGLIT_PASS);
}
