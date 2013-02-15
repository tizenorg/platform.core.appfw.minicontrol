/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#include <Elementary.h>
#include <Ecore_Evas.h>

#include "minicontrol-internal.h"
#include "minicontrol-type.h"
#include "minicontrol-viewer.h"
#include "minicontrol-log.h"

#define MINICTRL_PLUG_DATA_KEY "__minictrl_plug_name"

static void _minictrl_plug_server_del(Ecore_Evas *ee)
{
	char *svr_name = NULL;

	svr_name = ecore_evas_data_get(ee, MINICTRL_PLUG_DATA_KEY);
	if (!svr_name) {
		ERR("fail to get svr_name");
		return;
	}

	INFO("server - %s is deleted", svr_name);

	/* send message to remve plug */
	_minictrl_provider_message_send(MINICTRL_DBUS_SIG_STOP,
					svr_name, 0, 0,
					MINICONTROL_PRIORITY_LOW);
}

static void _minictrl_plug_del(void *data, Evas *e,
			Evas_Object *obj, void *event_info)
{
	Ecore_Evas *ee = NULL;
	char *svr_name = NULL;

	ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));
	if (!ee)
		return;

	svr_name = ecore_evas_data_get(ee, MINICTRL_PLUG_DATA_KEY);
	if (svr_name)
		free(svr_name);

	ecore_evas_data_set(ee, MINICTRL_PLUG_DATA_KEY, NULL);
}

EXPORT_API
Evas_Object *minicontrol_viewer_image_object_get(const Evas_Object *obj)
{
	return elm_plug_image_object_get(obj);
}

EXPORT_API Evas_Object *minicontrol_viewer_add(Evas_Object *parent,
						const char *svr_name)
{
	Evas_Object *plug = NULL;
	Evas_Object *plug_img = NULL;
	Ecore_Evas *ee = NULL;

	plug = elm_plug_add(parent);
	if (!plug) {
		ERR("fail to create plug");
		return NULL;
	}

	if (!elm_plug_connect(plug, svr_name, 0, EINA_FALSE)) {
		ERR("Cannot connect plug[%s]", svr_name);
		evas_object_del(plug);
		return NULL;
	}

	plug_img = elm_plug_image_object_get(plug);

	ee = ecore_evas_object_ecore_evas_get(plug_img);
	ecore_evas_data_set(ee, MINICTRL_PLUG_DATA_KEY, strdup(svr_name));
	ecore_evas_callback_delete_request_set(ee, _minictrl_plug_server_del);

	evas_object_event_callback_add(plug, EVAS_CALLBACK_DEL,
					_minictrl_plug_del, plug);

	return plug;
}

