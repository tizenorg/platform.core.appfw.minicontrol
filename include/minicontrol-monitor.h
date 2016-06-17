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

#ifndef _MINICTRL_MONITOR_H_
#define _MINICTRL_MONITOR_H_

#include <minicontrol-error.h>
#include <minicontrol-type.h>
#include <minicontrol-internal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*minicontrol_monitor_cb)(minicontrol_action_e action,
		const char *name, unsigned int width, unsigned int height,
		minicontrol_priority_e priority, void *data);

int minicontrol_monitor_start(minicontrol_monitor_cb callback, void *data) DEPRECATED;

int minicontrol_monitor_stop(void) DEPRECATED;

#ifdef __cplusplus
}
#endif
#endif /* _MINICTRL_MONITOR_H_ */

