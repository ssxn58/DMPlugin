#ifndef __SCRIPT_H__
#define __SCRIPT_H__



typedef struct
{
	HANDLE thread_handle;    // 线程句柄
	int *  is_run;           // 线程结束标志
	Idmsoft *dm;             // 线程用到的dm对象
}ScriptInfo;





ScriptInfo* StartOneScript(void);

BOOL        StopOneScript(ScriptInfo*  script_info);

BOOL        StopAllScript(vector<ScriptInfo*> & all_script_info);




#endif