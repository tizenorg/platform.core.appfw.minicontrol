/*
 * Copyright (c) 2013 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

static void _minictrl_win_del_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info);
static void _minictrl_win_hide_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info);
static void _minictrl_win_show_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info);
static void _minictrl_win_resize_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info);

static void __minicontrol_provider_free(struct _minicontrol_provider *pd)
{
	if (pd == NULL)
		return;

	if (pd->name)
		free(pd->name);

	if (pd->running_sh)
		_minictrl_dbus_sig_handle_dettach(pd->running_sh);

	if (pd->event_sh)
		_minictrl_dbus_sig_handle_dettach(pd->event_sh);

	free(pd);
}

static void _running_req_cb(void *data, GVariant *parameters)
{
	struct _minicontrol_provider *pd = data;
	Evas_Coord w = 0;
	Evas_Coord h = 0;

	if (!pd) {
		ERR("provider is NULL");
		return;
	}

	if (pd->state == MINICTRL_STATE_RUNNING) {
		evas_object_geometry_get(pd->obj, NULL, NULL, &w, &h);
		_minictrl_provider_message_send(MINICONTROL_EVENT_START,
				pd->name, w, h, 0);
	}
}

static void _sig_to_provider_handler_cb(void *data, GVariant *parameters)
{
	struct _minicontrol_provider *pd = data;
	char *minicontrol_name = NULL;
	minicontrol_viewer_event_e event;
	bundle *event_arg_bundle;
	bundle_raw *serialized_arg = NULL;
	unsigned int serialized_arg_length = 0;
	Evas_Coord width;
	Evas_Coord height;

	if (!pd) {
		ERR("provider is NULL");
		return;
	}

	g_variant_get(parameters, "(&si&su)", &minicontrol_name, &event,
			&serialized_arg, &serialized_arg_length);

	INFO("minicontrol_name[%s] event[%d] pd->name[%s]",
			minicontrol_name, event, pd->name);

	if (minicontrol_name && pd->name &&
			strcmp(minicontrol_name, pd->name) == 0) {
		event_arg_bundle = bundle_decode(serialized_arg,
				serialized_arg_length);
		if (event == MINICONTROL_VIEWER_EVENT_SHOW) {
			evas_object_geometry_get(pd->obj, NULL, NULL,
					&width, &height);
			INFO("width[%d] height[%d]", width, height);
			_minictrl_provider_message_send(
					MINICONTROL_EVENT_RESIZE, pd->name,
					width, height, 0);
		}

		if (pd->event_callback)
			pd->event_callback(event, event_arg_bundle);

		if (event_arg_bundle)
			bundle_free(event_arg_bundle);
	}
}

static char *_minictrl_create_name(const char *name)
{
	char *minictrl_name;
	int size;

	size = strlen(name) + 3;
	minictrl_name = malloc(sizeof(char) * size);
	if (minictrl_name == NULL) {
		/* LCOV_EXCL_START */
		ERR("out of memory");
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	snprintf(minictrl_name, size, "[%s]", name);

	return minictrl_name;
}

EXPORT_API Evas_Object *minicontrol_create_window(const char *name,
		minicontrol_target_viewer_e target_viewer,
		minicontrol_event_cb event_callback)
{
	int err_from_elm;
	Evas_Object *win;
	char *name_inter;
	struct _minicontrol_provider *pd;

	if (!name) {
		ERR("invalid parameter");
		set_last_result(MINICONTROL_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	win = elm_win_add(NULL, "minicontrol", ELM_WIN_SOCKET_IMAGE);
	if (!win) {
		/* LCOV_EXCL_START */
		ERR("elm_win_add failed");
		set_last_result(MINICONTROL_ERROR_ELM_FAILURE);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	name_inter = _minictrl_create_name(name);
	if (!name_inter) {
		/* LCOV_EXCL_START */
		ERR("Fail to create name_inter for : %s", name);
		set_last_result(MINICONTROL_ERROR_OUT_OF_MEMORY);
		evas_object_del(win);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	err_from_elm = elm_win_socket_listen(win, name_inter, 0, EINA_TRUE);
	if (!err_from_elm) {
		/* LCOV_EXCL_START */
		ERR("Fail to elm win socket listen [%d]", err_from_elm);
		set_last_result(MINICONTROL_ERROR_ELM_FAILURE);
		evas_object_del(win);
		free(name_inter);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	pd = malloc(sizeof(struct _minicontrol_provider));
	if (!pd) {
		/* LCOV_EXCL_START */
		ERR("Fail to alloc memory");
		set_last_result(MINICONTROL_ERROR_OUT_OF_MEMORY);
		evas_object_del(win);
		free(name_inter);
		return NULL;
		/* LCOV_EXCL_STOP */

	}
	memset(pd, 0x00, sizeof(struct _minicontrol_provider));

	pd->name = name_inter;
	pd->state = MINICTRL_STATE_READY;
	pd->obj = win;

	evas_object_data_set(win, MINICTRL_DATA_KEY, pd);
	elm_win_autodel_set(win, EINA_TRUE);

	evas_object_event_callback_add(win, EVAS_CALLBACK_DEL,
			_minictrl_win_del_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_SHOW,
			_minictrl_win_show_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_HIDE,
			_minictrl_win_hide_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_RESIZE,
			_minictrl_win_resize_cb, pd);

	pd->running_sh = _minictrl_dbus_sig_handle_attach(
			MINICTRL_DBUS_SIG_RUNNING_REQ, _running_req_cb, pd);
	pd->event_sh = _minictrl_dbus_sig_handle_attach(
			MINICTRL_DBUS_SIG_TO_PROVIDER,
			_sig_to_provider_handler_cb, pd);
	pd->event_callback = event_callback;
	INFO("new minicontrol created - %s", pd->name);

	return win;
}

EXPORT_API int minicontrol_send_event(Evas_Object *mincontrol,
		minicontrol_provider_event_e event, bundle *event_arg)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;

	if (!mincontrol) {
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		/* LCOV_EXCL_START */
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	if (!pd->name) {
		/* LCOV_EXCL_START */
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	if (pd->state == MINICTRL_STATE_RUNNING) {
		ret = _minictrl_send_event(MINICTRL_DBUS_SIG_TO_VIEWER,
				pd->name, event, event_arg);
	}

	return ret;
}

static int minicontrol_win_start(Evas_Object *mincontrol)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;
	Evas_Coord w = 0;
	Evas_Coord h = 0;

	if (!mincontrol) {
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		/* LCOV_EXCL_START */
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	if (!pd->name) {
		/* LCOV_EXCL_START */
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	if (pd->state != MINICTRL_STATE_RUNNING) {
		pd->state = MINICTRL_STATE_RUNNING;
		evas_object_geometry_get(mincontrol, NULL, NULL, &w, &h);
		_minictrl_provider_proc_send(MINICONTROL_DBUS_PROC_EXCLUDE);
		ret = _minictrl_provider_message_send(MINICONTROL_EVENT_START,
				pd->name, w, h, 0);
	}

	return ret;
}

static int minicontrol_win_stop(Evas_Object *mincontrol)
{
	struct _minicontrol_provider *pd;
	int ret = MINICONTROL_ERROR_NONE;

	if (!mincontrol) {
		/* LCOV_EXCL_START */
		ERR("mincontrol is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	pd = evas_object_data_get(mincontrol, MINICTRL_DATA_KEY);
	if (!pd) {
		/* LCOV_EXCL_START */
		ERR("pd is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	if (!pd->name) {
		/* LCOV_EXCL_START */
		ERR("pd name is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
		/* LCOV_EXCL_STOP */
	}

	if (pd->state != MINICTRL_STATE_READY) {
		pd->state = MINICTRL_STATE_READY;
		_minictrl_provider_proc_send(MINICONTROL_DBUS_PROC_INCLUDE);
		ret = _minictrl_provider_message_send(MINICONTROL_EVENT_STOP,
				pd->name, 0, 0, 0);
	}

	return ret;
}

static void _minictrl_win_del_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	struct _minicontrol_provider *pd;

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
	struct _minicontrol_provider *pd = data;
	Evas_Coord w = 0;
	Evas_Coord h = 0;

	if (!pd) {
		ERR("Invalid parameter");
		return;
	}

	if (pd->state == MINICTRL_STATE_RUNNING) {
		evas_object_geometry_get(obj, NULL, NULL, &w, &h);
		_minictrl_provider_message_send(MINICONTROL_EVENT_RESIZE,
				pd->name, w, h, 0);
	}
}

/* LCOV_EXCL_START */
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

	evas_object_event_callback_add(win, EVAS_CALLBACK_DEL,
			_minictrl_win_del_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_SHOW,
			_minictrl_win_show_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_HIDE,
			_minictrl_win_hide_cb, pd);
	evas_object_event_callback_add(win, EVAS_CALLBACK_RESIZE,
			_minictrl_win_resize_cb, pd);

	pd->running_sh = _minictrl_dbus_sig_handle_attach(
			MINICTRL_DBUS_SIG_RUNNING_REQ, _running_req_cb, pd);
	INFO("new minicontrol created - %s", pd->name);

	return win;
}
/* LCOV_EXCL_STOP */

/* LCOV_EXCL_START */
EXPORT_API int minicontrol_request(Evas_Object *mincontrol,
		minicontrol_request_e request)
{
	struct _minicontrol_provider *pd;
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
		default:
			ERR("Not supported request[%d]", request);
			return MINICONTROL_ERROR_NOT_SUPPORTED;
		}

		_minictrl_send_event(MINICTRL_DBUS_SIG_TO_VIEWER, pd->name,
				event, NULL);
	}

	return MINICONTROL_ERROR_NONE;
}
/* LCOV_EXCL_STOP */

