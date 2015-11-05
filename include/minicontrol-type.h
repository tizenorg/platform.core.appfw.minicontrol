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

#ifndef _MINICTRL_TYPE_H_
#define _MINICTRL_TYPE_H_

/**
 * @file minicontrol-type.h
 * @brief Minicontrol library type.
 */

/**
 * @addtogroup MINICONTROL_LIBRARY
 * @{
 */

#ifndef DEPRECATED
#define DEPRECATED                        __attribute__((deprecated))
#endif /* DEPRECATED */

/**
@section MINICONTROL_LIBRARY_EVENTS Events

<TABLE>
<TR>
<TD>Event Type</TD><TD>Originated by</TD>
<TD>Argument 1 Key</TD><TD>Argument 1 Type</TD>
<TD>Argument 2 Key</TD><TD>Argument 2 Type</TD>
<TD>Argument 3 Key</TD><TD>Argument 3 Type</TD>
</TR>
<TR>
<TD>MINICONTROL_EVENT_START</TD><TD>provider</TD>
<TD>minicontrol_name</TD><TD>string</TD>
<TD>width</TD><TD>integer</TD>
<TD>height</TD><TD>integer</TD>
</TR>
<TR>
<TD>MINICONTROL_EVENT_STOP</TD><TD>provider</TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
</TR>
<TR>
<TD>MINICONTROL_EVENT_RESIZE</TD><TD>provider</TD>
<TD>minicontrol_name</TD><TD>string</TD>
<TD>width</TD><TD>integer</TD>
<TD>height</TD><TD>integer</TD>
</TR>
<TR>
<TD>MINICONTROL_EVENT_REQUEST_HIDE</TD><TD>provider</TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
</TR>
<TR>
<TD>MINICONTROL_EVENT_REQUEST_ANGLE</TD><TD>provider</TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
</TR>
<TR>
<TD>MINICONTROL_EVENT_REPORT_ANGLE</TD><TD>viewer</TD>
<TD>angle</TD><TD>string</TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
</TR>
<TR>
<TD>MINICONTROL_VIEWER_EVENT_SHOW</TD><TD>viewer</TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
</TR>
<TR>
<TD>MINICONTROL_VIEWER_EVENT_HIDE</TD><TD>viewer</TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
<TD></TD><TD></TD>
</TR>
</TABLE>
*/

/**
 * @brief Enumerations for describing types of events originated by a minicontrol provider.
 * @since_tizen 2.4
 */
typedef enum {
	MINICONTROL_EVENT_START,               /**< A minicontrol object is created */
	MINICONTROL_EVENT_STOP,                /**< A minicontrol object is deleted */
	MINICONTROL_EVENT_RESIZE,              /**< A minicontrol object is resized */
	MINICONTROL_EVENT_REQUEST_HIDE  = 100, /**< Requests that the minicontrol viewer(s) close the provider's minicontrol */
	MINICONTROL_EVENT_REQUEST_ANGLE = 103, /**< Requests the current angle of the minicontrol viewer */
	MINICONTROL_EVENT_REPORT_ANGLE  = 200, /**< Reports the current angle of the minicontrol viewer */
} minicontrol_event_e;

/**
 * @brief Enumerations for types of events which will be sent by minicontrol_send_event.
 * @since_tizen 2.4
 */
typedef enum {
	MINICONTROL_PROVIDER_EVENT_REQUEST_HIDE  = MINICONTROL_EVENT_REQUEST_HIDE,  /**< Requests that the minicontrol viewer(s) close the provider's minicontrol. Required bundle argument : NULL. */
	MINICONTROL_PROVIDER_EVENT_REQUEST_ANGLE = MINICONTROL_EVENT_REQUEST_ANGLE, /**< Requests the current angle of the minicontrol viewer. Required bundle argument : NULL. */
} minicontrol_provider_event_e;

/**
 * @brief Enumerations for types of events which will be sent by minicontrol_viewer_send_event.
 * @since_tizen 2.4
 */
typedef enum {
	MINICONTROL_VIEWER_EVENT_REPORT_ANGLE    = MINICONTROL_EVENT_REPORT_ANGLE,  /**< Reports the current angle of the minicontrol viewer. Required argument : Angle of minicontrol viewer, Bundle Key : "angle", Bundle Value Type : string.*/
	MINICONTROL_VIEWER_EVENT_SHOW            = 201, /**< The minicontrol viewer is shown. Required argument : NULL.*/
	MINICONTROL_VIEWER_EVENT_HIDE            = 202, /**< The minicontrol viewer is hidden. Required argument : NULL.*/
} minicontrol_viewer_event_e;

/**
 * @brief Enumerations for selecting target viewers.
 * @since_tizen 2.4
 */
typedef enum {
	MINICONTROL_TARGET_VIEWER_QUICK_PANEL             = 0x0001,   /**< Request to place the minicontrol on the quickpanel */
	MINICONTROL_TARGET_VIEWER_STOCK_LOCK_SCREEN       = 0x0002,   /**< Request to place the minicontrol on the stock lock screen */
	MINICONTROL_TARGET_VIEWER_CUSTOM_LOCK_SCREEN      = 0x0004,   /**< Request to place the minicontrol on a lock screen replacement application */
} minicontrol_target_viewer_e;

/**
 * @}
 */

#endif /* _MINICTRL_TYPE_H_ */
