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

#ifndef _MINICTRL_VIEWER_H_
#define _MINICTRL_VIEWER_H_

#include <Evas.h>
#include <bundle.h>
#include "minicontrol-error.h"
#include "minicontrol-type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file minicontrol-viewer.h
 * @brief This minicontrol viewer library used to display minicontrol which created by the minicontrol provider.\n
 *         This library is providing functions for attach a minicontrol viewer to a parent evas object and sending some requests to a minicontrol provider.\n
 *         The minicontrol viewer places a space which will be used by the minicontrol provider to draw minicontrol.
 */

/**
 * @addtogroup MINICONTROL_VIEWER_MODULE
 * @{
 */

/**
 * @brief Adds minicontrol named as "minicontrol_name" to a given parent evas object and returns it.
 * @since_tizen 2.4
 * @param[in] parent Minicontrol object will be added to this parent evas object
 * @param[in] minicontrol_name Name of minicontrol
 * @return Evas object of minicontrol.  @c NULL failed to add, get_last_result() will returns reason of failure.
 */
Evas_Object *minicontrol_viewer_add(Evas_Object *parent, const char *minicontrol_name);

/**
 * @brief Sends a event to the provider.
 * @since_tizen 2.4
 * @param[in] minicontrol_name The name of the minicontrol window
 * @param[in] event Type of the event
 * @param[in] event_arg A bundle of arguments
 * @return #MINICONTROL_ERROR_NONE on success,
 *          otherwise an error code (see #MINICONTROL_ERROR_XXX) on failure
 * @retval #MINICONTROL_ERROR_INVALID_PARAMETER  Invalid argument
 * @see #minicontrol_viewer_event_e
 */
int minicontrol_viewer_send_event(const char *minicontrol_name, minicontrol_viewer_event_e event, bundle *event_arg);

/**
 * @brief Called when a event comes from the provider
 * @since_tizen 2.4
 * @param[in] event The type of fired event
 * @param[in] minicontrol_name The name of the minicontrol window
 * @param[in] event_arg A bundle of arguments
 * @param[in] data User data
 * @see #minicontrol_viewer_set_event_cb
 */
typedef void (*minicontrol_viewer_event_cb) (minicontrol_event_e event, const char *minicontrol_name, bundle *event_arg, void *data);

/**
 * @brief Registers a callback for events originated by minicontrol provider.
 * @since_tizen 2.4
 * @param[in] callback Callback function
 * @param[in] user_data User data
 * @return #MINICONTROL_ERROR_NONE on success,
 *         otherwise an error code (see #MINICONTROL_ERROR_XXX) on failure
 * @retval #MINICONTROL_ERROR_INVALID_PARAMETER  Invalid argument
 * @retval #MINICONTROL_ERROR_IPC_FAILURE IPC failure
 * @retval #MINICONTROL_ERROR_OUT_OF_MEMORY out of memory
 * @see #minicontrol_viewer_unset_event_cb
 * @see #minicontrol_viewer_event_cb
 */
int minicontrol_viewer_set_event_cb(minicontrol_viewer_event_cb callback, void *user_data);

/**
 * @brief Unregisters a callback for events originated by minicontrol provider.
 * @since_tizen 2.4
 * @return #MINICONTROL_ERROR_NONE if success, other value if failure
 * @see #minicontrol_viewer_set_event_cb
 */
int minicontrol_viewer_unset_event_cb(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* _MINICTRL_VIEWER_H_ */

