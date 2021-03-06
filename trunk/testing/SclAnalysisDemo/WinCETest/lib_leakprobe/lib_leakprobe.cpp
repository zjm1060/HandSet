//
//=============================================================================
// 实现泄露探测的总入口代码
//=============================================================================
//

#include "lib_leakprobe.h"


//
//-----------------------------------------------------------------------------
// 开始探测各种泄露
//-----------------------------------------------------------------------------
//
void  Lkp_StartProbe ()
{
//	Lkp_StartProbeMemLeak();
// 	Lkp_BreakAtAllocCount(98);
// 	Lkp_BreakAtAllocHandle((void*)0x6A4620);

	Lkp_StartProbeMemUsage();
}

//
//-----------------------------------------------------------------------------
// 结束探测各种泄露
//-----------------------------------------------------------------------------
//
void  Lkp_EndProbe ()
{
//	Lkp_EndProbeMemLeak();
    
	Lkp_EndProbeMemUsage();
}




//
//-----------------------------------------------------------------------------
// 自动启动泄漏检测辅助类, 并保证开始和结束顺序在运行时库的级别
//-----------------------------------------------------------------------------
//
#pragma init_seg(lib)
class CLeakProbeHelper
{
public:
    CLeakProbeHelper ()
    {
        Lkp_StartProbe();
    }
    ~CLeakProbeHelper ()
    {
        Lkp_EndProbe ();
    }
} g_leakerHelper;