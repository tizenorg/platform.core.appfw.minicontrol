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

#ifndef _MINICTRL_INTERNAL_H_
#define _MINICTRL_INTERNAL_H_

#include <dbus/dbus.h>
#include <sys/types.h>
#include <unistd.h>
#include <bundle.h>
#include "minicontrol-type.h"

#ifndef EXPORT_API
#define EXPORT_API __attribute__ ((visibility("default")))
#endif /* EXPORT_API */

#define MINICTRL_DBUS_SIG_RUNNING_REQ "minicontrol_running_request"

#define MINICTRL_DBUS_SIG_TO_PROVIDER "minicontrol_signal_to_provider"
#define MINICTRL_DBUS_SIG_TO_VIEWER   "minicontrol_signal_to_viewer"

#define BUNDLE_BUFFER_LENGTH 100

/**
 * @brief Enumeration for describing type of dbus.
 */

enum _minictrl_PROC_DBUS_TYPE {
	MINICONTROL_DBUS_PROC_EXCLUDE,    /**< Request to exclude from the unfreezing process list */
	MINICONTROL_DBUS_PROC_INCLUDE,    /**< Request to include to the unfreezing process list */
};

/**
 * @brief Enumeration for describing type of actions allowed to inhouse apps.
 * @since_tizen 2.4
 */
typedef enum {
	MINICONTROL_REQ_NONE = 0,
	MINICONTROL_REQ_HIDE_VIEWER = 100,       /**< Requests that the minicontrol viewer(s) close the provider's minicontrol */
	MINICONTROL_REQ_FREEZE_SCROLL_VIEWER,    /**< Requests that the minicontrol viewer(s) freeze its window scroll */
	MINICONTROL_REQ_UNFREEZE_SCROLL_VIEWER,  /**< Requests that the minicontrol viewer(s) unfreeze its window scroll */
	MINICONTROL_REQ_REPORT_VIEWER_ANGLE,     /**< Requests the current angle of the minicontrol viewer */
	MINICONTROL_REQ_ROTATE_PROVIDER = 200,	 /**< Requests that the minicontrol provider rotate the provider's minicontrol */
} minicontrol_request_e;

/**
 * @brief Enumeration for describing priority of a minicontrol provider.
 * @since_tizen 2.4
 */
typedef enum {
	MINICONTROL_PRIORITY_TOP = 1000,    /**< Top priority */
	MINICONTROL_PRIORITY_MIDDLE = 100,    /**< Middle priority */
	MINICONTROL_PRIORITY_LOW = 1,    /**< Low priority */
} minicontrol_priority_e;

/**
 * @brief Enumeration for describing type of events originated by a minicontrol provider.
 * @since_tizen 2.4
 */
typedef enum _minicontrol_action {
	MINICONTROL_ACTION_START = 0,  /**< A minicontrol object is created */
	MINICONTROL_ACTION_STOP,       /**< A minicontrol object is deleted */
	MINICONTROL_ACTION_RESIZE,     /**< A minicontrol object is resized */
	MINICONTROL_ACTION_REQUEST,    /**< the viewer of the minicontrol object is asked to do something */
} minicontrol_action_e;

typedef enum {
	MINICONTROL_EVENT_REQUEST_LOCK = 1001, /**< A minicontrol object should not be removed by user */
} minicontrol_internal_provider_event_e;

typedef struct _minictrl_sig_handle minictrl_sig_handle;

int _minictrl_provider_message_send(int event, const char *minicontrol_name, unsigned int witdh, unsigned int height, minicontrol_priority_e priority);

int _minictrl_viewer_req_message_send(void);

minictrl_sig_handle *_minictrl_dbus_sig_handle_attach(const char *signal,
				void (*callback) (void *data, DBusMessage *msg),
				void *data);

void _minictrl_dbus_sig_handle_dettach(minictrl_sig_handle *handle);

int _minictrl_provider_proc_send(int type);

/* new */
int _minictrl_send_event(const char *signal_name, const char *minicontrol_name, int event, bundle *signal_arg);

#endif /* _MINICTRL_INTERNAL_H_ */

