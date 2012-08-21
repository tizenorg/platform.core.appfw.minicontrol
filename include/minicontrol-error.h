/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MINICTRL_ERROR_H_
#define _MINICTRL_ERROR_H_

/**
 * @defgroup MINICONTROL_LIBRARY_ERROR_TYPE minicontrol monitor error type
 * @brief minicontrol library error type
 */

/**
 * @addtogroup MINICONTROL_LIBRARY_ERROR_TYPE
 * @{
 */

/**
 * @breief Enumeration describing error code of minicontrol library
 */
typedef enum _minicontrol_error {
	MINICONTROL_ERROR_NONE = 0,
	MINICONTROL_ERROR_INVALID_PARAMETER = -1,
	MINICONTROL_ERROR_OUT_OF_MEMORY = -2,
	MINICONTROL_ERROR_DBUS = -3,
	MINICONTROL_ERROR_UNKNOWN = -100,
}minicontrol_error_e;

#endif /* _MINICTRL_ERROR_H_ */


