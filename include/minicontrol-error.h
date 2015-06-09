/*
 * Copyright (c)  2013-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MINICTRL_ERROR_H_
#define _MINICTRL_ERROR_H_

#include <tizen_error.h>

/**
 * @file minicontrol-error.h
 * @brief Minicontrol library error type.
 */

/**
 * @addtogroup MINICONTROL_LIBRARY
 * @{
 */

/**
 * @brief Enumeration for describing error code of minicontrol library.
 * @since_tizen 2.4
 */
typedef enum _minicontrol_error {
	MINICONTROL_ERROR_NONE = TIZEN_ERROR_NONE,  /**< MiniControl error none */
	MINICONTROL_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid parameter */
	MINICONTROL_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,  /**< Out of memory */
	MINICONTROL_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permission denied */
	MINICONTROL_ERROR_IPC_FAILURE = TIZEN_ERROR_MINICONTROL | 0x02,  /**< IPC error */
	MINICONTROL_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED, /**< This function is not supported */
	MINICONTROL_ERROR_ELM_FAILURE = TIZEN_ERROR_MINICONTROL | 0x03,  /**< Some error occurred when creating a minicontrol window */
	MINICONTROL_ERROR_UNKNOWN = TIZEN_ERROR_UNKNOWN,  /**< Unknown error */
} minicontrol_error_e;

/**
 * @}
 */

#endif /* _MINICTRL_ERROR_H_ */


