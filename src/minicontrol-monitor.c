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

#include <stdlib.h>
#include <dbus/dbus.h>

#include "minicontrol-error.h"
#include "minicontrol-internal.h"
#include "minicontrol-monitor.h"
#include "minicontrol-viewer.h"
#include "minicontrol-log.h"

struct _minicontrol_monitor {
	minictrl_sig_handle *event_sh;
	minicontrol_monitor_cb callback;
	void *user_data;
};

static struct _minicontrol_monitor *g_monitor_h = NULL;

static minicontrol_priority_e _int_to_priority(unsigned int value)
{
	minicontrol_priority_e priority = MINICONTROL_PRIORITY_LOW;
	switch (value) {
	case MINICONTROL_PRIORITY_TOP:
		priority = MINICONTROL_PRIORITY_TOP;
		break;
	case MINICONTROL_PRIORITY_MIDDLE:
		priority = MINICONTROL_PRIORITY_MIDDLE;
		break;
	case MINICONTROL_PRIORITY_LOW:
	default:
		priority = MINICONTROL_PRIORITY_LOW;
		break;
	}
	return priority;
}

static void _sig_to_viewer_handler_cb(minicontrol_event_e event, const char *minicontrol_name, bundle *event_arg, void *data)
{
	minicontrol_action_e action;
	int width = 0;
	int height = 0;
	int priority_from_signal = 0;
	minicontrol_priority_e priority = 0;
	size_t n_size;

	switch(event) {
	case MINICONTROL_EVENT_START:
		action = MINICONTROL_ACTION_START;
		break;

	case MINICONTROL_EVENT_STOP:
		action = MINICONTROL_ACTION_STOP;
		break;

	case MINICONTROL_EVENT_RESIZE:
		action = MINICONTROL_ACTION_RESIZE;
		break;

	case MINICONTROL_EVENT_REQUEST_HIDE:
	case MINICONTROL_EVENT_REQUEST_ANGLE:
		action = MINICONTROL_ACTION_REQUEST;
		break;

	default:
		WARN("Not supported event [%d]", event);
		action = event;
		break;
	}

	if (action == MINICONTROL_ACTION_START || action == MINICONTROL_ACTION_RESIZE || action == MINICONTROL_ACTION_REQUEST) {
		bundle_get_byte(event_arg, "width", (void*)&width, &n_size);
		bundle_get_byte(event_arg, "height", (void*)&height, &n_size);
		bundle_get_byte(event_arg, "priority", (void*)&priority_from_signal, &n_size);
		priority = _int_to_priority(priority_from_signal);
	}
	else {
		priority = MINICONTROL_PRIORITY_LOW;
	}

	g_monitor_h->callback(action, minicontrol_name, width, height, priority, g_monitor_h->user_data);
}

EXPORT_API minicontrol_error_e minicontrol_monitor_start(minicontrol_monitor_cb callback, void *data)
{
	if (!callback)
		return MINICONTROL_ERROR_INVALID_PARAMETER;

	INFO("callback[%p], data[%p]", callback, data);

	if (g_monitor_h) {
		ERR("Already started");
		return MINICONTROL_ERROR_UNKNOWN;
	}

	g_monitor_h = malloc(sizeof(struct _minicontrol_monitor));
	if (g_monitor_h == NULL) {
		ERR("fail to alloc monitor_h");
		return MINICONTROL_ERROR_OUT_OF_MEMORY;
	}

	minicontrol_viewer_set_event_cb(_sig_to_viewer_handler_cb, data);

	g_monitor_h->callback = callback;
	g_monitor_h->user_data = data;

	return _minictrl_viewer_req_message_send();
}

EXPORT_API minicontrol_error_e minicontrol_monitor_stop(void)
{
	if (!g_monitor_h)
		return MINICONTROL_ERROR_NONE;

	minicontrol_viewer_unset_event_cb();

	free(g_monitor_h);
	g_monitor_h = NULL;

	return MINICONTROL_ERROR_NONE;
}
