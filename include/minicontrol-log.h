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

#ifndef _MINICTRL_LOG_H_
#define _MINICTRL_LOG_H_

#include <stdio.h>
#include <string.h>

#ifdef MINICTRL_USE_DLOG
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "MINICONTROL"
#define DBG(fmt , args...) \
	do{ \
		SECURE_LOGD("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#define INFO(fmt , args...) \
	do{ \
		SECURE_LOGI("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#define WARN(fmt , args...) \
	do{ \
		SECURE_LOGW("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#define ERR(fmt , args...) \
	do{ \
		SECURE_LOGE("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#else /* MINICTRL_USE_DLOG */
#define DBG(fmt , args...) \
	do{ \
		printf("[D][%s : %d] "fmt"\n", __func__,__LINE__,##args ); \
	} while(0)

#define INFO(fmt , args...) \
	do{ \
		printf("[I][%s : %d] "fmt"\n", __func__,__LINE__,##args ); \
	} while(0)

#define WARN(fmt , args...) \
	do{ \
		printf("[W][%s : %d] "fmt"\n", __func__,__LINE__,##args ); \
	} while(0)

#define ERR(fmt , args...) \
	do{ \
		printf("[E][%s : %d] "fmt"\n", __func__,__LINE__,##args ); \
	} while(0)

#endif /* MINICTRL_USE_DLOG */

#endif /* _MINICTRL_LOG_H_ */

