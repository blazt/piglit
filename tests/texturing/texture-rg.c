/*
 * Copyright (c) 2010 VMware, Inc.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL VMWARE AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file
 * Tests GL_ARB_texture_rg extension (red/green texture formats)
 */

#include "piglit-util-gl-common.h"

int piglit_width = 128, piglit_height = 128;
int piglit_window_mode = GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE;

static const char *TestName = "texture-rg";

static GLint TexWidth = 128, TexHeight = 128;

struct format_info
{
   const char *Name;
   GLenum IntFormat, BaseFormat;
   GLboolean IsInteger;
   GLboolean IsFloat;
   GLboolean IsCompressed;
};


static const struct format_info IntFormats[] = {
   { "GL_R8", GL_R8, GL_RED, GL_FALSE, GL_FALSE, GL_FALSE },
   { "GL_R16", GL_R16, GL_RED, GL_FALSE, GL_FALSE, GL_FALSE },
   { "GL_RG8", GL_RG8, GL_RG, GL_FALSE, GL_FALSE, GL_FALSE },
   { "GL_RG16", GL_RG16, GL_RG, GL_FALSE, GL_FALSE, GL_FALSE },
   { "GL_R16F", GL_R16F, GL_RED, GL_FALSE, GL_TRUE, GL_FALSE },
   { "GL_R32F", GL_R32F, GL_RED, GL_FALSE, GL_TRUE, GL_FALSE },
   { "GL_RG16F", GL_RG16F, GL_RG, GL_FALSE, GL_TRUE, GL_FALSE },
   { "GL_RG32F", GL_RG32F, GL_RG, GL_FALSE, GL_TRUE, GL_FALSE },
   { "GL_R8I", GL_R8I, GL_R8, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_R8UI", GL_R8UI, GL_RED, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_R16I", GL_R16I, GL_RED, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_R16UI", GL_R16UI, GL_RED, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_R32I", GL_R32I, GL_RED, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_R32UI", GL_R32UI, GL_RED, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RG8I", GL_RG8I, GL_RG, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RG8UI", GL_RG8UI, GL_RG, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RG16I", GL_RG16I, GL_RG, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RG16UI", GL_RG16UI, GL_RG, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RG32I", GL_RG32I, GL_RG, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RG32UI", GL_RG32UI, GL_RG, GL_TRUE, GL_FALSE, GL_FALSE },
   { "GL_RED", GL_RED, GL_RED, GL_FALSE, GL_FALSE, GL_FALSE },
   { "GL_RG", GL_RG, GL_RG, GL_FALSE, GL_FALSE, GL_FALSE },
   { "GL_COMPRESSED_RED", GL_COMPRESSED_RED, GL_RED, GL_FALSE, GL_FALSE, GL_TRUE },
   { "GL_COMPRESSED_R", GL_COMPRESSED_RG, GL_RG, GL_FALSE, GL_FALSE, GL_TRUE }
};

#define NUM_INT_FORMATS  (sizeof(IntFormats) / sizeof(IntFormats[0]))


static GLboolean
check_error(const char *file, int line)
{
   GLenum err = glGetError();
   if (err) {
      fprintf(stderr, "%s: error 0x%x at %s:%d\n", TestName, err, file, line);
      return GL_TRUE;
   }
   return GL_FALSE;
}


static void
fill_texture_image(GLint w, GLint h, GLint comps, GLubyte *buf)
{
   GLint i, j, k;
   for (i = 0; i < h; i++) {
      for (j = 0; j < w; j++) {
         for (k = 0; k < comps; k++) {
            GLfloat val;
            if (k == 0) {
               /* left/right = red gradient */
               val = (int) (255 * j / (float) (w - 1));
            }
            else {
               /* up/down = green gradient */
               val = (int) (255 * i / (float) (h - 1));
            }
            *buf++ = val;
         }
      }
   }
}


static GLboolean
test_teximage_formats(void)
{
   GLint i;
   GLubyte *image;

   GLuint tex;

   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glEnable(GL_TEXTURE_2D);

   image = (GLubyte *) malloc(TexWidth * TexHeight * 2 * sizeof(GLubyte));

   for (i = 0; i < NUM_INT_FORMATS; i++) {
      const struct format_info *info = &IntFormats[i];
      const GLuint comps = (info->BaseFormat == GL_RED) ? 1 : 2;
      GLfloat expected[4], result[4];
      GLfloat error = 2.0 / 255.0; /* XXX fix */

      if (info->IsInteger) {
#if 0
         /* XXX we don't test integer texture formats yet */
	      if (!piglit_is_extension_supported("GL_EXT_texture_integer")) {
            continue;
         }
         else {
            /* test integer format */
         }
#else
         continue;
#endif
      }
      else if (info->IsFloat) {
	      if (!piglit_is_extension_supported("GL_ARB_texture_float")) {
            continue;
         }
      }

      if (info->IsCompressed) {
         /* use extra large error margin */
         error = 1.0 / 15.0;
      }

      fill_texture_image(TexWidth, TexHeight, comps, image);

      glTexImage2D(GL_TEXTURE_2D, 0, info->IntFormat,
                   TexWidth, TexHeight, 0,
                   info->BaseFormat, GL_UNSIGNED_BYTE, image);

      if (check_error(__FILE__, __LINE__)) {
         fprintf(stderr, "%s: Error in glTexImage2D for "
                 "internalFormat = %s\n", TestName, info->Name);
         return GL_FALSE;
      }

      if (0) {
         GLint f;
         glGetTexLevelParameteriv(GL_TEXTURE_2D, 0,
                                  GL_TEXTURE_INTERNAL_FORMAT, &f);
         assert(f == info->IntFormat);
      }

      glClear(GL_COLOR_BUFFER_BIT);
      glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);   glVertex2f(0, 0);
      glTexCoord2f(1, 0);   glVertex2f(TexWidth, 0);
      glTexCoord2f(1, 1);   glVertex2f(TexWidth, TexHeight);
      glTexCoord2f(0, 1);   glVertex2f(0, TexHeight);
      glEnd();

      /* setup expected polygon color */
      if (comps == 1) {
         expected[0] = 0.5;
         expected[1] = 0.0;
         expected[2] = 0.0;
         expected[3] = 1.0;
      }
      else {
         expected[0] = 0.5;
         expected[1] = 0.5;
         expected[2] = 0.0;
         expected[3] = 1.0;
      }

      /* test center pixel */
      result[0] = result[1] = result[2] = 0.0;
      result[3] = 1.0;
      glReadPixels(TexWidth/2, TexHeight/2, 1, 1, GL_RG, GL_FLOAT, result);

      if (check_error(__FILE__, __LINE__)) {
         fprintf(stderr, "%s: Error in glReadPixels(format = GL_RG)\n",
                 TestName);
         return GL_FALSE;
      }

      if (fabsf(result[0] - expected[0]) > error ||
          fabsf(result[1] - expected[1]) > error ||
          fabsf(result[2] - expected[2]) > error ||
          fabsf(result[3] - expected[3]) > error) {
         fprintf(stderr, "%s: failure with format %s:\n", TestName,
                 info->Name);
         fprintf(stderr, "  expected color = %g, %g, %g, %g\n",
                 expected[0], expected[1], expected[2], expected[3]);
         fprintf(stderr, "  result color = %g, %g, %g, %g\n",
                 result[0], result[1], result[2], result[3]);
         return GL_FALSE;
      }

      glutSwapBuffers();
   }

   free(image);

   glDisable(GL_TEXTURE_2D);

   return GL_TRUE;
}


static GLboolean
test_drawpixels_formats(void)
{
   GLubyte *image;
   GLfloat result[4], expected[4];
   GLfloat error = 2.0 / 255.0; /* XXX fix */

   image = (GLubyte *) malloc(TexWidth * TexHeight * 2 * sizeof(GLubyte));

   fill_texture_image(TexWidth, TexHeight, 2, image);

   glWindowPos2iARB(0, 0);
   glDrawPixels(TexWidth, TexHeight, GL_RG, GL_UNSIGNED_BYTE, image);

   if (check_error(__FILE__, __LINE__)) {
      fprintf(stderr, "%s: Error in glDrawPixels(format = GL_RG)\n",
              TestName);
      return GL_FALSE;
   }

   /* test center pixel */
   result[0] = result[1] = result[2] = 0.0;
   result[3] = 1.0;
   glReadPixels(TexWidth/2, TexHeight/2, 1, 1, GL_RG, GL_FLOAT, result);
   
   expected[0] = 0.5;
   expected[1] = 0.5;
   expected[2] = 0.0;
   expected[3] = 1.0;

   if (fabsf(result[0] - expected[0]) > error ||
       fabsf(result[1] - expected[1]) > error ||
       fabsf(result[2] - expected[2]) > error ||
       fabsf(result[3] - expected[3]) > error) {
      fprintf(stderr, "%s: glDrawPixels failure with format GL_RG:\n", TestName);
      fprintf(stderr, "  expected color = %g, %g, %g, %g\n",
              expected[0], expected[1], expected[2], expected[3]);
      fprintf(stderr, "  result color = %g, %g, %g, %g\n",
              result[0], result[1], result[2], result[3]);
      return GL_FALSE;
   }


   free(image);

   return GL_TRUE;
}


enum piglit_result
piglit_display(void)
{
   if (test_teximage_formats() && test_drawpixels_formats())
      return PIGLIT_PASS;
   else
      return PIGLIT_FAIL;
}


void
piglit_init(int argc, char **argv)
{
   piglit_require_extension("GL_ARB_texture_rg");

   piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);
}
