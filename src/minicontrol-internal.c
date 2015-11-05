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
#include <dbus/dbus-glib-lowlevel.h>
#include <bundle.h>

#include "minicontrol-error.h"
#include "minicontrol-type.h"
#include "minicontrol-internal.h"
#include "minicontrol-log.h"

#define MINICTRL_DBUS_PATH "/org/tizen/minicontrol"
#define MINICTRL_DBUS_INTERFACE "org.tizen.minicontrol.signal"

#define PROC_DBUS_OBJECT 	"/Org/Tizen/ResourceD/Process"
#define PROC_DBUS_INTERFACE 	"org.tizen.resourced.process"
#define PROC_DBUS_METHOD 	"ProcExclude"
#define PROC_DBUS_EXCLUDE	"exclude"
#define PROC_DBUS_INCLUDE	"include"

struct _minictrl_sig_handle {
	DBusConnection *conn;
	void (*callback) (void *data, DBusMessage *msg);
	void *user_data;
	char *signal;
};

int _minictrl_viewer_req_message_send(void)
{
	DBusConnection *connection = NULL;
	DBusMessage *message = NULL;
	DBusError err;
	dbus_bool_t dbus_ret;
	int ret = MINICONTROL_ERROR_NONE;

	dbus_error_init(&err);
	connection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (!connection) {
		ERR("Fail to dbus_bus_get : %s", err.message);
		ret = MINICONTROL_ERROR_IPC_FAILURE;
		goto release_n_return;
	}

	message = dbus_message_new_signal(MINICTRL_DBUS_PATH,
				MINICTRL_DBUS_INTERFACE,
				MINICTRL_DBUS_SIG_RUNNING_REQ);
	if (!message) {
		ERR("fail to create dbus message");
		ret = MINICONTROL_ERROR_OUT_OF_MEMORY;
		goto release_n_return;
	}

	dbus_ret = dbus_connection_send(connection, message, NULL);
	if (!dbus_ret) {
		ERR("fail to send dbus viewer req message");
		ret = MINICONTROL_ERROR_IPC_FAILURE;
		goto release_n_return;
	}

	dbus_connection_flush(connection);

release_n_return:
	dbus_error_free(&err);

	if (message)
		dbus_message_unref(message);

	if (connection)
		dbus_connection_unref(connection);

	return ret;
}

int _minictrl_provider_proc_send(int type)
{
	DBusError err;
	DBusConnection* conn = NULL;
	DBusMessage* msg = NULL;
	int ret = -1;
	int pid = getpid();
	dbus_uint32_t serial = 0;
	char *typestr;
	if (type == MINICONTROL_DBUS_PROC_EXCLUDE)
		typestr = PROC_DBUS_EXCLUDE;
	else if  (type == MINICONTROL_DBUS_PROC_INCLUDE)
		typestr = PROC_DBUS_INCLUDE;
	else {
		ERR("Check unsupported type : %d", type);
		return ret;
	}
	DBG("_minictrl_provider_proc_send : %d, %d", pid, type);
	dbus_error_init(&err);
	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (!conn) {
		ERR("Fail to dbus_bus_get : %s", err.message);
		ret = MINICONTROL_ERROR_IPC_FAILURE;
		goto release_n_return;
	}
	msg = dbus_message_new_signal(PROC_DBUS_OBJECT, // object name of the signal
			    PROC_DBUS_INTERFACE, // interface name of the signal
			    PROC_DBUS_METHOD); // name of the signal
	if (!msg) {
		ERR("ERR Could not create DBus Message");
		goto release_n_return;
	}
	ret = dbus_message_append_args(msg,
			    DBUS_TYPE_STRING, &typestr,
			    DBUS_TYPE_INT32, &pid,
			    DBUS_TYPE_INVALID);
	if (!dbus_connection_send(conn, msg, &serial))
		ERR("ERR send DBus Message");
	dbus_connection_flush(conn);
release_n_return:
	dbus_error_free(&err);

	if (msg)
		dbus_message_unref(msg);

	if (conn)
		dbus_connection_unref(conn);

	return ret;

}

int _minictrl_send_event(const char *signal_name, const char *minicontrol_name, int event, bundle *signal_arg)
{
	DBusConnection *connection = NULL;
	DBusMessage *message = NULL;
	DBusError err;
	dbus_bool_t dbus_ret;
	bundle_raw *serialized_arg = NULL;
	unsigned int serialized_arg_length = 0;
	int ret = MINICONTROL_ERROR_NONE;

	if (minicontrol_name == NULL) {
		ERR("Invaild parameter");
		return MINICONTROL_ERROR_INVALID_PARAMETER;
	}

	dbus_error_init(&err);
	connection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (!connection) {
		ERR("Fail to dbus_bus_get : %s", err.message);
		ret = MINICONTROL_ERROR_IPC_FAILURE;
		goto release_n_return;
	}

	message = dbus_message_new_signal(MINICTRL_DBUS_PATH, MINICTRL_DBUS_INTERFACE, signal_name);

	if (!message) {
		ERR("fail to create dbus message");
		ret = MINICONTROL_ERROR_OUT_OF_MEMORY;
		goto release_n_return;
	}

	if (signal_arg != NULL) {
		if (bundle_encode(signal_arg, &serialized_arg, (int*)&serialized_arg_length) != BUNDLE_ERROR_NONE) {
			ERR("fail to serialize bundle argument");
			ret = MINICONTROL_ERROR_OUT_OF_MEMORY;
			goto release_n_return;
		}
	}
	else {
		serialized_arg = (bundle_raw*)strdup("");
		serialized_arg_length = 0;
	}

	dbus_ret = dbus_message_append_args(message,
			DBUS_TYPE_STRING, &minicontrol_name,
			DBUS_TYPE_INT32,  &event,
			DBUS_TYPE_STRING, &serialized_arg,
			DBUS_TYPE_UINT32,  &serialized_arg_length,
			DBUS_TYPE_INVALID);

	if (!dbus_ret) {
		ERR("fail to append arguments to dbus message : [%s][%d]", minicontrol_name, event);
		ret = MINICONTROL_ERROR_OUT_OF_MEMORY;
		goto release_n_return;
	}

	dbus_ret = dbus_connection_send(connection, message, NULL);

	if (!dbus_ret) {
		ERR("fail to send dbus message : %s", minicontrol_name);
		ret = MINICONTROL_ERROR_IPC_FAILURE;
		goto release_n_return;
	}

	dbus_connection_flush(connection);

release_n_return:
	dbus_error_free(&err);

	if (serialized_arg)
		free(serialized_arg);

	if (message)
		dbus_message_unref(message);

	if (connection)
		dbus_connection_unref(connection);

	return ret;
}


int _minictrl_provider_message_send(int event, const char *minicontrol_name, unsigned int witdh, unsigned int height, minicontrol_priority_e priority)
{
	bundle *event_arg_bundle = NULL;
	int ret = MINICONTROL_ERROR_NONE;
	char bundle_value_buffer[BUNDLE_BUFFER_LENGTH] = { 0, };

	event_arg_bundle = bundle_create();

	if (event_arg_bundle == NULL) {
		ERR("fail to create a bundle instance");
		ret = MINICONTROL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	snprintf(bundle_value_buffer, BUNDLE_BUFFER_LENGTH, "%s", minicontrol_name);

	bundle_add_str(event_arg_bundle, "minicontrol_name", bundle_value_buffer);
	bundle_add_byte(event_arg_bundle, "width", (void*)&witdh, sizeof(int));
	bundle_add_byte(event_arg_bundle, "height", (void*)&height, sizeof(int));
	bundle_add_byte(event_arg_bundle, "priority", (void*)&priority, sizeof(int));

	_minictrl_send_event(MINICTRL_DBUS_SIG_TO_VIEWER, minicontrol_name, event, event_arg_bundle);

out:
	if (event_arg_bundle)
		bundle_free(event_arg_bundle);

	return ret;
}

static DBusHandlerResult _minictrl_signal_filter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	minictrl_sig_handle *handle = NULL;
	const char *interface;
	DBusError error;
	dbus_bool_t ret;

	if (!user_data)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	handle = user_data;

	dbus_error_init(&error);

	interface = dbus_message_get_interface(msg);
	if (!interface)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (strcmp(MINICTRL_DBUS_INTERFACE, interface))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	ret = dbus_message_is_signal(msg, interface, handle->signal);
	if (!ret) {
		DBG("this msg is not signal");
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (handle->callback)
		handle->callback(handle->user_data, msg);

	return DBUS_HANDLER_RESULT_HANDLED;
}


minictrl_sig_handle *_minictrl_dbus_sig_handle_attach(const char *signal,
				void (*callback) (void *data, DBusMessage *msg),
				void *data)
{
	minictrl_sig_handle *handle = NULL;
	DBusError err;
	DBusConnection *conn = NULL;
	char rule[1024] = {'\0', };

	if (!signal) {
		ERR("signal is NULL");
		return NULL;
	}

	if (!callback) {
		ERR("call is NULL");
		return NULL;
	}

	handle = malloc(sizeof(minictrl_sig_handle));
	if (!handle) {
		ERR("fail to alloc handle");
		return NULL;
	}

	dbus_error_init(&err);
	conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
	if (!conn) {
		ERR("fail to get bus : %s", err.message);
		goto error_n_return;
	}

	dbus_connection_setup_with_g_main(conn, NULL);
	snprintf(rule, 1024,
		"path='%s',type='signal',interface='%s',member='%s'",
		MINICTRL_DBUS_PATH,
		MINICTRL_DBUS_INTERFACE,
		signal);

	dbus_bus_add_match(conn, rule, &err);
	if (dbus_error_is_set(&err)) {
		ERR("fail to dbus_bus_remove_match : %s",
				err.message);
		goto error_n_return;
	}

	if (dbus_connection_add_filter(conn, _minictrl_signal_filter,
					handle, NULL) == FALSE) {
		ERR("fail to dbus_connection_add_filter : %s",
				err.message);
		goto error_n_return;
	}

	dbus_connection_set_exit_on_disconnect(conn, FALSE);

	handle->conn = conn;
	handle->callback = callback;
	handle->user_data = data;
	handle->signal = strdup(signal);

	INFO("success to attach signal[%s]-[%p, %p]", signal, callback, data);

	return handle;


error_n_return:
	if (handle)
		free(handle);

	dbus_error_free(&err);

	if (conn)
		dbus_connection_close(conn);

	return NULL;
}

void _minictrl_dbus_sig_handle_dettach(minictrl_sig_handle *handle)
{
	DBusError err;
	char rule[1024] = {'\0', };

	if (!handle) {
		ERR("handle is NULL");
		return;
	}

	dbus_error_init(&err);

	dbus_connection_remove_filter(handle->conn, _minictrl_signal_filter, handle);

	snprintf(rule, 1024,
		"path='%s',type='signal',interface='%s',member='%s'",
		MINICTRL_DBUS_PATH,
		MINICTRL_DBUS_INTERFACE,
		handle->signal);

	dbus_bus_remove_match(handle->conn, rule, &err);
	if (dbus_error_is_set(&err)) {
		ERR("fail to dbus_bus_remove_match : %s", err.message);
		dbus_error_free(&err);
	}

	dbus_connection_close(handle->conn);

	free(handle->signal);
	free(handle);

	return;
}

