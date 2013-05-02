/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
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

#ifndef _MINICTRL_LOG_H_
#define _MINICTRL_LOG_H_

#include <stdio.h>
#include <string.h>

#ifdef MINICTRL_USE_DLOG
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "libminicontrol"
#define DBG(fmt , args...) \
	do{ \
		LOGD("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#define INFO(fmt , args...) \
	do{ \
		LOGI("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#define WARN(fmt , args...) \
	do{ \
		LOGI("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
	} while(0)

#define ERR(fmt , args...) \
	do{ \
		LOGI("[%s : %d] "fmt"\n",__func__,__LINE__,##args ); \
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

