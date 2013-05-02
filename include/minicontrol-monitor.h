/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MINICTRL_MONITOR_H_
#define _MINICTRL_MONITOR_H_

#include <minicontrol-error.h>
#include <minicontrol-type.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MINICONTROL_MONITOR_LIBRARY minicontrol monitor library
 * @brief This minicontrol monitor library used to manage events triggered by minicontrol provider
 */

/**
 * @ingroup MINICONTROL_MONITOR_LIBRARY
 * @defgroup MINICONTROL_MONITOR_LIBRARY_TYPE minicontrol monitor library type
 * @brief minicontrol monitor library type
 */

/**
 * @addtogroup MINICONTROL_MONITOR_LIBRARY_TYPE
 * @{
 */

  /**
 * @brief Called when event is triggered
 * @param[in] action The type of fired event
 * @param[in] name The name of provider
 * @param[in] width The width of provider
 * @param[in] height The height of provider
 * @param[in] priority The priority of provider
 * @param[in] data user data
 * @pre minicontrol_monitor_start() used to register this callback
 * @see #minicontrol_action_e
 * @see #minicontrol_priority_e
 */
typedef void (*minicontrol_monitor_cb) (minicontrol_action_e action,
					const char *name,
					unsigned int width,
					unsigned int height,
					minicontrol_priority_e priority,
					void *data);

/**
 * @addtogroup MINICONTROL_MONITOR_LIBRARY
 * @{
 */

/**
 * @brief Register a callback for events originated by minicontrol provider
 * @param[in] callback callback function
 * @param[in] data user data
 */
minicontrol_error_e minicontrol_monitor_start(minicontrol_monitor_cb callback,
					void *data);

/**
 * @brief Unregister a callback for events originated by minicontrol provider
 * @return #MINICONTROL_ERROR_NONE if success, other value if failure
 * @see #minicontrol_error_e
 */
minicontrol_error_e minicontrol_monitor_stop(void);

#ifdef __cplusplus
}
#endif
#endif /* _MINICTRL_MONITOR_H_ */

