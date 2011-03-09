/*
 * Copyright © 2010 Intel Corporation
 * Copyright © 2010 Marek Olšák <maraeo@gmail.com>
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
 *    Marek Olšák <maraeo@gmail.com>
 *
 */

#include "piglit-util.h"
#include "fbo-formats.h"

int piglit_width = 128;
int piglit_height = 64;
int piglit_window_mode = GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE;

static const struct test_desc *test_set;
static int test_index;
static int format_index;

static void blend(const float *rect, const float *src, const float *dst, const float *blendcol,
                  GLenum blendsrc, GLenum blenddst)
{
	glColor4fv(dst);
	piglit_draw_rect(rect[0], rect[1], rect[2], rect[3]);
	glEnable(GL_BLEND);
	glBlendFunc(blendsrc, blenddst);
	if (blendcol)
		glBlendColor(blendcol[0], blendcol[1], blendcol[2], blendcol[3]);
	glColor4fv(src);
	piglit_draw_rect(rect[0], rect[1], rect[2], rect[3]);
	glDisable(GL_BLEND);
}

static enum piglit_result test_format(const struct format_desc *format, GLenum baseformat)
{
	GLboolean pass = GL_TRUE;
	GLuint tex, fb;
	GLenum status;
	int r, g, b, l, a, i;

	float res0[] = {0.3, 0.3, 0.3, 0.0};

	float pos1[] = {-0.66, -1.0, 0.33, 2.0};
        float src1[] = {0.4, 0.9, 0.8, 0.7};
        float dst1[] = {0.5, 0.4, 0.6, 0.2};
        float con1[] = {0.2, 0.8, 0.4, 0.6};
	float res1[] = {dst1[0]*(1-con1[0]) + src1[0]*con1[0],
                        dst1[1]*(1-con1[1]) + src1[1]*con1[1],
                        dst1[2]*(1-con1[2]) + src1[2]*con1[2],
                        dst1[3]*(1-con1[3]) + src1[3]*con1[3]};

	float pos2[] = {-0.33, -1.0, 0.33, 2.0};
	float dst2[] = {0.9, 0.4, 0.7, 0.5};
        float src2[] = {0.8, 0.3, 0.5, 0.9};
	float res2[] = {dst2[0]*(1-dst2[0]) + src2[0]*dst2[0],
			dst2[1]*(1-dst2[1]) + src2[1]*dst2[1],
			dst2[2]*(1-dst2[2]) + src2[2]*dst2[2],
			dst2[3]*(1-dst2[3]) + src2[3]*dst2[3]};

	float pos3[] = {0.0, -1.0, 0.33, 2.0};
	float dst3[] = {0.6, 0.4, 0.8, 0.5};
	float src3[] = {0.8, 0.9, 0.7, 0.8};
	float res3[] = {dst3[0]*(1-src3[0]) + src3[0]*src3[0],
			dst3[1]*(1-src3[1]) + src3[1]*src3[1],
			dst3[2]*(1-src3[2]) + src3[2]*src3[2],
			dst3[3]*(1-src3[3]) + src3[3]*src3[3]};

	float pos4[] = {0.33, -1.0, 0.33, 2.0};
	float dst4[] = {0.9, 0.4, 0.7, 0.5};
	float src4[] = {0.8, 0.3, 0.5, 0.9};
	float res4[] = {dst4[0]*(1-dst4[3]) + src4[0]*dst4[3],
			dst4[1]*(1-dst4[3]) + src4[1]*dst4[3],
			dst4[2]*(1-dst4[3]) + src4[2]*dst4[3],
			dst4[3]*(1-dst4[3]) + src4[3]*dst4[3]};

	float pos5[] = {0.66, -1.0, 0.33, 2.0};
	float dst5[] = {0.6, 0.4, 0.8, 0.5};
	float src5[] = {0.8, 0.9, 0.7, 0.8};
	float res5[] = {dst5[0]*(1-src5[3]) + src5[0]*src5[3],
			dst5[1]*(1-src5[3]) + src5[1]*src5[3],
			dst5[2]*(1-src5[3]) + src5[2]*src5[3],
			dst5[3]*(1-src5[3]) + src5[3]*src5[3]};

        if (baseformat == GL_DEPTH_COMPONENT ||
            baseformat == GL_DEPTH_STENCIL)
		return PIGLIT_SKIP;

	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	glViewport(0, 0, piglit_width, piglit_height);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, format->internalformat,
		     piglit_width, piglit_height, 0,
		     GL_RGBA, GL_FLOAT, NULL);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
				 GL_TEXTURE_LUMINANCE_SIZE, &l);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
				 GL_TEXTURE_ALPHA_SIZE, &a);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
				 GL_TEXTURE_INTENSITY_SIZE, &i);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
				 GL_TEXTURE_RED_SIZE, &r);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
				 GL_TEXTURE_GREEN_SIZE, &g);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
				 GL_TEXTURE_BLUE_SIZE, &b);

        if (i) {
		res0[3] = res0[2] = res0[1] = res0[0];
		res1[3] = res1[2] = res1[1] = res1[0];
		res2[3] = res2[2] = res2[1] = res2[0];
		res3[3] = res3[2] = res3[1] = res3[0];
		res4[3] = res4[2] = res4[1] = res4[0];
		res5[3] = res5[2] = res5[1] = res5[0];
	} else if (l) {
		res0[2] = res0[1] = res0[0];
		res1[2] = res1[1] = res1[0];
		res2[2] = res2[1] = res2[0];
		res3[2] = res3[1] = res3[0];
		res4[2] = res4[1] = res4[0];
		res5[2] = res5[1] = res5[0];
		if (!a) {
			res0[3] = 1;
			res1[3] = 1;
			res2[3] = 1;
			res3[3] = 1;
			res4[3] = 1;
			res5[3] = 1;
		}
        } else {
		if (!r) {
			res0[0] = 0;
			res1[0] = 0;
			res2[0] = 0;
			res3[0] = 0;
			res4[0] = 0;
			res5[0] = 0;
		}
		if (!g) {
			res0[1] = 0;
			res1[1] = 0;
			res2[1] = 0;
			res3[1] = 0;
			res4[1] = 0;
			res5[1] = 0;
		}
		if (!b) {
			res0[2] = 0;
			res1[2] = 0;
			res2[2] = 0;
			res3[2] = 0;
			res4[2] = 0;
			res5[2] = 0;
		}
		if (!a) {
			res0[3] = 1;
			res1[3] = 1;
			res2[3] = 1;
			res3[3] = 1;
			res4[3] = 1;
			res5[3] = 1;
		}
        }

	/* Clamp the bits for the framebuffer, except we aren't checking
	 * the actual framebuffer bits.
	 */
	if (l > 8)
		l = 8;
	if (i > 8)
		i = 8;
	if (r > 8)
		r = 8;
	if (g > 8)
		g = 8;
	if (b > 8)
		b = 8;
	if (a > 8)
		a = 8;

        if (i) {
		piglit_set_tolerance_for_bits(i, i, i, i);
        } else if (l) {
		piglit_set_tolerance_for_bits(l, l, l, a);
        } else {
		piglit_set_tolerance_for_bits(r, g, b, a);
        }

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				  GL_COLOR_ATTACHMENT0_EXT,
				  GL_TEXTURE_2D,
				  tex,
				  0);
	assert(glGetError() == 0);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	printf("Testing %s", format->name);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		printf(" - fbo incomplete (status = 0x%04x)\n", status);
		return PIGLIT_SKIP;
	}
        printf("\n");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4fv(res0);
	piglit_draw_rect(-1.0, -1.0, 0.33, 2.0);

        blend(pos1, src1, dst1, con1, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
	blend(pos2, src2, dst2, NULL, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR);
	blend(pos3, src3, dst3, NULL, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	blend(pos4, src4, dst4, NULL, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	blend(pos5, src5, dst5, NULL, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!piglit_probe_pixel_rgba(piglit_width * 1 / 12, 0, res0)) {
		printf("  when testing FBO result, simple.\n");
		pass = GL_FALSE;
        }
	if (!piglit_probe_pixel_rgba(piglit_width * 3 / 12, 0, res1)) {
		printf("  when testing FBO result, blending with CONSTANT_COLOR.\n");
		pass = GL_FALSE;
        }
	if (!piglit_probe_pixel_rgba(piglit_width * 5 / 12, 0, res2)) {
		printf("  when testing FBO result, blending with DST_COLOR.\n");
		pass = GL_FALSE;
        }
	if (!piglit_probe_pixel_rgba(piglit_width * 7 / 12, 0, res3)) {
		printf("  when testing FBO result, blending with SRC_COLOR.\n");
		pass = GL_FALSE;
        }
	if ((i || a) && !piglit_probe_pixel_rgba(piglit_width * 9 / 12, 0, res4)) {
		printf("  when testing FBO result, blending with DST_ALPHA.\n");
		pass = GL_FALSE;
	}
	if (!piglit_probe_pixel_rgba(piglit_width * 11 / 12, 0, res5)) {
		printf("  when testing FBO result, blending with SRC_ALPHA.\n");
		pass = GL_FALSE;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, piglit_width, piglit_height);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,   GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);

	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	piglit_draw_rect_tex(-1, -1, 2, 2,
			     0, 0, 1, 1);

	glDisable(GL_TEXTURE_2D);
	glDeleteTextures(1, &tex);
	glDeleteFramebuffersEXT(1, &fb);

	if (!piglit_probe_pixel_rgba(piglit_width * 1 / 12, 0, res0)) {
		printf("  when testing window result, simple.\n");
		pass = GL_FALSE;
        }
	if (!piglit_probe_pixel_rgba(piglit_width * 3 / 12, 0, res1)) {
		printf("  when testing window result, blending with CONSTANT_COLOR.\n");
		pass = GL_FALSE;
        }
	if (!piglit_probe_pixel_rgba(piglit_width * 5 / 12, 0, res2)) {
		printf("  when testing window result, blending DST_COLOR.\n");
		pass = GL_FALSE;
        }
	if (!piglit_probe_pixel_rgba(piglit_width * 7 / 12, 0, res3)) {
		printf("  when testing window result, blending SRC_COLOR.\n");
		pass = GL_FALSE;
        }
	if ((i || a) && !piglit_probe_pixel_rgba(piglit_width * 9 / 12, 0, res4)) {
		printf("  when testing window result, blending DST_ALPHA.\n");
		pass = GL_FALSE;
	}
	if (!piglit_probe_pixel_rgba(piglit_width * 11 / 12, 0, res5)) {
		printf("  when testing window result, blending SRC_ALPHA.\n");
		pass = GL_FALSE;
	}

	glutSwapBuffers();

	return pass ? PIGLIT_SUCCESS : PIGLIT_FAILURE;
}

static void add_result(bool *all_skip, enum piglit_result *end_result,
		       enum piglit_result new)
{
	if (new != PIGLIT_SKIP)
		*all_skip = false;

	if (new == PIGLIT_FAILURE)
		*end_result = new;
}

enum piglit_result piglit_display(void)
{
	enum piglit_result result, end_result = PIGLIT_SUCCESS;
	bool all_skip = true;
	int i;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);

	if (piglit_automatic) {
		for (i = 0; i < test_set->num_formats; i++) {
			result = test_format(&test_set->format[i],
					     test_set->base);
			add_result(&all_skip, &end_result, result);
		}
	} else {
		result = test_format(&test_sets[test_index].format[format_index],
				     test_sets[test_index].base);
		add_result(&all_skip, &end_result, result);
	}

	glutSwapBuffers();

	if (all_skip)
		return PIGLIT_SKIP;
	return end_result;
}

static void key_func(unsigned char key, int x, int y)
{
	switch (key) {
	case 'n': /* next test set */
		do {
			test_index++;
			if (test_index >= ARRAY_SIZE(test_sets)) {
				test_index = 0;
			}
		} while (!supported(&test_sets[test_index]));
		format_index = 0;
		printf("Using test set: %s\n", test_sets[test_index].param);
		break;

	case 'N': /* previous test set */
		do {
			test_index--;
			if (test_index < 0) {
				test_index = ARRAY_SIZE(test_sets) - 1;
			}
		} while (!supported(&test_sets[test_index]));
		format_index = 0;
		printf("Using test set: %s\n", test_sets[test_index].param);
		break;

	case 'm': /* next format */
		format_index++;
		if (format_index >= test_sets[test_index].num_formats) {
			format_index = 0;
		}
		break;

	case 'M': /* previous format */
		format_index--;
		if (format_index < 0) {
			format_index = test_sets[test_index].num_formats - 1;
		}
		break;
	}

	piglit_escape_exit_key(key, x, y);
}

void piglit_init(int argc, char **argv)
{
	int i, j, k;

	glutKeyboardFunc(key_func);

	piglit_require_extension("GL_EXT_framebuffer_object");
	piglit_require_extension("GL_ARB_texture_env_combine");

	test_set = &test_sets[0];

	for (i = 1; i < argc; i++) {
		for (j = 1; j < ARRAY_SIZE(test_sets); j++) {
			if (!strcmp(argv[i], test_sets[j].param)) {
				for (k = 0; k < 3; k++) {
					if (test_sets[j].ext[k]) {
						piglit_require_extension(test_sets[j].ext[k]);
					}
				}

				test_set = &test_sets[j];
				break;
			}
		}
		if (j == ARRAY_SIZE(test_sets)) {
			fprintf(stderr, "Unknown argument: %s\n", argv[i]);
			exit(1);
		}
	}

	if (!piglit_automatic) {
		printf("    -n   Next test set.\n"
		       "    -N   Previous test set.\n"
		       "    -m   Next format in the set.\n"
		       "    -M   Previous format in the set.\n");
	}

	printf("Using test set: %s\n", test_set->param);
}