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

#ifndef _MINICTRL_PROVIDER_H_
#define _MINICTRL_PROVIDER_H_

#include <Evas.h>
#include "minicontrol-error.h"
#include "minicontrol-type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MINICONTROL_PROVIDER_LIBRARY minicontrol provider library
 * @brief This minicontrol provider library used to create evas socket window
 */

/**
 * @addtogroup MINICONTROL_PROVIDER_LIBRARY
 * @{
 */

/**
 * @brief This function create evas socket window
 * @param[in] name name of socket window
 * @return evas object of socket window
 */
Evas_Object *minicontrol_win_add(const char *name);

/**
 * @brief This function request various action to viewer
 * @param[in] minicontrol evas object of socket window
 * @param[in] action type of action
 * @return evas object of socket window
 */
minicontrol_error_e minicontrol_request(Evas_Object *mincontrol, minicontrol_request_e request);

#ifdef __cplusplus
}
#endif
#endif /* _MINICTRL_PROVIDER_H_ */

