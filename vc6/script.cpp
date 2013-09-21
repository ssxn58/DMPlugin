#include "StdAfx.h"


static void script_main(Idmsoft & dm,int * is_run)
{
	// 执行脚本函数 这里我就弹个版本号
	MessageBox(NULL,dm.Ver(),"",MB_OK);

	Sleep(1000);
}
static unsigned WINAPI EntryScript(LPVOID pParam)
{
    ScriptInfo *script_info  = (ScriptInfo *)pParam;


	// 不要在线程里创建dm对象.除非你真的能控制线程.

	// 开始脚本循环
	while (1)
	{
		// 检测脚本是否要结束
		if (*script_info->is_run == 0)
			break;
		
		script_main(*script_info->dm,script_info->is_run);
	}


	return 1;
	
}

ScriptInfo * malloc_script_info(void)
{
	ScriptInfo *script_info = (ScriptInfo *)malloc(sizeof(ScriptInfo));
	if (script_info == NULL)
		return NULL;
	

	script_info->is_run = (int *)malloc(sizeof(int));
	if (script_info->is_run == NULL)
	{
		free(script_info);
		return NULL;
	}
	
	script_info->dm = new Idmsoft;
	if (script_info->dm == NULL)
	{
		free(script_info->is_run);
		free(script_info);
		return NULL;
	}

	return script_info;
}

void free_script_info(ScriptInfo * script_info)
{
	if (script_info)
	{
		if (script_info->is_run)
			free(script_info->is_run);

		if (script_info->dm)
			delete script_info->dm;
		
		free(script_info);
	}
}
ScriptInfo*  StartOneScript(void)
{
	// 分配空间
	ScriptInfo *script_info = malloc_script_info();

	if (script_info == NULL)
		return NULL;
	
	

	// 创建大漠对象, 必须在线程外创建，避免线程被强制关闭，导致绑定窗口挂掉
	if (script_info->dm->CreateDispatch("dm.dmsoft") == FALSE)
	{
		free_script_info(script_info);
		return NULL;
	}
	
	// 设置为启动状态
	*script_info->is_run = 1;

	// 创建线程
	unsigned thread_id = 0;
	script_info->thread_handle = (HANDLE)_beginthreadex(NULL,0,EntryScript,(LPVOID)script_info,0,(unsigned *)&thread_id);
	
	if (script_info->thread_handle == INVALID_HANDLE_VALUE)
	{
		free_script_info(script_info);
		return NULL;
	}
	
	return script_info;
}

BOOL        StopOneScript(ScriptInfo  *script_info)
{
	// 设置标志位
	*script_info->is_run = 0;
	// 等待最多3秒,如果超时,就强制结束线程.危险.(有资源泄漏)
	if (WAIT_OBJECT_0 != WaitForSingleObject(script_info->thread_handle,3000))
	{
		// 先解绑窗口. 避免窗口崩溃
		script_info->dm->UnBindWindow();
		TerminateThread(script_info->thread_handle,0);
	}
	CloseHandle(script_info->thread_handle);


	free_script_info(script_info);
	
	return TRUE;
}

BOOL        StopAllScript(vector<ScriptInfo *> & all_script_info)
{
	// 先把所有标志位置0
	int i;
	for (i = 0;i < all_script_info.size();++i)
	{
		*all_script_info[i]->is_run = 0;
	}
	
	// 等待所有
	for (i = 0;i < all_script_info.size();++i)
	{
		// 等待最多3秒,如果超时,就强制结束线程.危险.(有资源泄漏)
		if (WAIT_OBJECT_0 != WaitForSingleObject(all_script_info[i]->thread_handle,3000))
		{
			// 先解绑窗口. 避免窗口崩溃
			all_script_info[i]->dm->UnBindWindow();
			TerminateThread(all_script_info[i]->thread_handle,0);
		}
		CloseHandle(all_script_info[i]->thread_handle);
		free_script_info(all_script_info[i]);
	}
	
	all_script_info.clear();
	return TRUE;
}