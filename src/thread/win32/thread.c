/* ***************************************************************************
 * thread.c -- the win32 edition thread opreation set.
 * 
 * Copyright (C) 2013-2015 by
 * Liu Chao
 * 
 * This file is part of the LCUI project, and may only be used, modified, and
 * distributed under the terms of the GPLv2.
 * 
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 * 
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *  
 * The LCUI project is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for more details.
 * 
 * You should have received a copy of the GPLv2 along with this file. It is 
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/
 
/* ****************************************************************************
 * thread.c -- win32版的线程操作集
 *
 * 版权所有 (C) 2013-2015 归属于
 * 刘超
 * 
 * 这个文件是LCUI项目的一部分，并且只可以根据GPLv2许可协议来使用、更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 * 
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 * 
 * LCUI 项目是基于使用目的而加以散布的，但不负任何担保责任，甚至没有适销性或特
 * 定用途的隐含担保，详情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>. 
 * ****************************************************************************/

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/thread.h>

#ifdef LCUI_THREAD_WIN32
#include <process.h>
#include <windows.h>

typedef struct _LCUI_ThreadData {
	HANDLE handle;
	unsigned int tid;
	void *retval;
	void (*func)(void*);
	void *arg;
} LCUI_ThreadData;

static LCUI_BOOL db_init = FALSE;
static LinkedList thread_database;

static unsigned __stdcall run_thread(void *arg)
{
	LCUI_ThreadData *thread;
	thread = (LCUI_ThreadData*)arg;
	thread->func( thread->arg );
	return 0;
}

int LCUIThread_Create( LCUI_Thread *thread, void(*func)(void*), void *arg )
{
	LCUI_ThreadData *thread_ptr;
	if(!db_init) {
		db_init = TRUE;
		LinkedList_Init( &thread_database );
	}
	thread_ptr = NEW(LCUI_ThreadData, 1);
	thread_ptr->func = func;
	thread_ptr->arg = arg;
	thread_ptr->retval = NULL;
	thread_ptr->handle = (HANDLE)_beginthreadex( NULL, 0, run_thread, 
					thread_ptr, 0, &thread_ptr->tid );
	if( thread_ptr->handle == 0 ) {
		*thread = 0;
		return -1;
	}
	LinkedList_Append( &thread_database, thread_ptr );
	*thread = thread_ptr->tid;
	return 0;
}

static LCUI_ThreadData *LCUIThread_Find( LCUI_Thread tid )
{
	LinkedListNode *node;
	LCUI_ThreadData *thread_data;
	for( LinkedList_Each( node, &thread_database ) ) {
		thread_data = (LCUI_ThreadData*)node->data;
		if( thread_data && thread_data->tid == tid ) {
			return thread_data;
		}
	}
	return NULL;
}

static int LCUIThread_Destroy( LCUI_Thread thread )
{
	LinkedListNode *node;
	LCUI_ThreadData *thread_data;
	if(!thread){
		return -1;
	}
	for( LinkedList_Each( node, &thread_database ) ) {
		thread_data = (LCUI_ThreadData*)node->data;
		if( thread_data && thread_data->tid == thread ) {
			LinkedList_DeleteNode( &thread_database, node );
			return 0;
		}
	}
	return -2;
}

LCUI_Thread LCUIThread_SelfID( void )
{
	return GetCurrentThreadId();
}

void LCUIThread_Exit( void *retval )
{
	LCUI_ThreadData *thread;
	LCUI_Thread tid;

	tid = LCUIThread_SelfID();
	thread = LCUIThread_Find( tid );
	if( !thread ) {
		return;
	}
	thread->retval = retval;
}

void LCUIThread_Cancel( LCUI_Thread thread )
{
	LCUI_ThreadData *data_ptr;
	data_ptr = LCUIThread_Find(thread);
	TerminateThread( data_ptr->handle, FALSE );
	LCUIThread_Destroy( data_ptr->tid );
}

int LCUIThread_Join( LCUI_Thread thread, void **retval )
{
	LCUI_ThreadData *data_ptr;

	data_ptr = LCUIThread_Find( thread );
	if( data_ptr == NULL ) {
		return -1;
	}
	WaitForSingleObject( data_ptr->handle, INFINITE );
	CloseHandle( data_ptr->handle );
	if( retval ) {
		*retval = data_ptr->retval;
	}
	return LCUIThread_Destroy( data_ptr->tid );
}
#endif
