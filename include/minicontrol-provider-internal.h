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

#ifndef _MINICONTROL_PROVIDER_INTERNAL_H_
#define _MINICONTROL_PROVIDER_INTERNAL_H_

#include <Evas.h>
#include "minicontrol-error.h"
#include "minicontrol-type.h"

#ifdef __cplusplus
extern "C" {
#endif

Evas_Object *minicontrol_win_add(const char *name) DEPRECATED;

int minicontrol_request(Evas_Object *mincontrol, minicontrol_request_e request) DEPRECATED;

#ifdef __cplusplus
}
#endif
#endif /* _MINICONTROL_PROVIDER_INTERNAL_H_ */
