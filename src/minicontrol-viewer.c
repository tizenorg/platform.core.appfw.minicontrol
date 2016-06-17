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

#include "minicontrol-internal.h"
#include "minicontrol-type.h"
#include "minicontrol-viewer.h"
#include "minicontrol-viewer-internal.h"
#include "minicontrol-log.h"

#define MINICTRL_PLUG_DATA_KEY "__minictrl_plug_name"

struct _minicontrol_viewer {
	minictrl_sig_handle *event_sh;
	minicontrol_viewer_event_cb callback;
	void *user_data;
};

static struct _minicontrol_viewer *g_minicontrol_viewer_h;

EXPORT_API int minicontrol_viewer_send_event(const char *minicontrol_name,
		minicontrol_viewer_event_e event, bundle *event_arg)
{
	int ret = MINICONTROL_ERROR_NONE;

	if (minicontrol_name == NULL) {
		ERR("appid is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	ret = _minictrl_send_event(MINICTRL_DBUS_SIG_TO_PROVIDER,
			minicontrol_name, event, event_arg);

	return ret;
}

static void _sig_to_viewer_handler_cb(void *data, GVariant *parameters)
{
	char *minicontrol_name = NULL;
	minicontrol_event_e event;
	bundle *event_arg_bundle = NULL;
	bundle_raw *serialized_arg = NULL;
	unsigned int serialized_arg_length = 0;

	g_variant_get(parameters, "(&si&su)", &minicontrol_name, &event,
			&serialized_arg, &serialized_arg_length);

	if (serialized_arg_length != 0) {
		event_arg_bundle = bundle_decode(serialized_arg,
				serialized_arg_length);
		if (event_arg_bundle == NULL) {
			/* LCOV_EXCL_START */
			ERR("fail to deserialize arguments");
			return;
			/* LCOV_EXCL_STOP */
		}
	}

	if (g_minicontrol_viewer_h->callback) {
		g_minicontrol_viewer_h->callback(event, minicontrol_name,
				event_arg_bundle,
				g_minicontrol_viewer_h->user_data);
	}

	bundle_free(event_arg_bundle);
}


EXPORT_API int minicontrol_viewer_set_event_cb(
		minicontrol_viewer_event_cb callback, void *data)
{
	minictrl_sig_handle *event_sh;
	struct _minicontrol_viewer *minicontrol_viewer_h;

	if (!callback) {
		ERR("MINICONTROL_ERROR_INVALID_PARAMETER");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	INFO("g_minicontrol_viewer_h [%p]", g_minicontrol_viewer_h);

	if (g_minicontrol_viewer_h == NULL) {
		event_sh = _minictrl_dbus_sig_handle_attach(
				MINICTRL_DBUS_SIG_TO_VIEWER,
				_sig_to_viewer_handler_cb, NULL);
		if (!event_sh) {
			/* LCOV_EXCL_START */
			ERR("fail to _minictrl_dbus_sig_handle_attach - %s",
					MINICTRL_DBUS_SIG_TO_VIEWER);
			return MINICONTROL_ERROR_IPC_FAILURE;
			/* LCOV_EXCL_STOP */
		}

		minicontrol_viewer_h =
			malloc(sizeof(struct _minicontrol_viewer));
		if (!minicontrol_viewer_h) {
			/* LCOV_EXCL_START */
			ERR("fail to alloc minicontrol_viewer_h");
			_minictrl_dbus_sig_handle_dettach(event_sh);
			return MINICONTROL_ERROR_OUT_OF_MEMORY;
			/* LCOV_EXCL_STOP */
		}

		minicontrol_viewer_h->event_sh = event_sh;
		g_minicontrol_viewer_h = minicontrol_viewer_h;
	}

	g_minicontrol_viewer_h->callback = callback;
	g_minicontrol_viewer_h->user_data = data;
	INFO("callback[%p], data[%p]", callback, data);

	return _minictrl_viewer_req_message_send();
}

EXPORT_API int minicontrol_viewer_unset_event_cb(void)
{
	if (!g_minicontrol_viewer_h)
		return MINICONTROL_ERROR_NONE;

	if (g_minicontrol_viewer_h->event_sh) {
		_minictrl_dbus_sig_handle_dettach(
				g_minicontrol_viewer_h->event_sh);
	}

	free(g_minicontrol_viewer_h);
	g_minicontrol_viewer_h = NULL;

	return MINICONTROL_ERROR_NONE;
}

/* LCOV_EXCL_START */
static void _minictrl_plug_server_del(Ecore_Evas *ee)
{
	char *minicontrol_name = NULL;

	minicontrol_name = ecore_evas_data_get(ee, MINICTRL_PLUG_DATA_KEY);
	if (!minicontrol_name) {
		ERR("fail to get minicontrol_name");
		return;
	}

	INFO("server - %s is deleted", minicontrol_name);

	/*
	 * To avoid retrying to free minicontrol_name again,
	 * set MINICTRL_PLUG_DATA_KEY as NULL
	 */
	ecore_evas_data_set(ee, MINICTRL_PLUG_DATA_KEY, NULL);

	/* send message to remove plug */
	_minictrl_provider_message_send(MINICONTROL_EVENT_STOP,
			minicontrol_name, 0, 0, MINICONTROL_PRIORITY_LOW);
	_minictrl_provider_proc_send(MINICONTROL_DBUS_PROC_INCLUDE);
	free(minicontrol_name);
}
/* LCOV_EXCL_STOP */

static void _minictrl_plug_del(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	Ecore_Evas *ee = data;
	char *minicontrol_name;

	if (!ee)
		return;

	minicontrol_name = ecore_evas_data_get(ee, MINICTRL_PLUG_DATA_KEY);
	if (minicontrol_name) {
		/*
		 * Sending an event 'MINICONTROL_EVENT_REQUEST_HIDE'
		 * should be done by minicontrol viewer manually
		 */
		free(minicontrol_name);
		ecore_evas_data_set(ee, MINICTRL_PLUG_DATA_KEY, NULL);
	}
}

EXPORT_API Evas_Object *minicontrol_viewer_add(Evas_Object *parent,
		const char *minicontrol_name)
{
	Evas_Object *plug = NULL;
	Evas_Object *plug_img = NULL;
	Ecore_Evas *ee = NULL;

	if (parent == NULL || minicontrol_name == NULL) {
		ERR("invalid parameter");
		set_last_result(MINICONTROL_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	plug = elm_plug_add(parent);
	if (!plug) {
		/* LCOV_EXCL_START */
		ERR("fail to create plug");
		set_last_result(MINICONTROL_ERROR_ELM_FAILURE);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	if (!elm_plug_connect(plug, minicontrol_name, 0, EINA_TRUE)) {
		/* LCOV_EXCL_START */
		ERR("Cannot connect plug[%s]", minicontrol_name);
		set_last_result(MINICONTROL_ERROR_ELM_FAILURE);
		evas_object_del(plug);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	plug_img = elm_plug_image_object_get(plug);

	ee = ecore_evas_object_ecore_evas_get(plug_img);
	ecore_evas_data_set(ee, MINICTRL_PLUG_DATA_KEY,
			strdup(minicontrol_name));
	ecore_evas_callback_delete_request_set(ee, _minictrl_plug_server_del);

	evas_object_event_callback_add(plug, EVAS_CALLBACK_DEL,
			_minictrl_plug_del, ee);

	return plug;
}

/* LCOV_EXCL_START */
EXPORT_API Evas_Object *minicontrol_viewer_image_object_get(
		const Evas_Object *obj)
{
	return elm_plug_image_object_get(obj);
}
/* LCOV_EXCL_STOP */

/* LCOV_EXCL_START */
EXPORT_API int minicontrol_viewer_request(const char *minicontrol_name,
		minicontrol_request_e request, int value)
{
	minicontrol_viewer_event_e event = MINICONTROL_EVENT_REPORT_ANGLE;
	bundle *event_arg_bundle;
	char bundle_value_buffer[BUNDLE_BUFFER_LENGTH];

	if (minicontrol_name == NULL) {
		ERR("appid is NULL, invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (request != MINICONTROL_REQ_ROTATE_PROVIDER)
		return MINICONTROL_ERROR_INVALID_PARAMETER;

	event_arg_bundle = bundle_create();
	if (event_arg_bundle == NULL) {
		ERR("fail to create a bundle instance");
		return MINICONTROL_ERROR_OUT_OF_MEMORY;
	}

	snprintf(bundle_value_buffer, sizeof(bundle_value_buffer), "%d", value);
	bundle_add_str(event_arg_bundle, "angle", bundle_value_buffer);
	_minictrl_send_event(MINICTRL_DBUS_SIG_TO_PROVIDER, minicontrol_name,
			event, event_arg_bundle);
	bundle_free(event_arg_bundle);

	return MINICONTROL_ERROR_NONE;
}
/* LCOV_EXCL_STOP */

