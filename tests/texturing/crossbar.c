/*
 * (C) Copyright IBM Corporation 2005
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
 * VA LINUX SYSTEM, IBM AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * \file crossbar.c
 *
 * Simple test of GL_ARB_texture_env_crossbar functionality.  Several squares
 * are drawn with different texture combine modes, but all should be rendered
 * with the same final color.
 *
 * \author Ian Romanick <idr@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "piglit-util-gl-common.h"

static const GLint tests[][8] = {
   { 1, GL_REPLACE,  GL_PRIMARY_COLOR, GL_PRIMARY_COLOR,
     2, GL_REPLACE,  GL_TEXTURE,       GL_PRIMARY_COLOR },
   { 3, GL_REPLACE,  GL_PRIMARY_COLOR, GL_PRIMARY_COLOR,
     2, GL_SUBTRACT, GL_TEXTURE0,      GL_TEXTURE1 },
   { 2, GL_REPLACE,  GL_PRIMARY_COLOR, GL_PRIMARY_COLOR,
     2, GL_REPLACE,  GL_TEXTURE0,      GL_TEXTURE0 },
   { 2, GL_REPLACE,  GL_PRIMARY_COLOR, GL_PRIMARY_COLOR,
     1, GL_SUBTRACT, GL_TEXTURE0,      GL_TEXTURE1 },
   { 3, GL_ADD,      GL_TEXTURE1,      GL_TEXTURE1,
     2, GL_MODULATE, GL_TEXTURE1,      GL_PREVIOUS },
   { 3, GL_ADD,      GL_TEXTURE1,      GL_TEXTURE1,
     4, GL_MODULATE, GL_TEXTURE0,      GL_PREVIOUS },
};

#define NUM_TESTS (sizeof(tests) / sizeof(tests[0]))

int piglit_width = 100 * (NUM_TESTS + 1);
int piglit_height = 100;
int piglit_window_mode = GLUT_RGB | GLUT_DOUBLE;

static void DoFrame( void )
{
   unsigned i;

   glClearColor(0.0, 0.0, 1.0, 0);
   glClear( GL_COLOR_BUFFER_BIT );

   glPushMatrix();

   /* This is the "reference" square.
    */

   glActiveTexture( GL_TEXTURE0 );
   glDisable( GL_TEXTURE_2D );
   glActiveTexture( GL_TEXTURE1 );
   glDisable( GL_TEXTURE_2D );

   glTranslatef(1.5, 0.0, 0.0);
   glBegin(GL_QUADS);
   glColor3f( 0.5, 0.5, 0.5 );
   glVertex2f(-1, -1);
   glVertex2f( 1, -1);
   glVertex2f( 1,  1);
   glVertex2f(-1,  1);
   glEnd();

   for ( i = 0 ; i < NUM_TESTS ; i++ ) {
      glActiveTexture( GL_TEXTURE0 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, tests[i][0] );
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
      glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, tests[i][1] );
      glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, tests[i][2] );
      glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, tests[i][3] );

      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, tests[i][4] );
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
      glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, tests[i][5] );
      glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, tests[i][6] );
      glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, tests[i][7] );

      glCallList(1);
   }

   glPopMatrix();

}

static int DoTest( void )
{
   int i;
   GLfloat probe[NUM_TESTS+1][4];
   GLfloat dr, dg, db;
   GLfloat dmax;

   glReadBuffer( GL_BACK );

   dmax = 0;
   for( i = 0; i <= NUM_TESTS; ++i ) {
      glReadPixels(piglit_width*(2*i+1)/((NUM_TESTS+1)*2), piglit_height/2, 1, 1, GL_RGBA, GL_FLOAT, probe[i]);
      printf("Probe %i: %f,%f,%f\n", i, probe[i][0], probe[i][1], probe[i][2]);
      dr = probe[i][0] - 0.5f;
      dg = probe[i][1] - 0.5f;
      db = probe[i][2] - 0.5f;
      printf("   Delta: %f,%f,%f\n", dr, dg, db);
      if (dr > dmax) dmax = dr;
      else if (-dr > dmax) dmax = -dr;
      if (dg > dmax) dmax = dg;
      else if (-dg > dmax) dmax = -dg;
      if (db > dmax) dmax = db;
      else if (-db > dmax) dmax = -db;
   }

   printf("Max delta: %f\n", dmax);

   if (dmax >= 0.07) // roughly 1/128
      return 0;
   else
      return 1;
}

enum piglit_result
piglit_display(void)
{
   if (piglit_automatic) {
      DoFrame();
      glutSwapBuffers();
      return PIGLIT_PASS;
   } else {
      int success, retry;

      printf("\nFirst frame\n-----------\n");
      DoFrame();
      success = DoTest();
      glutSwapBuffers();

      printf("\nSecond frame\n------------\n");
      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();
      glOrtho( 0, 3*(NUM_TESTS+1), -1.5, 1.5, -1, 1 );
      glMatrixMode( GL_MODELVIEW );
      glLoadIdentity();

      DoFrame();
      retry = DoTest();
      glutSwapBuffers();

      if (retry && success)
          return PIGLIT_PASS;
      else
          return PIGLIT_FAIL;
   }
}


static void Reshape( int width, int height )
{
   piglit_width = width;
   piglit_height = height;

   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( 0, 3*(NUM_TESTS+1), -1.5, 1.5, -1, 1 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
}

void piglit_init(int argc, char **argv)
{
   const char * const ver_string = (const char * const)
       glGetString( GL_VERSION );
   float ver = strtof( ver_string, NULL );
   GLint tex_units;
   GLint temp[ 256 ];

   glutReshapeFunc(Reshape);

   if ( (!piglit_is_extension_supported("GL_ARB_multitexture")
	 && (ver < 1.3))
	|| (!piglit_is_extension_supported("GL_ARB_texture_env_combine")
	    && !piglit_is_extension_supported("GL_EXT_texture_env_combine")
	    && (ver < 1.3))
	|| (!piglit_is_extension_supported("GL_ARB_texture_env_crossbar")
	    && !piglit_is_extension_supported("GL_NV_texture_env_combine4")
	    && (ver < 1.4)) ) {
      printf("\nSorry, this program requires GL_ARB_multitexture and either\n"
	     "GL_ARB_texture_env_combine or GL_EXT_texture_env_combine (or OpenGL 1.3).\n"
	     "Either GL_ARB_texture_env_crossbar or GL_NV_texture_env_combine4 (or\n"
	     "OpenGL 1.4) are also required.\n");
      if (!piglit_automatic)
         printf("PIGLIT: {'result': 'fail' }\n");
      exit(1);
   }

   glGetIntegerv( GL_MAX_TEXTURE_UNITS, & tex_units );
   if ( tex_units < 2 ) {
      printf("\nSorry, this program requires at least 2 texture units.\n");
      if (!piglit_automatic)
         printf("PIGLIT: {'result': 'fail' }\n");
      exit(1);
   }

   if (piglit_automatic)
      printf("\nAll %lu squares should be the same color.\n", (unsigned long) NUM_TESTS + 1);

   (void) memset( temp, 0x00, sizeof( temp ) );
   glBindTexture( GL_TEXTURE_2D, 1 );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, temp );

   (void) memset( temp, 0x7f, sizeof( temp ) );
   glBindTexture( GL_TEXTURE_2D, 2 );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, temp );

   (void) memset( temp, 0xff, sizeof( temp ) );
   glBindTexture( GL_TEXTURE_2D, 3 );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, temp );

   (void) memset( temp, 0x3f, sizeof( temp ) );
   glBindTexture( GL_TEXTURE_2D, 4 );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, temp );


   glNewList( 1, GL_COMPILE );
   glTranslatef(3.0, 0, 0);
   glBegin(GL_QUADS);
   glColor3f( 0.9, 0.0, 0.0 );
   glMultiTexCoord2f( GL_TEXTURE0, 0.5, 0.5 );
   glMultiTexCoord2f( GL_TEXTURE1, 0.5, 0.5 );
   glVertex2f(-1, -1);
   glVertex2f( 1, -1);
   glVertex2f( 1,  1);
   glVertex2f(-1,  1);
   glEnd();
   glEndList();
}
