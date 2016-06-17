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

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <bundle.h>

#include "minicontrol-error.h"
#include "minicontrol-type.h"
#include "minicontrol-internal.h"
#include "minicontrol-log.h"

#define MINICTRL_DBUS_PATH "/org/tizen/minicontrol"
#define MINICTRL_DBUS_INTERFACE "org.tizen.minicontrol.signal"

#define PROC_DBUS_OBJECT	"/Org/Tizen/ResourceD/Process"
#define PROC_DBUS_INTERFACE	"org.tizen.resourced.process"
#define PROC_DBUS_METHOD	"ProcExclude"
#define PROC_DBUS_EXCLUDE	"exclude"
#define PROC_DBUS_INCLUDE	"include"

struct _minictrl_sig_handle {
	GDBusConnection *conn;
	guint s_id;
	void (*callback)(void *data, GVariant *parameters);
	void *user_data;
	char *signal;
};

static int __send_signal(const char *object_path, const char *interface_name,
		const char *signal_name, GVariant *parameters)
{
	GError *err = NULL;
	GDBusConnection *conn;
	gboolean ret;

	conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);
	if (conn == NULL) {
		ERR("g_bus_get_sync() failed. %s", err->message);
		g_error_free(err);
		return MINICONTROL_ERROR_IPC_FAILURE;
	}

	ret = g_dbus_connection_emit_signal(conn, NULL, object_path,
			interface_name, signal_name, parameters, &err);
	if (!ret) {
		ERR("g_dbus_connection_emit_signal() failed. %s", err->message);
		g_error_free(err);
		g_object_unref(conn);
		return MINICONTROL_ERROR_IPC_FAILURE;
	}

	g_dbus_connection_flush_sync(conn, NULL, &err);
	g_object_unref(conn);
	g_clear_error(&err);

	return MINICONTROL_ERROR_NONE;
}

int _minictrl_viewer_req_message_send(void)
{
	int ret;

	ret = __send_signal(MINICTRL_DBUS_PATH, MINICTRL_DBUS_INTERFACE,
			MINICTRL_DBUS_SIG_RUNNING_REQ, NULL);

	return ret;
}

int _minictrl_provider_proc_send(int type)
{
	int ret;
	GVariant *param;
	const char *type_str = PROC_DBUS_INCLUDE;
	int pid = getpid();

	if (type == MINICONTROL_DBUS_PROC_EXCLUDE)
		type_str = PROC_DBUS_EXCLUDE;

	DBG("pid: %d, type: %d(%s)", pid, type, type_str);

	param = g_variant_new("(si)", type_str, pid);
	if (param == NULL) {
		ERR("out of memory");
		return MINICONTROL_ERROR_OUT_OF_MEMORY;
	}

	ret = __send_signal(PROC_DBUS_OBJECT, PROC_DBUS_INTERFACE,
			PROC_DBUS_METHOD, param);

	return ret;
}

int _minictrl_send_event(const char *signal_name, const char *minicontrol_name,
		int event, bundle *signal_arg)
{
	int ret;
	bundle_raw *serialized_arg = NULL;
	unsigned int serialized_arg_length = 0;
	GVariant *param;

	if (minicontrol_name == NULL || signal_name == NULL) {
		ERR("Invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	if (signal_arg) {
		ret = bundle_encode(signal_arg, &serialized_arg,
				(int *)&serialized_arg_length);
		if (ret != BUNDLE_ERROR_NONE) {
			ERR("Failed to serialize bundle argument");
			return MINICONTROL_ERROR_OUT_OF_MEMORY;
		}
	} else {
		serialized_arg = (bundle_raw *)strdup("");
		if (serialized_arg == NULL) {
			ERR("out of memory");
			return MINICONTROL_ERROR_OUT_OF_MEMORY;
		}
		serialized_arg_length = 0;
	}

	param = g_variant_new("(sisu)", minicontrol_name, event,
			serialized_arg, serialized_arg_length);
	if (param == NULL) {
		ERR("out of memory");
		free(serialized_arg);
		return MINICONTROL_ERROR_OUT_OF_MEMORY;
	}

	ret = __send_signal(MINICTRL_DBUS_PATH, MINICTRL_DBUS_INTERFACE,
			signal_name, param);

	free(serialized_arg);

	return ret;
}

int _minictrl_provider_message_send(int event, const char *minicontrol_name,
		unsigned int witdh, unsigned int height,
		minicontrol_priority_e priority)
{
	int ret;
	bundle *event_arg_bundle;
	char bundle_value_buffer[BUNDLE_BUFFER_LENGTH];

	event_arg_bundle = bundle_create();
	if (event_arg_bundle == NULL) {
		ERR("Fail to create a bundle instance");
		return MINICONTROL_ERROR_OUT_OF_MEMORY;
	}

	snprintf(bundle_value_buffer, sizeof(bundle_value_buffer),
			"%s", minicontrol_name);

	bundle_add_str(event_arg_bundle, "minicontrol_name",
			bundle_value_buffer);
	bundle_add_byte(event_arg_bundle, "width", (void *)&witdh, sizeof(int));
	bundle_add_byte(event_arg_bundle, "height", (void *)&height,
			sizeof(int));
	bundle_add_byte(event_arg_bundle, "priority", (void *)&priority,
			sizeof(int));

	ret = _minictrl_send_event(MINICTRL_DBUS_SIG_TO_VIEWER,
			minicontrol_name, event, event_arg_bundle);

	return ret;
}

static void __minictrl_signal_filter(GDBusConnection *connection,
		const gchar *sender_name, const gchar *object_path,
		const gchar *interface_name, const gchar *signal_name,
		GVariant *parameters, gpointer user_data)
{
	minictrl_sig_handle *handle = (minictrl_sig_handle *)user_data;

	if (handle == NULL)
		return;

	if (g_strcmp0(signal_name, handle->signal) == 0) {
		if (handle->callback)
			handle->callback(handle->user_data, parameters);
	}
}

minictrl_sig_handle *_minictrl_dbus_sig_handle_attach(const char *signal,
		void (*callback)(void *data, GVariant *parameters), void *data)
{
	GError *err = NULL;
	minictrl_sig_handle *handle;

	if (signal == NULL || callback == NULL) {
		ERR("Invalid prameter");
		return NULL;
	}

	handle = (minictrl_sig_handle *)malloc(sizeof(minictrl_sig_handle));
	if (handle == NULL) {
		ERR("out of memory");
		return NULL;
	}

	handle->conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);
	if (handle->conn == NULL) {
		ERR("g_bus_get_sync() failed. %s", err->message);
		g_error_free(err);
		free(handle);
		return NULL;
	}

	handle->s_id = g_dbus_connection_signal_subscribe(handle->conn,
			NULL, MINICTRL_DBUS_INTERFACE, signal,
			MINICTRL_DBUS_PATH, NULL, G_DBUS_SIGNAL_FLAGS_NONE,
			__minictrl_signal_filter, handle, NULL);
	if (handle->s_id == 0) {
		ERR("g_dbus_connection_signal_subscribe() failed.");
		g_object_unref(handle->conn);
		free(handle);
		return NULL;
	}

	handle->callback = callback;
	handle->user_data = data;
	handle->signal = strdup(signal);

	INFO("success to attach signal[%s]-[%p, %p]", signal, callback, data);
	g_clear_error(&err);

	return handle;
}

void _minictrl_dbus_sig_handle_dettach(minictrl_sig_handle *handle)
{
	if (!handle) {
		ERR("handle is NULL");
		return;
	}

	g_dbus_connection_signal_unsubscribe(handle->conn, handle->s_id);
	g_object_unref(handle->conn);
	free(handle->signal);
	free(handle);
}

