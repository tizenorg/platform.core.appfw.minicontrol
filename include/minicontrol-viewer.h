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

#ifndef _MINICTRL_VIEWER_H_
#define _MINICTRL_VIEWER_H_

#include <Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MINICONTROL_VIEWER_LIBRARY minicontrol provider library
 * @brief This minicontrol viewer library used to display minicontrol which created by minicontrol provider
 */

/**
 * @addtogroup MINICONTROL_VIEWER_LIBRARY
 * @{
 */

/**
 * @brief This function add minicontrol named as "svr_name" to given parent evas object then return it
 * @param[in] parent minicontrol object will be added to this parent evas object
 * @param[in] svr_name name of minicontrol
 * @return evas object of minicontrol
 */
Evas_Object *minicontrol_viewer_add(Evas_Object *parent, const char *svr_name);

/**
 * @brief Get the basic evas image object from given minicontrol object
 * @param[in] obj minicontrol object
 * @return basic evas image object of minicontrol object
 */
Evas_Object *minicontrol_viewer_image_object_get(const Evas_Object *obj);

#ifdef __cplusplus
}
#endif
#endif /* _MINICTRL_VIEWER_H_ */

