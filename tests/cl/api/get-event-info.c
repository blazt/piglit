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
 * @file get-event-info.c
 *
 * Test API function:
 *
 *   cl_int clGetEventInfo (cl_event event,
 *                          cl_event_info param_name,
 *                          size_t param_value_size,
 *                          void *param_value,
 *                          size_t *param_value_size_ret)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clGetEventInfo";
	config.version_min = 10;

	config.run_per_device = true;
	config.create_context = true;

PIGLIT_CL_API_TEST_CONFIG_END


enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	enum piglit_result result = PIGLIT_PASS;

	int i;
	cl_int errNo;
	cl_event event;
	cl_mem memobj;
	unsigned char buffer[1];

	size_t param_value_size;
	void* param_value;
	
	int num_event_infos = PIGLIT_CL_ENUM_NUM(cl_event_info, env->version);
	const cl_event_info* event_infos = PIGLIT_CL_ENUM_ARRAY(cl_event_info);

	memobj = clCreateBuffer(env->context->cl_ctx,
	                        CL_MEM_READ_WRITE,
	                        512,
	                        NULL,
	                        NULL);

	errNo = clEnqueueReadBuffer(env->context->command_queues[0], memobj, true, 0, 1, buffer, 0, NULL, &event);
	if(!piglit_cl_check_error(errNo, CL_SUCCESS)){
		fprintf(stderr,
		        "Failed (error code: %s): Create an event by enqueueing a buffer read.\n",
		        piglit_cl_get_error_name(errNo));
		return PIGLIT_FAIL;
	}

	/*** Normal usage ***/
	for(i = 0; i < num_event_infos; i++) {
		printf("%s ", piglit_cl_get_enum_name(event_infos[i]));

		errNo = clGetEventInfo(event,
		                       event_infos[i],
		                       0,
		                       NULL,
		                       &param_value_size);
		if(!piglit_cl_check_error(errNo, CL_SUCCESS)) {
			fprintf(stderr,
			        "Failed (error code: %s): Get size of %s.\n",
			        piglit_cl_get_error_name(errNo),
			        piglit_cl_get_enum_name(event_infos[i]));
			piglit_merge_result(&result, PIGLIT_FAIL);
			continue;
		}

		param_value = malloc(param_value_size);
		errNo = clGetEventInfo(event,
		                       event_infos[i],
		                       param_value_size,
		                       param_value,
		                       NULL);
		if(!piglit_cl_check_error(errNo, CL_SUCCESS)) {
			fprintf(stderr,
			        "Failed (error code: %s): Get value of %s.\n",
			        piglit_cl_get_error_name(errNo),
			        piglit_cl_get_enum_name(event_infos[i]));
			piglit_merge_result(&result, PIGLIT_FAIL);
		}

		//TODO: output returned values
		printf("\n");
		free(param_value);
	}

	/*** Errors ***/

	/*
	 * CL_INVALID_VALUE if param_name is not one of the supported
	 * values or if size in bytes specified by param_value_size is
	 * less than size of return type and param_value is not a NULL
	 * value.
	 */
	errNo = clGetEventInfo(event,
	                       CL_DEVICE_NAME,
	                       0,
	                       NULL,
	                       &param_value_size);
	if(!piglit_cl_check_error(errNo, CL_INVALID_VALUE)) {
		fprintf(stderr,
		        "Failed (error code: %s): Trigger CL_INVALID_VALUE if param_name is not one of the supported values.\n",
		        piglit_cl_get_error_name(errNo));
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	errNo = clGetEventInfo(event,
	                       CL_EVENT_COMMAND_QUEUE,
	                       1,
	                       param_value,
	                       NULL);
	if(!piglit_cl_check_error(errNo, CL_INVALID_VALUE)) {
		fprintf(stderr,
		        "Failed (error code: %s): Trigger CL_INVALID_VALUE if size in bytes specified by param_value is less than size of return type and param_value is not a NULL value.\n",
		        piglit_cl_get_error_name(errNo));
		piglit_merge_result(&result, PIGLIT_FAIL);
	}
	
	/*
	 * CL_INVALID_EVENT if event is not a valid event object.
	 */
	errNo = clGetEventInfo(NULL,
	                       CL_EVENT_COMMAND_QUEUE,
	                       0,
	                       NULL,
	                       &param_value_size);
	if(!piglit_cl_check_error(errNo, CL_INVALID_EVENT)) {
		fprintf(stderr,
		        "Failed (error code: %s): Trigger CL_INVALID_EVENT if event is not a valid event object.\n",
		        piglit_cl_get_error_name(errNo));
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	clReleaseEvent(event);
	clReleaseMemObject(memobj);

	return result;
}
