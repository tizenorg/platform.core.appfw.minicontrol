/*
 * minicontrol-viewer-internal.h
 *
 *  Created on: 2015. 4. 13.
 *      Author: kyuho.jo
 */

#ifndef _MINICONTROL_VIEWER_INTERNAL_H_
#define _MINICONTROL_VIEWER_INTERNAL_H_

int minicontrol_viewer_request(const char *appid, minicontrol_request_e request, int value) DEPRECATED;

Evas_Object *minicontrol_viewer_image_object_get(const Evas_Object *obj) DEPRECATED;

#endif /* _MINICONTROL_VIEWER_INTERNAL_H_ */
