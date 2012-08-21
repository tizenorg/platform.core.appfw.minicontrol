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

#include <stdlib.h>
#include <dbus/dbus.h>

#include "minicontrol-error.h"
#include "minicontrol-internal.h"
#include "minicontrol-monitor.h"
#include "minicontrol-log.h"

struct _minicontrol_monitor {
	minictrl_sig_handle *start_sh;
	minictrl_sig_handle *stop_sh;
	minictrl_sig_handle *resize_sh;
	minicontrol_monitor_cb callback;
	void *user_data;
};

static struct _minicontrol_monitor *g_monitor_h;

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

static void _provider_start_cb(void *data, DBusMessage *msg)
{
	DBusError err;
	char *svr_name = NULL;
	unsigned int w = 0;
	unsigned int h = 0;
	unsigned int pri = 0;
	minicontrol_priority_e priority;
	dbus_bool_t dbus_ret;

	dbus_error_init(&err);

	dbus_ret = dbus_message_get_args(msg, &err,
				DBUS_TYPE_STRING, &svr_name,
				DBUS_TYPE_UINT32, &w,
				DBUS_TYPE_UINT32, &h,
				DBUS_TYPE_UINT32, &pri,
				DBUS_TYPE_INVALID);
	if (!dbus_ret) {
		ERR("fail to get args : %s", err.message);
		dbus_error_free(&err);
		return;
	}

	priority = _int_to_priority(pri);

	if (g_monitor_h->callback)
		g_monitor_h->callback(MINICONTROL_ACTION_START,
				svr_name, w, h, priority,
				g_monitor_h->user_data);

	dbus_error_free(&err);
}

static void _provider_stop_cb(void *data, DBusMessage *msg)
{
	DBusError err;
	char *svr_name = NULL;
	dbus_bool_t dbus_ret;

	dbus_error_init(&err);

	dbus_ret = dbus_message_get_args(msg, &err,
				DBUS_TYPE_STRING, &svr_name,
				DBUS_TYPE_INVALID);
	if (!dbus_ret) {
		ERR("fail to get args : %s", err.message);
		dbus_error_free(&err);
		return;
	}

	if (g_monitor_h->callback)
		g_monitor_h->callback(MINICONTROL_ACTION_STOP,
				svr_name, 0, 0, MINICONTROL_PRIORITY_LOW,
				g_monitor_h->user_data);

	dbus_error_free(&err);
}

static void _provider_resize_cb(void *data, DBusMessage *msg)
{
	DBusError err;
	char *svr_name = NULL;
	unsigned int w = 0;
	unsigned int h = 0;
	unsigned int pri = 0;
	minicontrol_priority_e priority;
	dbus_bool_t dbus_ret;

	dbus_error_init(&err);

	dbus_ret = dbus_message_get_args(msg, &err,
				DBUS_TYPE_STRING, &svr_name,
				DBUS_TYPE_UINT32, &w,
				DBUS_TYPE_UINT32, &h,
				DBUS_TYPE_UINT32, &pri,
				DBUS_TYPE_INVALID);
	if (!dbus_ret) {
		ERR("fail to get args : %s", err.message);
		dbus_error_free(&err);
		return;
	}

	priority = _int_to_priority(pri);

	if (g_monitor_h->callback)
		g_monitor_h->callback(MINICONTROL_ACTION_RESIZE,
				svr_name, w, h, priority,
				g_monitor_h->user_data);

	dbus_error_free(&err);
}


EXPORT_API minicontrol_error_e minicontrol_monitor_start(
				minicontrol_monitor_cb callback, void *data)
{
	if (!callback)
		return MINICONTROL_ERROR_INVALID_PARAMETER;

	if (!g_monitor_h) {
		minictrl_sig_handle *start_sh;
		minictrl_sig_handle *stop_sh;
		minictrl_sig_handle *resize_sh;
		struct _minicontrol_monitor *monitor_h;

		start_sh = _minictrl_dbus_sig_handle_attach(
				MINICTRL_DBUS_SIG_START,
				_provider_start_cb, NULL);
		if (!start_sh) {
			ERR("fail to _minictrl_dbus_sig_handle_attach - %s",
				MINICTRL_DBUS_SIG_START);
			return MINICONTROL_ERROR_DBUS;
		}

		stop_sh = _minictrl_dbus_sig_handle_attach(
				MINICTRL_DBUS_SIG_STOP,
				_provider_stop_cb, NULL);
		if (!start_sh) {
			ERR("fail to _minictrl_dbus_sig_handle_attach - %s",
				MINICTRL_DBUS_SIG_STOP);
			return MINICONTROL_ERROR_DBUS;
		}

		resize_sh = _minictrl_dbus_sig_handle_attach(
				MINICTRL_DBUS_SIG_RESIZE,
				_provider_resize_cb, NULL);
		if (!resize_sh) {
			ERR("fail to _minictrl_dbus_sig_handle_attach - %s",
				MINICTRL_DBUS_SIG_RESIZE);
			return MINICONTROL_ERROR_DBUS;
		}

		monitor_h = malloc(sizeof(struct _minicontrol_monitor));
		if (!monitor_h) {
			ERR("fail to alloc monitor_h");
			_minictrl_dbus_sig_handle_dettach(start_sh);
			_minictrl_dbus_sig_handle_dettach(stop_sh);
			_minictrl_dbus_sig_handle_dettach(resize_sh);
			return MINICONTROL_ERROR_OUT_OF_MEMORY;
		}

		monitor_h->start_sh = start_sh;
		monitor_h->stop_sh = stop_sh;
		monitor_h->resize_sh = resize_sh;
		g_monitor_h = monitor_h;
	}

	g_monitor_h->callback = callback;
	g_monitor_h->user_data = data;
	INFO("callback[%p], data[%p]", callback, data);

	return _minictrl_viewer_req_message_send();
}

EXPORT_API minicontrol_error_e minicontrol_monitor_stop(void)
{
	if (!g_monitor_h)
		return MINICONTROL_ERROR_NONE;

	if (g_monitor_h->start_sh)
		_minictrl_dbus_sig_handle_dettach(g_monitor_h->start_sh);

	if (g_monitor_h->stop_sh)
		_minictrl_dbus_sig_handle_dettach(g_monitor_h->stop_sh);

	if (g_monitor_h->resize_sh)
		_minictrl_dbus_sig_handle_dettach(g_monitor_h->resize_sh);

	free(g_monitor_h);
	g_monitor_h = NULL;

	return MINICONTROL_ERROR_NONE;
}

