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

#ifndef _MINICTRL_PROVIDER_H_
#define _MINICTRL_PROVIDER_H_

#include <Evas.h>
#include <bundle.h>
#include "minicontrol-error.h"
#include "minicontrol-type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file minicontrol-provider.h
 * @brief This minicontrol provider library used to create evas socket window.\n
 *       This library is providing functions for create a remote evas object to draw a minicontrol on a minicontrol viewer and\n
 *       sending some requests to the minicontrol viewer.\n
 *        Drawings on this remote evas object will be shown on the place ofthe  minicontrol viewer.
 */

/**
 * @addtogroup MINICONTROL_PROVIDER_MODULE
 * @{
 */

/**
 * @brief Called when a event comes from viewer
 * @since_tizen 2.4
 * @param[in] event_type The type of fired event
 * @param[in] event_arg argument of the event
 * @pre minicontrol_viewer_register_event_callback() used to register this callback.
 * @see #minicontrol_create_window
 * @see #minicontrol_viewer_event_e
 */
typedef void (*minicontrol_event_cb) (minicontrol_viewer_event_e event_type, bundle *event_arg);

/**
 * @brief Creates a window for minicontrol.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/minicontrol.provider
 * @param[in] name Name of minicontrol socket window
 * @param[in] target_viewer Target viewer for minicontrol. You can select multiple viewers by using bitwise OR operator.
 * @param[in] callback a callback function for events originated by minicontrol viewer.
 * @return Evas object of socket window. @c NULL failed to create, get_last_result() will returns reason of failure.
 * @see #minicontrol_target_viewer_e
 * @see #minicontrol_event_cb
 */
Evas_Object* minicontrol_create_window(const char *name, minicontrol_target_viewer_e target_viewer, minicontrol_event_cb callback);

/**
 * @brief Sends a event to the viewer.
 * @remarks When a viewer doesn't handle some events, it can be ignored.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/minicontrol.provider
 * @param[in] minicontrol minicontrol window
 * @param[in] event Type of the event
 * @param[in] event_arg Bundle argument of the event
 * @return #MINICONTROL_ERROR_NONE on success,
 *         otherwise an error code (see #MINICONTROL_ERROR_XXX) on failure
 * @retval #MINICONTROL_ERROR_INVALID_PARAMETER Invalid argument
 * @retval #MINICONTROL_ERROR_PERMISSION_DENIED Permission denied
 * @see #minicontrol_provider_event_e
 * @see #minicontrol_create_window
 * @see #minicontrol_request_to_viewer_e
 */
int minicontrol_send_event(Evas_Object *mincontrol, minicontrol_provider_event_e event, bundle *event_arg);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* _MINICTRL_PROVIDER_H_ */

