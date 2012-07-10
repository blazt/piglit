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
 * @file get-device-info.c
 *
 * Test API function:
 *
 *   cl_int clGetDeviceInfo(cl_device_id device,
 *                          cl_device_info param_name,
 *                          size_t param_value_size,
 *                          void *param_value,
 *                          size_t *param_value_size_ret)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clGetDeviceInfo";
	config.version_min = 10;

	config.run_per_device = true;

PIGLIT_CL_API_TEST_CONFIG_END


enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	enum piglit_result result = PIGLIT_PASS;

	int i;

	size_t param_value_size;
	void* param_value;

	int num_device_infos = PIGLIT_CL_ENUM_NUM(cl_device_info, env->version);
	const cl_device_info *device_infos = PIGLIT_CL_ENUM_ARRAY(cl_device_info);

	/*** Normal usage ***/

	for(i = 0; i < num_device_infos; i++) {
		printf("%s ", piglit_cl_get_enum_name(device_infos[i]));

		if(!piglit_cl_check_error(clGetDeviceInfo(env->device_id,
		                                          device_infos[i],
		                                          0,
		                                          NULL,
		                                          &param_value_size),
		                          CL_SUCCESS)) {
			piglit_merge_result(&result, PIGLIT_FAIL);
		}

		param_value = malloc(param_value_size);
		if(!piglit_cl_check_error(clGetDeviceInfo(env->device_id,
		                                          device_infos[i],
		                                          param_value_size,
		                                          param_value,
		                                          NULL),
		                          CL_SUCCESS)) {
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
	if(!piglit_cl_check_error(clGetDeviceInfo(env->device_id,
	                                          CL_DEVICE_VERSION,
	                                          1,
	                                          param_value,
	                                          NULL),
	                          CL_INVALID_VALUE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}
	
	if(!piglit_cl_check_error(clGetDeviceInfo(env->device_id,
	                                          CL_PLATFORM_NAME,
	                                          0,
	                                          NULL,
	                                          &param_value_size),
	                          CL_INVALID_VALUE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	/*
	 * CL_INVALID_DEVICE if device is not a valid device.
	 */
	if(!piglit_cl_check_error(clGetDeviceInfo(NULL,
	                                          CL_DEVICE_NAME,
	                                          0,
	                                          NULL,
	                                          &param_value_size),
	                          CL_INVALID_DEVICE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}


	return result;
}
