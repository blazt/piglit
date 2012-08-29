/*
 * Copyright (c) Blaž Tomažič
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
 * @file retain_release-context.c
 *
 * Test API functions:
 *
 *   cl_int clRetainContext (cl_context context)
 *   cl_int clReleaseContext (cl_context context)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clRetainContext and clReleaseContext";
	config.version_min = 10;

	config.run_per_platform = true;

PIGLIT_CL_API_TEST_CONFIG_END


enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	int ref_count = 0;
	const int max_ref_count = 10;
	cl_int errNo;
	cl_uint* ref_count_ptr;
	cl_context cl_ctx;

	cl_context_properties context_properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)env->platform_id,
		0
	};

	/*** Normal usage ***/

	cl_ctx = clCreateContextFromType(context_properties,
	                                 CL_DEVICE_TYPE_ALL,
	                                 NULL,
	                                 NULL,
	                                 &errNo);
	if(errNo == CL_DEVICE_NOT_FOUND) {
		fprintf(stderr, "No available devices.\n");
		return PIGLIT_SKIP;
	}
	if(!piglit_cl_check_error(errNo, CL_SUCCESS)){
		fprintf(stderr,
		        "Failed (error code: %s): Create context.\n",
		        piglit_cl_get_error_name(errNo));
		return PIGLIT_FAIL;
	}

	ref_count_ptr = piglit_cl_get_context_info(cl_ctx, CL_CONTEXT_REFERENCE_COUNT);
	if(*ref_count_ptr != 1) {
		free(ref_count_ptr);
		fprintf(stderr,
		        "CL_CONTEXT_REFERENCE_COUNT should be 1 after creating context.\n");
		return PIGLIT_FAIL;
	}
	free(ref_count_ptr);
	
	/* increase by two and decrease by one on each itreation */
	for(ref_count = 1; ref_count < max_ref_count; ref_count++) {
		errNo = clRetainContext(cl_ctx);
		if(!piglit_cl_check_error(errNo, CL_SUCCESS)) {
			fprintf(stderr,
			        "clRetainContext: Failed (error code: %s): Retain context.\n",
			        piglit_cl_get_error_name(errNo));
			return PIGLIT_FAIL;
		}
		errNo = clReleaseContext(cl_ctx);
		if(!piglit_cl_check_error(errNo, CL_SUCCESS)){
			fprintf(stderr,
			        "clReleaseContext: Failed (error code: %s): Release context.\n",
			        piglit_cl_get_error_name(errNo));
			return PIGLIT_FAIL;
		}
		errNo = clRetainContext(cl_ctx);
		if(!piglit_cl_check_error(errNo, CL_SUCCESS)){
			fprintf(stderr,
			        "clRetainContext: Failed (error code: %s): Retain context.\n",
			        piglit_cl_get_error_name(errNo));
			return PIGLIT_FAIL;
		}

		/* check internal value of reference count */
		ref_count_ptr = piglit_cl_get_context_info(cl_ctx, CL_CONTEXT_REFERENCE_COUNT);
		if(*ref_count_ptr != (ref_count+1)) {
			free(ref_count_ptr);
			fprintf(stderr,
			        "CL_CONTEXT_REFERENCE_COUNT is not changing accordingly.\n");
			return PIGLIT_FAIL;
		}
		free(ref_count_ptr);
	}
	/* Decrease reference count to 0 */
	for(ref_count = max_ref_count; ref_count > 0; ref_count--) {
		errNo = clReleaseContext(cl_ctx);
		if(!piglit_cl_check_error(errNo, CL_SUCCESS)){
			fprintf(stderr,
			        "clReleaseContext: Failed (error code: %s): Release context.\n",
			        piglit_cl_get_error_name(errNo));
			return PIGLIT_FAIL;
		}

		/* check internal value of reference count */
		if(ref_count > 1) {
			ref_count_ptr = piglit_cl_get_context_info(cl_ctx, CL_CONTEXT_REFERENCE_COUNT);
			if(*ref_count_ptr != (ref_count-1)) {
				free(ref_count_ptr);
				fprintf(stderr,
				        "CL_CONTEXT_REFERENCE_COUNT is not changing accordingly.\n");
				return PIGLIT_FAIL;
			}
			free(ref_count_ptr);
		}
	}

	/*** Errors ***/

	/*
	 * CL_INVALID_CONTEXT if context is not a valid OpenCL context.
	 */
	errNo = clReleaseContext(cl_ctx);
	if(!piglit_cl_check_error(errNo, CL_INVALID_CONTEXT)) {
			fprintf(stderr,
			        "clReleaseContext: Failed (error code: %s): Trigger CL_INVALID_CONTEXT if context is not a valid context (already released).\n",
			        piglit_cl_get_error_name(errNo));
		return PIGLIT_FAIL;
	}
	errNo = clReleaseContext(NULL);
	if(!piglit_cl_check_error(errNo, CL_INVALID_CONTEXT)) {
			fprintf(stderr,
			        "clReleaseContext: Failed (error code: %s): Trigger CL_INVALID_CONTEXT if context is not a valid context (NULL).\n",
			        piglit_cl_get_error_name(errNo));
		return PIGLIT_FAIL;
	}

	return PIGLIT_PASS;
}
