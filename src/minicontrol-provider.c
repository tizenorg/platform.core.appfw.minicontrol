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

#include <Elementary.h>
#include <Ecore_Evas.h>

#include "minicontrol-error.h"
#include "minicontrol-type.h"
#include "minicontrol-internal.h"
#include "minicontrol-provider.h"
#include "minicontrol-provider-internal.h"
#include "minicontrol-log.h"

#define MINICTRL_PRIORITY_SUFFIX_TOP "__minicontrol_top"
#define MINICTRL_PRIORITY_SUFFIX_LOW "__minicontrol_low"
#define MINICTRL_DATA_KEY "__minictrl_data_internal"

enum {
	MINICTRL_STATE_READY = 0,
	MINICTRL_STATE_RUNNING,
};

struct _minicontrol_provider {
	char *name;
	int state;
	Evas_Object *obj;
	minictrl_sig_handle *running_sh;
	minictrl_sig_handle *event_sh;
	minicontrol_event_cb event_callback;
};

static void _minictrl_win_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _minictrl_win_hide_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _minictrl_win_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _minictrl_win_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void __minicontrol_provider_free(struct _minicontrol_provider *pd)
{
	if (pd) {
		if (pd->name)
			free(pd->name);

		if (pd->running_sh)
			_minictrl_dbus_sig_handle_dettach(pd->running_sh);

		if (pd->event_sh)
			_minictrl_dbus_sig_handle_dettach(pd->event_sh);

		free(pd);
	}
}

static void _running_req_cb(void *data, DBusMessage *msg)
{
	struct _minicontrol_provider *pd;

	if (!data) {
		ERR("data is NULL");
		return;
	}
	pd = data;

	if (pd->state == MINICTRL_STATE_RUNNING) {
		Evas_Coord w = 0;
		Evas_Coord h = 0;
		evas_object_geometry_get(pd->obj, NULL, NULL, &w, &h);
		_minictrl_provider_message_send(MINICONTROL_EVENT_START, pd->name, w, h, 0);
	}
}

static void _sig_to_provider_handler_cb(void *data, DBusMessage *msg)
{
	struct _minicontrol_provider *pd;
	DBusError err;
	char *minicontrol_name = NULL;
	minicontrol_viewer_event_e event;
	dbus_bool_t dbus_ret;
	bundle *event_arg_bundle = NULL;
	bundle_raw *serialized_arg = NULL;
	unsigned int serialized_arg_length = 0;


	if (!data) {
		ERR("data is NULL");
		return;
	}
	pd = data;

	dbus_error_init(&err); /* Does not allocate any memory. the error only needs to be freed if it is set at some point. */

	dbus_ret = dbus_message_get_args(msg, &err,
				DBUS_TYPE_STRING, &minicontrol_name,
				DBUS_TYPE_INT32,  &event,
				DBUS_TYPE_STRING, &serialized_arg,
				DBUS_TYPE_UINT32, &serialized_arg_length,
				DBUS_TYPE_INVALID);

	if (!dbus_ret) {
		ERR("fail to get args : %s", err.message);
		dbus_error_free(&err);
		return;
	}

	INFO("minicontrol_name[%s] event[%d] pd->name[%s]", minicontrol_name, event, pd->name);

	if (minicontrol_name && pd->name && strcmp(minicontrol_name, pd->name) == 0) {
		event_arg_bundle = bundle_decode(serialized_arg, serialized_arg_length);
		/* event argument can be null */

		if (event == MINICONTROL_VIEWER_EVENT_SHOW) {
			Evas_Coord width;
			Evas_Coord height;
			evas_object_geometry_get(pd->obj, NULL, NULL, &width, &height);
			INFO("width[%d] height[%d]", width, height);
			_minictrl_provider_message_send(MINICONTROL_EVENT_RESIZE, pd->name, width, height, 0);
		}

		if (pd->event_callback)
			pd->event_callback(event, event_arg_bundle);

		if (event_arg_bundle)
			bundle_free(event_arg_bundle);
	}
}



static char *_minictrl_create_name(const char *name)
{
	char *buf;
	int size = 0;

	if (!name) {
		ERR("name is NULL, invaild parameter");
		set_last_result(MINICONTROL_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	size = snprintf(NULL, 0, "[%s]", name) + 1;
	buf = (char *)malloc(sizeof(char) * size);
	if (!buf) {
		ERR("fail to alloc buf");
		set_last_result(MINICONTROL_ERROR_OUT_OF_MEMORY);
		return NULL;
	}

	snprintf(buf, size, "[%s]", name);

	return buf;
}

static void _access_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));
	if (ee != NULL) {
		if (elm_config_access_get()) {
			/* TODO : Check this API is supported
			ecore_evas_extn_socket_events_block_set(ee, EINA_TRUE);
			 */
		} else {
			/* TODO : Check this API is supported
			ecore_evas_extn_socket_events_block_set(ee, EINA_FALSE);
			 */
		}
	}
}


EXPORT_API Evas_Object* minicontrol_create_window(const char *name, minicontrol_target_viewer_e target_viewer, minicontrol_event_cb event_callback)
{
	int err_from_elm;
	Evas_Object *win = NULL;
	char *name_inter = NULL;
	struct _minicontrol_provider *pd;

	if (!name) {
		ERR("invalid parameter");
		set_last_result(MINICONTROL_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	win = elm_win_add(NULL, "minicontrol", ELM_WIN_SOCKET_IMAGE);
	if (!win) {
		ERR("elm_win_add failed");
		set_last_result(MINICONTROL_ERROR_ELM_FAILURE);
		return NULL;
	}

	if (elm_config_access_get()) {
		Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas_object_evas_get(win));

		if (ee != NULL) {
			/* TODO : Check this API is supported
			ecore_evas_extn_socket_events_block_set(ee, EINA_TRUE);
			 */
		}

	}
	evas_object_smart_callback_add(win, "access,changed", _access_changed_cb, NULL);

	name_inter = _minictrl_create_name(name);
	if (!name_inter) {
		ERR("Fail to create name_inter for : %s", name);
		evas_object_del(win);
		return NULL;
	}

	err_from_elm = elm_win_socket_listen(win, name_inter, 0, EINA_TRUE);
	if (!err_from_elm) {
		ERR("Fail to elm win socket listen [%d]", err_from_elm);
		set_last_result(MINICONTROL_ERROR_ELM_FAILURE);
		evas_object_del(win);
		free(name_inter);
		return NULL;
	}

	pd = malloc(sizeof(struct _minicontrol_provider));
	if (!pd) {
		ERR("Fail to alloc memory");
		set_last_result(MINICONTROL_ERROR_OUT_OF_MEMORY);
		evas_object_del(win);
		free(name_inter);
		return NULL;

	}
	memset(pd, 0x00, sizeof(struct _minicontrol_provider));
	pd->name = name_inter;
	pd->state = MINICTRL_STATE_READY;
	pd->obj = win;

	evas_object_data_set(win, MINICTRL_DATA_KEY, pd);

	elm_win_autodel_set(win, EINA_TRUE);

	evas_object_event_callback_add(win, EVAS_CALLBACK_DEL, _minictrl_win_del_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_SHOW, _minictrl_win_show_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_HIDE,	_minictrl_win_hide_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_RESIZE, _minictrl_win_resize_cb, pd);

	pd->running_sh = _minictrl_dbus_sig_handle_attach(MINICTRL_DBUS_SIG_RUNNING_REQ, _running_req_cb, pd);
	pd->event_sh   = _minictrl_dbus_sig_handle_attach(MINICTRL_DBUS_SIG_TO_PROVIDER, _sig_to_provider_handler_cb, pd);
	pd->event_callback = event_callback;

	INFO("new minicontrol created - %s", pd->name);
	return win;
}

EXPORT_API int minicontrol_send_event(Evas_Object *mincontrol, minicontrol_provider_event_e event, bundle *event_arg)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;

	if (!mincontrol) {
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (!pd->name) {
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (pd->state == MINICTRL_STATE_RUNNING) {
		ret = _minictrl_send_event(MINICTRL_DBUS_SIG_TO_VIEWER, pd->name, event, event_arg);
	}

	return ret;
}

static int minicontrol_win_start(Evas_Object *mincontrol)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;

	if (!mincontrol) {
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (!pd->name) {
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (pd->state != MINICTRL_STATE_RUNNING) {
		Evas_Coord w = 0;
		Evas_Coord h = 0;
		pd->state = MINICTRL_STATE_RUNNING;

		evas_object_geometry_get(mincontrol, NULL, NULL, &w, &h);
		_minictrl_provider_proc_send(MINICONTROL_DBUS_PROC_EXCLUDE);
		ret = _minictrl_provider_message_send(MINICONTROL_EVENT_START, pd->name, w, h, 0);
	}

	return ret;
}

static int minicontrol_win_stop(Evas_Object *mincontrol)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;

	if (!mincontrol) {
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (!pd->name) {
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}
	if (pd->state != MINICTRL_STATE_READY) {
		pd->state = MINICTRL_STATE_READY;
		_minictrl_provider_proc_send(MINICONTROL_DBUS_PROC_INCLUDE);
		ret = _minictrl_provider_message_send(MINICONTROL_EVENT_STOP, pd->name, 0, 0, 0);
	}

	return ret;
}

static void _minictrl_win_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	struct _minicontrol_provider *pd = NULL;

	minicontrol_win_stop(obj);

	pd = evas_object_data_get(obj, MINICTRL_DATA_KEY);
	__minicontrol_provider_free(pd);

	evas_object_data_set(obj, MINICTRL_DATA_KEY, NULL);
}

static void _minictrl_win_hide_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	minicontrol_win_stop(obj);
}

static void _minictrl_win_show_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	minicontrol_win_start(obj);
}

static void _minictrl_win_resize_cb(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	struct _minicontrol_provider *pd;

	if (!data) {
		ERR("data is NULL, invaild parameter");
		return;
	}
	pd = data;

	if (pd->state == MINICTRL_STATE_RUNNING) {
		Evas_Coord w = 0;
		Evas_Coord h = 0;

		evas_object_geometry_get(obj, NULL, NULL, &w, &h);
		_minictrl_provider_message_send(MINICONTROL_EVENT_RESIZE, pd->name, w, h, 0);
	}
}

EXPORT_API Evas_Object *minicontrol_win_add(const char *name)
{
	Evas_Object *win = NULL;
	char *name_inter = NULL;
	struct _minicontrol_provider *pd;

	INFO("minicontrol_win_add [%s]", name);

	if (!name) {
		ERR("name is null");
		return NULL;
	}

	win = elm_win_add(NULL, "minicontrol", ELM_WIN_SOCKET_IMAGE);
	if (!win) {
		ERR("elm_win_add returns null for [%s]", name);
		return NULL;
	}

	if (elm_config_access_get()) {
		Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas_object_evas_get(win));

		if (ee != NULL) {
			/* TODO : Check this API is supported
			ecore_evas_extn_socket_events_block_set(ee, EINA_TRUE);
			 */
		}

	}
	evas_object_smart_callback_add(win, "access,changed", _access_changed_cb, NULL);

	name_inter = _minictrl_create_name(name);
	if (!name_inter) {

		ERR("Fail to create name_inter for : %s", name);
		evas_object_del(win);
		return NULL;
	}

	if (!elm_win_socket_listen(win, name_inter, 0, EINA_TRUE)) {
		ERR("Fail to elm win socket listen");
		evas_object_del(win);
		free(name_inter);
		return NULL;
	}

	pd = malloc(sizeof(struct _minicontrol_provider));
	if (!pd) {
		ERR("Fail to alloc memory");
		evas_object_del(win);
		free(name_inter);
		return NULL;

	}
	memset(pd, 0x00, sizeof(struct _minicontrol_provider));
	pd->name = name_inter;
	pd->state = MINICTRL_STATE_READY;
	pd->obj = win;

	evas_object_data_set(win, MINICTRL_DATA_KEY, pd);

	elm_win_autodel_set(win, EINA_TRUE);

	evas_object_event_callback_add(win, EVAS_CALLBACK_DEL, _minictrl_win_del_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_SHOW, _minictrl_win_show_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_HIDE,	_minictrl_win_hide_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_RESIZE, _minictrl_win_resize_cb, pd);

	pd->running_sh = _minictrl_dbus_sig_handle_attach(MINICTRL_DBUS_SIG_RUNNING_REQ, _running_req_cb, pd);

	INFO("new minicontrol created - %s", pd->name);
	return win;
}

EXPORT_API int minicontrol_request(Evas_Object *mincontrol, minicontrol_request_e request)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;
	minicontrol_event_e event;

	if (!mincontrol) {
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (!pd->name) {
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (pd->state == MINICTRL_STATE_RUNNING) {
		switch (request) {
		case MINICONTROL_REQ_HIDE_VIEWER:
			event = MINICONTROL_EVENT_REQUEST_HIDE;
			break;

		case MINICONTROL_REQ_REPORT_VIEWER_ANGLE:
			event = MINICONTROL_EVENT_REQUEST_ANGLE;
			break;

		case MINICONTROL_REQ_FREEZE_SCROLL_VIEWER:
		case MINICONTROL_REQ_UNFREEZE_SCROLL_VIEWER:
		case MINICONTROL_REQ_ROTATE_PROVIDER:
			WARN("Could be not supported [%d]", request);
			event = request;
			break;

		case MINICONTROL_REQ_NONE:
		default:
			ERR("Not supported request[%d]", request);
			ret = MINICONTROL_ERROR_NOT_SUPPORTED;
			goto out;
		}

		_minictrl_send_event(MINICTRL_DBUS_SIG_TO_VIEWER, pd->name, event, NULL);
	}
out:
	return ret;
}

