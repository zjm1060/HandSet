/// @file
///
/// @brief
///     SMV发送配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.24
///
/// 修改历史：
///

#include "StdAfx.h"
#include "ParamConfigSMVSend.h"
#include "src/utils/MessageCodec/include/sav91_msg.h"
#include "src/utils/MessageCodec/include/nanrui_ft3_msg.h"
#if !SHENG_ZE
#include "src/main/mgr/statusmgr.h"
#include "src/main/resourcestr.h"
#include "src/utils/res/dsm_stringmgr.h"
#endif

#define NODE_ROOT                       L"config"                       ///< 跟节点
#define NODE_SMVType                    L"SMVType"                      ///< SMV类型
#define NODE_SVDisplay                  L"SVDisplay"                    ///< 采样值显示
#define NODE_Frequency                  L"Frequency"                    ///< 采样频率
#define NODE_BuadRate                   L"BuadRate"                     ///< 波特率
#define NODE_ASDUCount                  L"ASDUCount"                    ///< ASDU数目
#define NODE_VirtualChannelGroup        L"VirtualChannelGroup"          ///< VirtualChannelGroup
#define NODE_VirtualChannel             L"VirtualChannel"               ///< VirtualChannel
#define NODE_VirtualChannelName         L"Name"                         ///< Name
#define NODE_CurrentType                L"CurrentType"                  ///< 电流类型
#define NODE_SmvCbGroup                 L"SmvCbGroup"                   ///< SmvCbGroup
#define NODE_IEC61850_9_1               L"IEC61850_9_1"                 ///< IEC61850-9-1
#define NODE_IEC61850_9_2               L"IEC61850_9_2"                 ///< IEC61850-9-2
#define NODE_FT3                        L"FT3"                          ///< FT3
#define NODE_FT3_EXPAND                 L"FT3Expand"                    ///< FT3Expand
#define NODE_SmvCb                      L"SmvCb"                        ///< SmvCb


///
/// @brief
///    构造函数
//
CParamConfigSMVSend::CParamConfigSMVSend(void)
:m_eSMVType(FRAME_TYPE_NET_9_2)
,m_eSVDisplay(SMV_VALUE_TYPE_PRIMARY)
,m_eFrequency(FT3_SAMP_RATE_4K)
,m_eBuadRate(FT3_BAUD_RATE_5M)
,m_uiASDUCount(1)
{
    m_listIEC9_1CbGroup.clear();
    m_listIEC9_2CbGroup.clear();
    m_listFt3CbGroup.clear();
    m_listFt3ExpandCbGroup.clear();
}

///
/// @brief
///    析构函数
///
CParamConfigSMVSend::~CParamConfigSMVSend(void)
{
    ClearAllIEC9_1Cb();
    ClearAllIEC9_2Cb();
    ClearAllFt3Cb();
    ClearAllFt3ExpandCb();
}

///
/// @brief
///    获取配置实例接口
///
CParamConfigSMVSend* CParamConfigSMVSend::getInstance()
{
    static CParamConfigSMVSend s_paramConfigSMVSend;
    return &s_paramConfigSMVSend;
}

///
/// @brief
///    加载配置文件
///
bool CParamConfigSMVSend::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    // 检查输入参数
    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // 加载XML文件
    if (!xml.Load(m_strConfigFile))
    {
        return false;
    }

    // 清空容器
    m_mapCurrentType.clear();
    ClearAllIEC9_1Cb();
    ClearAllIEC9_2Cb();
    ClearAllFt3Cb();
    ClearAllFt3ExpandCb();

    // find and inside <config>
    if (!xml.FindElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    // <SMVType>
    if(!xml.FindElem(NODE_SMVType))
    {
        return false;
    }
    m_eSMVType = (enumFrameType)_wtoi(xml.GetData().c_str());

    // <SVDisplay>
    if(!xml.FindElem(NODE_SVDisplay))
    {
        return false;
    }
    m_eSVDisplay = (enumSmvValueType)_wtoi(xml.GetData().c_str());

    // <Frequency>
    if(!xml.FindElem(NODE_Frequency))
    {
        return false;
    }
    m_eFrequency = (FT3_SAMP_RATE)_wtoi(xml.GetData().c_str());

    // <BuadRate>
    if(!xml.FindElem(NODE_BuadRate))
    {
        return false;
    }
    m_eBuadRate = (FT3_BAUD_RATE)_wtoi(xml.GetData().c_str());

    // <ASDUCount>
    if(!xml.FindElem(NODE_ASDUCount))
    {
        return false;
    }
    m_uiASDUCount = _wtoi(xml.GetData().c_str());

    // <VirtualChannelGroup>
    if (xml.FindElem(NODE_VirtualChannelGroup) && xml.IntoElem())
    {
        // <VirtualChannel>
        std::wstring    strChannelName;
        eCurrentType    eChannelCurType;
        while(xml.FindElem(NODE_VirtualChannel))
        {
            // <Name>
            if (!xml.FindChildElem(NODE_VirtualChannelName))
            {
                return false;
            }
            strChannelName = xml.GetChildData();

            // CurrentType
            if (!xml.FindChildElem(NODE_CurrentType))
            {
                return false;
            }
            eChannelCurType = (eCurrentType)_wtoi(xml.GetChildData().c_str());

            // 保存虚拟通道的交直流设置
            std::pair< std::map<std::wstring , eCurrentType>::iterator, bool > retPair;
            retPair = m_mapCurrentType.insert(std::pair<std::wstring, eCurrentType>(strChannelName, eChannelCurType));
            if (!retPair.second)
            {
                return false;
            }
        }

        // Quit <VirtualChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    //
    // find and inside <SmvCbGroup>
    //
    if (xml.FindElem(NODE_SmvCbGroup) && xml.IntoElem())
    {
        //
        // find and inside <IEC61850_9_1>
        //
        if (xml.FindElem(NODE_IEC61850_9_1) && xml.IntoElem())
        {
            // find <SmvCb>
            while(xml.FindElem(NODE_SmvCb))
            {
                CIEC9_1Cb*  pIEC9_1Cb = new CIEC9_1Cb(this);
                if (NULL == pIEC9_1Cb)
                {
                    return false;
                }

                // 设置GOOSE控制块选中状态
                int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
                if (iSelected != 0)
                {
                    pIEC9_1Cb->m_bSelected  = true;
                }

                // inside <SmvCb>
                if (!xml.IntoElem())
                {
                    delete pIEC9_1Cb;
                    return false;
                }

                // 加载IEC61850_9_1控制块
                if (!pIEC9_1Cb->__LoadConfig(xml))
                {
                    delete pIEC9_1Cb;
                    return false;
                }

                // outside <SmvCb>
                if (!xml.OutOfElem())
                {
                    delete pIEC9_1Cb;
                    return false;
                }

                // 添加IEC61850_9_1控制块对象指针到容器
                if (!__AddIEC9_1Cb(pIEC9_1Cb))
                {
                    delete pIEC9_1Cb;
                    return false;
                }
            }

            // Quit <IEC61850_9_1>
            if (!xml.OutOfElem())
            {
                return false;
            }
        }

        //
        // find and inside <IEC61850_9_2>
        //
        if (xml.FindElem(NODE_IEC61850_9_2) && xml.IntoElem())
        {
            // find <SmvCb>
            while(xml.FindElem(NODE_SmvCb))
            {
                CIEC9_2Cb*  pIEC9_2Cb = new CIEC9_2Cb(this);
                if (NULL == pIEC9_2Cb)
                {
                    return false;
                }

                // 设置GOOSE控制块选中状态
                int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
                if (iSelected != 0)
                {
                    pIEC9_2Cb->m_bSelected  = true;
                }

                // inside <SmvCb>
                if (!xml.IntoElem())
                {
                    delete pIEC9_2Cb;
                    return false;
                }

                // 加载IEC61850-9-2控制块
                if (!pIEC9_2Cb->__LoadConfig(xml))
                {
                    delete pIEC9_2Cb;
                    return false;
                }

                // outside <SmvCb>
                if (!xml.OutOfElem())
                {
                    delete pIEC9_2Cb;
                    return false;
                }

                // 添加IEC61850-9-2控制块对象指针到容器
                if (!__AddIEC9_2Cb(pIEC9_2Cb))
                {
                    delete pIEC9_2Cb;
                    return false;
                }
            }

            // Quit <IEC61850_9_2>
            if (!xml.OutOfElem())
            {
                return false;
            }
        }

        //
        // find and inside <FT3>
        //
        if (xml.FindElem(NODE_FT3) && xml.IntoElem())
        {
            // find <SmvCb>
            while(xml.FindElem(NODE_SmvCb))
            {
                CFT3Cb*  pFt3Cb = new CFT3Cb(this);
                if (NULL == pFt3Cb)
                {
                    return false;
                }

                // 设置GOOSE控制块选中状态
                int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
                if (iSelected != 0)
                {
                    pFt3Cb->m_bSelected  = true;
                }

                // inside <SmvCb>
                if (!xml.IntoElem())
                {
                    delete pFt3Cb;
                    return false;
                }

                // 加载FT3控制块
                if (!pFt3Cb->__LoadConfig(xml))
                {
                    delete pFt3Cb;
                    return false;
                }

                // outside <SmvCb>
                if (!xml.OutOfElem())
                {
                    delete pFt3Cb;
                    return false;
                }

                // 添加FT3控制块对象指针到容器
                if (!__AddFt3Cb(pFt3Cb))
                {
                    delete pFt3Cb;
                    return false;
                }
            }

            // Quit <FT3>
            if (!xml.OutOfElem())
            {
                return false;
            }
        }

        //
        // find and inside <FT3Expand>
        //
        if (xml.FindElem(NODE_FT3_EXPAND) && xml.IntoElem())
        {
            // find <SmvCb>
            while(xml.FindElem(NODE_SmvCb))
            {
                CFT3ExpandCb*  pFt3CbEx = new CFT3ExpandCb(this);
                if (NULL == pFt3CbEx)
                {
                    return false;
                }

                // 设置GOOSE控制块选中状态
                int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
                if (iSelected != 0)
                {
                    pFt3CbEx->m_bSelected  = true;
                }

                // inside <SmvCb>
                if (!xml.IntoElem())
                {
                    delete pFt3CbEx;
                    return false;
                }

                // 加载FT3扩展控制块
                if (!pFt3CbEx->__LoadConfig(xml))
                {
                    delete pFt3CbEx;
                    return false;
                }

                // outside <SmvCb>
                if (!xml.OutOfElem())
                {
                    delete pFt3CbEx;
                    return false;
                }

                // 添加FT3扩展控制块对象指针到容器
                if (!__AddFt3ExpandCb(pFt3CbEx))
                {
                    delete pFt3CbEx;
                    return false;
                }
            }

            // Quit <FT3Expand>
            if (!xml.OutOfElem())
            {
                return false;
            }
        }

        // Quit <SmvCbGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // 标记为未修改
    SetModified(false);
    return true;
}

///
/// @brief
///    添加IEC61850_9_1控制块
///
/// @param[in]  pNewCb 指向IEC61850_9_1控制块对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamConfigSMVSend::__AddIEC9_1Cb(CIEC9_1Cb* pNewCb)
{
    if (NULL == pNewCb)
    {
        return false;
    }

    // 添加控制块到容器中
    m_listIEC9_1CbGroup.push_back(pNewCb);
    return true;
}

///
/// @brief
///    添加IEC61850-9-2控制块
///
/// @param[in]  pNewCb 指向IEC61850-9-2控制块对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamConfigSMVSend::__AddIEC9_2Cb(CIEC9_2Cb* pNewCb)
{
    if (NULL == pNewCb)
    {
        return false;
    }

    // 添加控制块到容器中
    m_listIEC9_2CbGroup.push_back(pNewCb);
    return true;
}

///
/// @brief
///    添加FT3控制块
///
/// @param[in]  pNewCb 指向FT3控制块对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamConfigSMVSend::__AddFt3Cb(CFT3Cb* pNewCb)
{
    if (NULL == pNewCb)
    {
        return false;
    }

    // 添加控制块到容器中
    m_listFt3CbGroup.push_back(pNewCb);
    return true;
}

///
/// @brief
///    添加FT3扩展控制块
///
/// @param[in]  pNewCb 指向FT3扩展控制块对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamConfigSMVSend::__AddFt3ExpandCb(CFT3ExpandCb* pNewCb)
{
    if (NULL == pNewCb)
    {
        return false;
    }

    // 添加控制块到容器中
    m_listFt3ExpandCbGroup.push_back(pNewCb);
    return true;
}

///
/// @brief
///    保存配置文件
///
/// @return
///     bool    true表示保存成功
///
bool CParamConfigSMVSend::SaveConfig()
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    // 如果没有任何改动，那么不保存
    if (!m_bModified)
    {
        return true;
    }

    // add and inside <config>
    if (!xml.AddElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    // <SMVType>
    if (!xml.AddElem(NODE_SMVType) || !xml.SetData((int)m_eSMVType))
    {
        return false;
    }

    // <SVDisplay>
    if(!xml.AddElem(NODE_SVDisplay) || !xml.SetData((int)m_eSVDisplay))
    {
        return false;
    }

    // <Frequency>
    if(!xml.AddElem(NODE_Frequency) || !xml.SetData((int)m_eFrequency))
    {
        return false;
    }

    // <BuadRate>
    if(!xml.AddElem(NODE_BuadRate) || !xml.SetData((int)m_eBuadRate))
    {
        return false;
    }

    // <ASDUCount>
    if(!xml.AddElem(NODE_ASDUCount) || !xml.SetData(m_uiASDUCount))
    {
        return false;
    }

    // <VirtualChannelGroup>
    if (xml.AddElem(NODE_VirtualChannelGroup) && xml.IntoElem())
    {
        // 遍历交直流设置容器
        for (std::map<std::wstring, eCurrentType>::iterator iter=m_mapCurrentType.begin(); iter!=m_mapCurrentType.end(); ++iter)
        {
            // <VirtualChannel>
            if (!xml.AddElem(NODE_VirtualChannel))
            {
                return false;
            }

            // <Name>子节点
            if (!xml.AddChildElem(NODE_VirtualChannelName, iter->first))
            {
                return false;
            }

            // <CurrentType>子节点
            if (!xml.AddChildElem(NODE_CurrentType, iter->second))
            {
                return false;
            }
        }

        // Quit <VirtualChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // <SmvCbGroup>
    if (!xml.AddElem(NODE_SmvCbGroup) || !xml.IntoElem())
    {
        return false;
    }

    // IEC61850-9-1控制块组
    if (m_listIEC9_1CbGroup.size())
    {
        // <IEC61850_9_1>
        if (!xml.AddElem(NODE_IEC61850_9_1) || !xml.IntoElem())
        {
            return false;
        }

        // 保存每一个控制块
        CIEC9_1Cb* pIEC9_1Cb = FirstIEC9_1Cb();
        while(pIEC9_1Cb)
        {
            // <SmvCb>
            if (!xml.AddElem(NODE_SmvCb))
            {
                return false;
            }

            // <SmvCb selected="">
            int iSelected = pIEC9_1Cb->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <SmvCb>
            if (!xml.IntoElem())
            {
                return false;
            }

            // 保存IEC61850-9-1控制块
            if (!pIEC9_1Cb->__SaveConfig(xml))
            {
                return false;
            }

            // outside <SmvCb>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pIEC9_1Cb = NextIEC9_1Cb(pIEC9_1Cb);
        }

        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // IEC61850-9-2控制块组
    if (m_listIEC9_2CbGroup.size())
    {
        // <IEC61850_9_2>
        if (!xml.AddElem(NODE_IEC61850_9_2) || !xml.IntoElem())
        {
            return false;
        }

        // 保存每一个控制块
        CIEC9_2Cb* pIEC9_2Cb = FirstIEC9_2Cb();
        while(pIEC9_2Cb)
        {
            // <SmvCb>
            if (!xml.AddElem(NODE_SmvCb))
            {
                return false;
            }

            // <SmvCb selected="">
            int iSelected = pIEC9_2Cb->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <SmvCb>
            if (!xml.IntoElem())
            {
                return false;
            }

            // 保存IEC61850-9-2控制块
            if (!pIEC9_2Cb->__SaveConfig(xml))
            {
                return false;
            }

            // outside <SmvCb>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pIEC9_2Cb = NextIEC9_2Cb(pIEC9_2Cb);
        }

        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // FT3控制块组
    if (m_listFt3CbGroup.size())
    {
        // <FT3>
        if (!xml.AddElem(NODE_FT3) || !xml.IntoElem())
        {
            return false;
        }

        // 保存每一个控制块
        CFT3Cb* pFt3Cb = FirstFt3Cb();
        while(pFt3Cb)
        {
            // <SmvCb>
            if (!xml.AddElem(NODE_SmvCb))
            {
                return false;
            }

            // <SmvCb selected="">
            int iSelected = pFt3Cb->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <SmvCb>
            if (!xml.IntoElem())
            {
                return false;
            }

            // 保存FT3控制块
            if (!pFt3Cb->__SaveConfig(xml))
            {
                return false;
            }

            // outside <SmvCb>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pFt3Cb = NextFt3Cb(pFt3Cb);
        }

        // Quit <FT3>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // FT3扩展控制块组
    if (m_listFt3ExpandCbGroup.size())
    {
        // <FT3Expand>
        if (!xml.AddElem(NODE_FT3_EXPAND) || !xml.IntoElem())
        {
            return false;
        }
        
        // 保存每一个控制块
        CFT3ExpandCb* pFt3ExpandCb = FirstFt3ExpandCb();
        while(pFt3ExpandCb)
        {
            // <SmvCb>
            if (!xml.AddElem(NODE_SmvCb))
            {
                return false;
            }

            // <SmvCb selected="">
            int iSelected = pFt3ExpandCb->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <SmvCb>
            if (!xml.IntoElem())
            {
                return false;
            }

            // 保存FT3控制块
            if (!pFt3ExpandCb->__SaveConfig(xml))
            {
                return false;
            }

            // outside <SmvCb>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pFt3ExpandCb = NextFt3ExpandCb(pFt3ExpandCb);
        }

        // Quit <FT3Expand>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // Quit <SmvCbGroup>
    if (!xml.OutOfElem())
    {
        return false;
    }

    // 添加xml头
    xml.ResetPos();
    xml.InsertNode( CMarkup::MNT_PROCESSING_INSTRUCTION, L"xml" );
    xml.SetAttrib( L"version", L"1.0" );
    xml.SetAttrib( L"encoding", L"utf-8" );

    // 保存xml文件
    if(!xml.Save(m_strConfigFile))
    {
        return false;
    }

    // 标记为未修改
    SetModified(false);
    return true;
}

///
/// @brief
///    获得SMV类型
///
/// @return
///     enumFrameType    获得的SMV类型
///
enumFrameType CParamConfigSMVSend::GetSMVType()
{
    return m_eSMVType;
}

///
/// @brief
///    设置SMV类型
///
/// @param[in]  eNewSMVType SMV类型
///
void CParamConfigSMVSend::SetSMVType(enumFrameType eNewSMVType)
{
    if (m_eSMVType != eNewSMVType)
    {
        m_eSMVType = eNewSMVType;

        // 根据SMV类型修正波特率
        if ((FRAME_TYPE_FT3_GUOWANG == m_eSMVType) && (FT3_BAUD_RATE_2P5M == m_eBuadRate))
        {
            m_eBuadRate = FT3_BAUD_RATE_5M;
        }

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    获得采样值显示方式
///
/// @return
///     enumSmvValueType    采样值显示方式
///
enumSmvValueType CParamConfigSMVSend::GetSVDisplay()
{
    return m_eSVDisplay;
}

///
/// @brief
///    设置采样值显示方式
///
/// @param[in]  eNewSVDisplay 采样值显示方式
///
void CParamConfigSMVSend::SetSVDisplay(enumSmvValueType eNewSVDisplay)
{
    if (m_eSVDisplay != eNewSVDisplay)
    {
        m_eSVDisplay = eNewSVDisplay;

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    获得采样频率
///
/// @return
///     FT3_SAMP_RATE    采样频率
///
FT3_SAMP_RATE CParamConfigSMVSend::GetFrequency()
{
    return m_eFrequency;
}

///
/// @brief
///    设置采样频率
///
/// @param[in]  eNewFrequency 采样频率
///
void CParamConfigSMVSend::SetFrequency(FT3_SAMP_RATE eNewFrequency)
{
    if (m_eFrequency != eNewFrequency)
    {
        m_eFrequency = eNewFrequency;

        // 修正各种类型控制块的勾选数
        ReviseSelectedCbCount();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    获得波特率
///
/// @return
///     FT3_BAUD_RATE    波特率
///
FT3_BAUD_RATE CParamConfigSMVSend::GetBaudRate()
{
    return m_eBuadRate;
}

///
/// @brief
///    设置波特率
///
/// @param[in]  eBaudRate 波特率
///
void CParamConfigSMVSend::SetBaudRate(FT3_BAUD_RATE eNewBaudRate)
{
    if (m_eBuadRate != eNewBaudRate)
    {
        m_eBuadRate = eNewBaudRate;

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    获得ASDU数目
///
/// @return
///     int    ASDU数目，返回-1表示查询失败
///
int CParamConfigSMVSend::GetASDUCount()
{
    return m_uiASDUCount;
}

///
/// @brief
///    设置ASDU数目
///
/// @param[in]  iNewASDUCount ASDU数目
///
/// @return
///     bool    true表示保存成功
///
bool CParamConfigSMVSend::SetASDUCount(int iNewASDUCount)
{
    if ((iNewASDUCount < 1) || (iNewASDUCount > 8))
    {
#if !SHENG_ZE
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
        return false;
    }
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return false;
	}
	FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();
#endif

    if (m_uiASDUCount != iNewASDUCount)
    {
#if !SHENG_ZE
		// 设置ASDU的有效输入数值，如果输入的值无效，则给出警告
		CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
		if (NULL == pParamConfigSMVSend)
		{
			return false;
		}
		FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();
		switch (iNewASDUCount)
		{
		case 1:
			iNewASDUCount = 1;
			break;
		case 2:
			iNewASDUCount = 2;
			break;
		case 3:
			{
				CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
				iNewASDUCount = 2;
			}
			break;
		case 4:
			iNewASDUCount = 4;
			break;
		case 5:
			{
				switch (sampRate)
				{
				case FT3_SAMP_RATE_4K:
					iNewASDUCount = 5;
					break;
				case FT3_SAMP_RATE_12P8K:
					{
						iNewASDUCount = 4;
						CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
					}
					break;
				}
			}
			break;
		case 6:
			{
				switch (sampRate)
				{
				case FT3_SAMP_RATE_4K:
					{
						iNewASDUCount = 5;
						CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
					}
					break;
				case FT3_SAMP_RATE_12P8K:
					{
						iNewASDUCount = 4;
						CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
					}
					break;
				}
			}
			break;
		case 7:
			{
			switch (sampRate)
			{
				case FT3_SAMP_RATE_4K:
					{
						iNewASDUCount = 5;
						CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
					}
					break;
				case FT3_SAMP_RATE_12P8K:
					{
						iNewASDUCount = 4;
						CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_ASDU_COUNT_INVALID), 3000);
					}
					break;
				}
			}
			break;
		case 8:
			iNewASDUCount = 8;
			break;
		default:
			break;
		}
#endif
        m_uiASDUCount = iNewASDUCount;
		
        // 标记为已修改
        SetModified(true);
    }
    return true;
}

///
/// @brief
///    获得指定虚拟通道的交直流设置
///
/// @param[in]  strChannelName 虚拟通道名
///
/// @return
///     eCurrentType    虚拟通道的交直流设置
///
eCurrentType CParamConfigSMVSend::GetCurrentType(std::wstring strChannelName)
{
    if (strChannelName.empty())
    {
        return CURRENT_TYPE_UNKNOWN;
    }

    std::map<std::wstring, eCurrentType>::iterator iter = m_mapCurrentType.find(strChannelName);
    if (m_mapCurrentType.end() != iter)
    {
        return iter->second;
    }

    return CURRENT_TYPE_UNKNOWN;
}

///
/// @brief
///    设置指定虚拟通道的交直流设置
///
/// @param[in]  strChannelName 虚拟通道名
/// @param[in]  eChannelCurType 交直流设置
///
/// @return
///     bool    true表示设置成功
///
bool CParamConfigSMVSend::SetCurrentType(std::wstring strChannelName, eCurrentType eChannelCurType)
{
    if (strChannelName.empty())
    {
        return false;
    }

    std::map<std::wstring , eCurrentType>::iterator iter = m_mapCurrentType.find(strChannelName);
    if (m_mapCurrentType.end() != iter)
    {
        iter->second = eChannelCurType;

        // 标记为已修改
        SetModified(true);
    }

    return true;
}

///
/// @brief
///    枚举第一个IEC61850-9-1控制块
///
/// @return
///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::FirstIEC9_1Cb()
{
    std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
    if (m_listIEC9_1CbGroup.end() != iter)
    {
        return (*iter);
    }

    return NULL;
}

///
/// @brief
///    获得下一个IEC61850-9-1控制块
///
/// @param[in]  pCurIEC9_1Cb 当前IEC61850-9-1控制块的指针
///
/// @return
///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::NextIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb)
{
    CIEC9_1Cb*                       pIEC9_1Cb   = NULL;
    std::list<CIEC9_1Cb*>::iterator  iter;


    iter = m_listIEC9_1CbGroup.begin();
    while (m_listIEC9_1CbGroup.end() != iter)
    {
        pIEC9_1Cb = *iter;

        // 查找当前控制块的位置
        if (pIEC9_1Cb == pCurIEC9_1Cb)
        {
            // 获得下一个控制块的位置
            iter++;
            if (m_listIEC9_1CbGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得上一个IEC61850-9-1控制块
///
/// @param[in]  pCurIEC9_1Cb 当前IEC61850-9-1控制块的指针
///
/// @return
///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::PrevIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb)
{
    CIEC9_1Cb*                               pIEC9_1Cb   = NULL;
    std::list<CIEC9_1Cb*>::reverse_iterator  rIter;


    rIter = m_listIEC9_1CbGroup.rbegin();
    while (m_listIEC9_1CbGroup.rend() != rIter)
    {
        pIEC9_1Cb = *rIter;

        // 查找当前控制块的位置
        if (pIEC9_1Cb == pCurIEC9_1Cb)
        {
            // 获得上一个控制块的位置
            rIter++;
            if (m_listIEC9_1CbGroup.rend() == rIter)
            {
                return NULL;
            }

            return (*rIter);
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    获得最后一个IEC61850-9-1控制块
///
/// @return
///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::LastIEC9_1Cb()
{
    std::list<CIEC9_1Cb*>::reverse_iterator rIter = m_listIEC9_1CbGroup.rbegin();
    if (m_listIEC9_1CbGroup.rend() != rIter)
    {
        return (*rIter);
    }

    return NULL;
}

///
/// @brief
///     获得指定APPID的IEC61850-9-1控制块
///
/// @param[in]  
///     iAPPID  -   控制块的APPID
///
/// @return
///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::__GetIEC9_1Cb(int iAPPID)
{
    CIEC9_1Cb*                          pIEC9_1Cb   = NULL;
    std::list<CIEC9_1Cb*>::iterator     iter;


    iter = m_listIEC9_1CbGroup.begin();
    while (m_listIEC9_1CbGroup.end() != iter)
    {
        pIEC9_1Cb = *iter;
        if (pIEC9_1Cb && (pIEC9_1Cb->GetAPPID() == iAPPID))
        {
            return pIEC9_1Cb;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得被选中的IEC61850-9-1控制块个数
///
/// @return
///     int    被选中的控制块个数
///
int CParamConfigSMVSend::GetSelectedIEC9_1CbCount()
{
    int                             iCount  = 0;
    CIEC9_1Cb*                      pIEC9_1Cb   = NULL;
    std::list<CIEC9_1Cb*>::iterator iter;


    iter = m_listIEC9_1CbGroup.begin();
    while (m_listIEC9_1CbGroup.end() != iter)
    {
        pIEC9_1Cb = *iter;

        // 判断控制块是否被选中
        if (pIEC9_1Cb && pIEC9_1Cb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}

///
/// @brief
///    添加IEC61850-9-1控制块
///
/// @return
///     CIEC9_1Cb*    新添加的IEC61850-9-1控制块对象的指针
///
CIEC9_1Cb* CParamConfigSMVSend::AddIEC9_1Cb()
{
    // new一个控制块对象
    CIEC9_1Cb*  pNewIEC9_1Cb = new CIEC9_1Cb(this);
    if (NULL == pNewIEC9_1Cb)
    {
        return NULL;
    }

    //
    // 如果当前没有IEC61850-9-1控制块，那么添加默认控制块及默认通道
    // 如果当前存在IEC61850-9-1控制块，那么拷贝最后一个控制块
    //
    if (0 == m_listIEC9_1CbGroup.size())
    {
        // 使用模板填充控制块
        if (!pNewIEC9_1Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO))
        {
            delete pNewIEC9_1Cb;
            return NULL;
        }
    }
    else
    {
        // 拷贝已存在的最后一个控制块
        pNewIEC9_1Cb->__Copy(LastIEC9_1Cb());

        // APPID加1
        pNewIEC9_1Cb->m_iAPPID            += 1;

        // MAC地址末字节加1
        pNewIEC9_1Cb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] = pNewIEC9_1Cb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] + 1;
    }

    // 尝试勾选控制块
    TryToSelectIec9_1Cb(pNewIEC9_1Cb);

    // 添加控制块到容器中
    if (!__AddIEC9_1Cb(pNewIEC9_1Cb))
    {
        delete pNewIEC9_1Cb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pNewIEC9_1Cb;
}

///
/// @brief
///     添加IEC61850-9-1控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pNetCb   -   指向控制块的指针
///
/// @return
///     CIEC9_1Cb*   成功时返回新添加的IEC61850-9-1控制块指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::AddIEC9_1Cb(CNetCb* pNetCb)
{
    CIEC9_1Cb*       pNewIEC9_1Cb    = NULL;


    // 检查输入参数
    if (NULL == pNetCb)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    if (__GetIEC9_1Cb(pNetCb->GetAPPID()))
    {
        return NULL;
    }

    // new一个控制块
    pNewIEC9_1Cb = new CIEC9_1Cb(this);
    if (NULL == pNewIEC9_1Cb)
    {
        return NULL;
    }

    // 深度拷贝控制块
    if (!pNewIEC9_1Cb->CNetCb::__Copy(pNetCb))
    {
        delete pNewIEC9_1Cb;
        return NULL;
    }

    // 限制控制块通道数
    pNewIEC9_1Cb->SetChannelCount(12);

    // 尝试勾选控制块
    TryToSelectIec9_1Cb(pNewIEC9_1Cb);

    // 将新的控制块添加到容器中
    if (!__AddIEC9_1Cb(pNewIEC9_1Cb))
    {
        delete pNewIEC9_1Cb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pNewIEC9_1Cb;
}

///
/// @brief
///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pDetectedIEC9_1Item    -   扫描列表中的SMV控制块指针
///
/// @return
///     CIEC9_1Cb*    成功时返回新添加的IEC61850-9-1控制块指针，失败时返回NULL
///
CIEC9_1Cb* CParamConfigSMVSend::AddIEC9_1Cb(CDetectItemFrame* pDetectedIEC9_1Item)
{
    CIEC9_1Cb*      pNewIEC9_1Cb    = NULL;
    Sav91Msg*       pSav91Msg       = NULL;
    bool            bFlag           = false;


    // 检查输入参数
    if (NULL == pDetectedIEC9_1Item)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    CNetDetectItem* pNetDetectItem  = (CNetDetectItem*)pDetectedIEC9_1Item->GetDetectItem();
    if ((NULL == pNetDetectItem) || __GetIEC9_1Cb(pNetDetectItem->appId))
    {
        return NULL;
    }

    // new一个控制块
    pNewIEC9_1Cb = new CIEC9_1Cb(this);
    if (NULL == pNewIEC9_1Cb)
    {
        return NULL;
    }

    // 设置收发端口
    pNewIEC9_1Cb->m_ePortName   = pNetDetectItem->enPortName;

    // 创建IEC61850-9-1报文的编解码接口对象
    pSav91Msg = CreateSav91MsgImpl();
    if (NULL == pSav91Msg)
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 对IEC61850-9-1报文帧进行解码
    if (0 == pSav91Msg->Decode(pDetectedIEC9_1Item->GetFrameData(), pDetectedIEC9_1Item->GetFrameLen()))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 深度拷贝扫描列表中的IEC61850-9-1控制块
    if (!pNewIEC9_1Cb->__Copy(pSav91Msg))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 限制控制块通道数
    pNewIEC9_1Cb->SetChannelCount(12);

    // 尝试勾选控制块
    TryToSelectIec9_1Cb(pNewIEC9_1Cb);

    // 将新的IEC61850-9-1控制块添加到容器中
    if (!__AddIEC9_1Cb(pNewIEC9_1Cb))
    {
        bFlag = true;
        goto ExitHandle;
    }

ExitHandle:
    // 如果有错误出现
    if (bFlag)
    {
        // 如果添加失败，那么释放控制块对象
        if (pNewIEC9_1Cb)
        {
            delete pNewIEC9_1Cb;
            pNewIEC9_1Cb = NULL;
        }
    }
    else
    {
        // 标记为已修改
        SetModified(true);
    }

    // 释放IEC61850-9-1报文的编解码接口对象
    if(pSav91Msg)
    {
        ReleaseSav91MsgImpl(pSav91Msg);
    }

    return pNewIEC9_1Cb;
}

///
/// @brief
///    删除IEC61850-9-1控制块
///
/// @param[in]  pCurIEC9_1Cb IEC61850-9-1控制块指针
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb)
{
    // 检查输入参数
    if (NULL == pCurIEC9_1Cb)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
    while (m_listIEC9_1CbGroup.end() != iter)
    {
        CIEC9_1Cb* pIEC9_1Cb = *iter;
        if (pIEC9_1Cb == pCurIEC9_1Cb)
        {
            m_listIEC9_1CbGroup.remove(pIEC9_1Cb);
            delete pIEC9_1Cb;

            // 标记为已修改
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    删除指定APPID的IEC61850-9-1控制块
///
/// @param[in]  iAPPID 待删除的控制块APPID
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteIEC9_1Cb(int iAPPID)
{
    // 检查输入参数
    if (0 >= iAPPID)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
    while (m_listIEC9_1CbGroup.end() != iter)
    {
        CIEC9_1Cb* pIEC9_1Cb = *iter;
        if (pIEC9_1Cb && (pIEC9_1Cb->GetAPPID() == iAPPID))
        {
            m_listIEC9_1CbGroup.remove(pIEC9_1Cb);
            delete pIEC9_1Cb;

            // 标记为已修改
            SetModified(true);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    清除所有IEC61850-9-1控制块
///
void CParamConfigSMVSend::ClearAllIEC9_1Cb()
{
    // 遍历容器，释放对象
    std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
    while (m_listIEC9_1CbGroup.end() != iter)
    {
        CIEC9_1Cb* pIEC9_1Cb = *iter;
        if (pIEC9_1Cb)
        {
            delete pIEC9_1Cb;
        }

        iter++;
    }

    if (m_listIEC9_1CbGroup.size())
    {
        // 清空子通道
        m_listIEC9_1CbGroup.clear();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    枚举第一个IEC61850-9-2控制块
///
/// @return
///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::FirstIEC9_2Cb()
{
    std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
    if (m_listIEC9_2CbGroup.end() != iter)
    {
        return (*iter);
    }

    return NULL;
}

///
/// @brief
///    获得下一个IEC61850-9-2控制块
///
/// @param[in]  pCurIEC9_2Cb 当前IEC61850-9-2控制块的指针
///
/// @return
///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb)
{
    CIEC9_2Cb*                       pIEC9_2Cb   = NULL;
    std::list<CIEC9_2Cb*>::iterator  iter;


    iter = m_listIEC9_2CbGroup.begin();
    while (m_listIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = *iter;

        // 查找当前控制块的位置
        if (pIEC9_2Cb == pCurIEC9_2Cb)
        {
            // 获得下一个控制块的位置
            iter++;
            if (m_listIEC9_2CbGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得上一个IEC61850-9-2控制块
///
/// @param[in]  pCurIEC9_2Cb 当前IEC61850-9-2控制块的指针
///
/// @return
///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::PrevIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb)
{
    CIEC9_2Cb*                               pIEC9_2Cb   = NULL;
    std::list<CIEC9_2Cb*>::reverse_iterator  rIter;


    rIter = m_listIEC9_2CbGroup.rbegin();
    while (m_listIEC9_2CbGroup.rend() != rIter)
    {
        pIEC9_2Cb = *rIter;

        // 查找当前控制块的位置
        if (pIEC9_2Cb == pCurIEC9_2Cb)
        {
            // 获得上一个控制块的位置
            rIter++;
            if (m_listIEC9_2CbGroup.rend() == rIter)
            {
                return NULL;
            }

            return (*rIter);
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    获得最后一个IEC61850-9-2控制块
///
/// @return
///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::LastIEC9_2Cb()
{
    std::list<CIEC9_2Cb*>::reverse_iterator rIter = m_listIEC9_2CbGroup.rbegin();
    if (m_listIEC9_2CbGroup.rend() != rIter)
    {
        return (*rIter);
    }

    return NULL;
}

///
/// @brief
///     获得指定APPID的IEC61850-9-2控制块
///
/// @param[in]  
///     iAPPID  -   控制块的APPID
///
/// @return
///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::__GetIEC9_2Cb(int iAPPID)
{
    CIEC9_2Cb*                          pIEC9_2Cb   = NULL;
    std::list<CIEC9_2Cb*>::iterator     iter;


    iter = m_listIEC9_2CbGroup.begin();
    while (m_listIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = *iter;
        if (pIEC9_2Cb && (pIEC9_2Cb->GetAPPID() == iAPPID))
        {
            return pIEC9_2Cb;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得被选中的IEC61850-9-2控制块个数
///
/// @return
///     int    被选中的控制块个数
///
int CParamConfigSMVSend::GetSelectedIEC9_2CbCount()
{
    int                             iCount  = 0;
    CIEC9_2Cb*                      pIEC9_2Cb   = NULL;
    std::list<CIEC9_2Cb*>::iterator iter;


    iter = m_listIEC9_2CbGroup.begin();
    while (m_listIEC9_2CbGroup.end() != iter)
    {
        pIEC9_2Cb = *iter;

        // 判断控制块是否被选中
        if (pIEC9_2Cb && pIEC9_2Cb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}

///
/// @brief
///    添加IEC61850-9-2控制块
///
/// @return
///     CIEC9_2Cb*    新添加的IEC61850-9-2控制块对象的指针
///
CIEC9_2Cb* CParamConfigSMVSend::AddIEC9_2Cb()
{
    // new一个控制块对象
    CIEC9_2Cb*  pNewIEC9_2Cb = new CIEC9_2Cb(this);
    if (NULL == pNewIEC9_2Cb)
    {
        return NULL;
    }

    //
    // 如果当前没有IEC61850-9-2控制块，那么添加默认控制块及默认通道
    // 如果当前存在IEC61850-9-2控制块，那么拷贝最后一个控制块
    //
    if (0 == m_listIEC9_2CbGroup.size())
    {
        // 使用模板填充控制块
        if (!pNewIEC9_2Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO))
        {
            delete pNewIEC9_2Cb;
            return NULL;
        }
    }
    else
    {
        // 拷贝已存在的最后一个控制块
        pNewIEC9_2Cb->__Copy(LastIEC9_2Cb());

        // APPID加1
        pNewIEC9_2Cb->m_iAPPID            += 1;

        // MAC地址末字节加1
        pNewIEC9_2Cb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] = pNewIEC9_2Cb->m_ucDesMAC[ETHERNET_MAC_LEN - 1] + 1;
    }

    // 尝试勾选控制块
    TryToSelectIec9_2Cb(pNewIEC9_2Cb);

    // 添加控制块到容器中
    if (!__AddIEC9_2Cb(pNewIEC9_2Cb))
    {
        delete pNewIEC9_2Cb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pNewIEC9_2Cb;
}

///
/// @brief
///     添加IEC61850-9-2控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pIEC9_2Cb   -   IEC61850-9-2控制块指针
///
/// @return
///     CIEC9_2Cb*   成功时返回新添加的IEC61850-9-2控制块指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::AddIEC9_2Cb(CIEC9_2Cb* pIEC9_2Cb)
{
    CIEC9_2Cb*       pNewIEC9_2Cb    = NULL;


    // 检查输入参数
    if (NULL == pIEC9_2Cb)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    if (__GetIEC9_2Cb(pIEC9_2Cb->GetAPPID()))
    {
        return NULL;
    }

    // new一个控制块
    pNewIEC9_2Cb = new CIEC9_2Cb(this);
    if (NULL == pNewIEC9_2Cb)
    {
        return NULL;
    }

    // 深度拷贝控制块
    if (!pNewIEC9_2Cb->__Copy(pIEC9_2Cb))
    {
        delete pNewIEC9_2Cb;
        return NULL;
    }

    // 尝试勾选控制块
    TryToSelectIec9_2Cb(pNewIEC9_2Cb);

    // 将新的控制块添加到容器中
    if (!__AddIEC9_2Cb(pNewIEC9_2Cb))
    {
        delete pNewIEC9_2Cb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pNewIEC9_2Cb;
}

///
/// @brief
///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pDetectedIEC9_2Item   -   扫描列表中的SMV控制块指针
///
/// @return
///     CIEC9_2Cb*    成功时返回新添加的IEC61850-9-2控制块指针，失败时返回NULL
///
CIEC9_2Cb* CParamConfigSMVSend::AddIEC9_2Cb(CDetectItemFrame* pDetectedIEC9_2Item)
{
    CIEC9_2Cb*      pNewIEC9_2Cb    = NULL;
    Sav92Msg*       pSav92Msg       = NULL;
    bool            bFlag           = false;


    // 检查输入参数
    if (NULL == pDetectedIEC9_2Item)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    CNetDetectItem* pNetDetectItem  = (CNetDetectItem*)pDetectedIEC9_2Item->GetDetectItem();
    if ((NULL == pNetDetectItem) || __GetIEC9_2Cb(pNetDetectItem->appId))
    {
        return NULL;
    }

    // new一个控制块
    pNewIEC9_2Cb = new CIEC9_2Cb(this);
    if (NULL == pNewIEC9_2Cb)
    {
        return NULL;
    }

    // 设置收发端口
    pNewIEC9_2Cb->m_ePortName   = pNetDetectItem->enPortName;

    // 创建IEC61850-9-2报文的编解码接口对象
    pSav92Msg = CreateSav92MsgImpl();
    if (NULL == pSav92Msg)
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 对IEC61850-9-2报文帧进行解码
    if (0 == pSav92Msg->Decode(pDetectedIEC9_2Item->GetFrameData(), pDetectedIEC9_2Item->GetFrameLen()))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 深度拷贝扫描列表中的IEC61850-9-2控制块
    if (!pNewIEC9_2Cb->__Copy(pSav92Msg))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 尝试勾选控制块
    TryToSelectIec9_2Cb(pNewIEC9_2Cb);

    // 将新的IEC61850-9-2控制块添加到容器中
    if (!__AddIEC9_2Cb(pNewIEC9_2Cb))
    {
        bFlag = true;
        goto ExitHandle;
    }

ExitHandle:
    // 如果有错误出现
    if (bFlag)
    {
        // 如果添加失败，那么释放控制块对象
        if (pNewIEC9_2Cb)
        {
            delete pNewIEC9_2Cb;
            pNewIEC9_2Cb = NULL;
        }
    }
    else
    {
        // 标记为已修改
        SetModified(true);
    }

    // 释放IEC61850-9-2报文的编解码接口对象
    if(pSav92Msg)
    {
        ReleaseSav92MsgImpl(pSav92Msg);
    }

    return pNewIEC9_2Cb;
}

///
/// @brief
///    删除IEC61850-9-2控制块
///
/// @param[in]  pCurIEC9_2Cb IEC61850-9-2控制块指针
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb)
{
    // 检查输入参数
    if (NULL == pCurIEC9_2Cb)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
    while (m_listIEC9_2CbGroup.end() != iter)
    {
        CIEC9_2Cb* pIEC9_2Cb = *iter;
        if (pIEC9_2Cb  == pCurIEC9_2Cb)
        {
            m_listIEC9_2CbGroup.remove(pIEC9_2Cb);
            delete pIEC9_2Cb;

            // 标记为已修改
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    删除指定APPID的IEC61850-9-2控制块
///
/// @param[in]  iAPPID 待删除的控制块APPID
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteIEC9_2Cb(int iAPPID)
{
    // 检查输入参数
    if (0 >= iAPPID)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
    while (m_listIEC9_2CbGroup.end() != iter)
    {
        CIEC9_2Cb* pIEC9_2Cb = *iter;
        if (pIEC9_2Cb && (pIEC9_2Cb->GetAPPID() == iAPPID))
        {
            m_listIEC9_2CbGroup.remove(pIEC9_2Cb);
            delete pIEC9_2Cb;

            // 标记为已修改
            SetModified(true);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    清除所有IEC61850-9-2控制块
///
void CParamConfigSMVSend::ClearAllIEC9_2Cb()
{
    // 遍历容器，释放对象
    std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
    while (m_listIEC9_2CbGroup.end() != iter)
    {
        CIEC9_2Cb* pIEC9_2Cb = *iter;
        if (pIEC9_2Cb)
        {
            delete pIEC9_2Cb;
        }

        iter++;
    }

    if (m_listIEC9_2CbGroup.size())
    {
        // 清空子通道
        m_listIEC9_2CbGroup.clear();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    获得第一个FT3控制块
///
/// @return
///     CFT3Cb*    成功时返回CFT3控制块指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::FirstFt3Cb()
{
    std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
    if (m_listFt3CbGroup.end() != iter)
    {
        return (*iter);
    }

    return NULL;
}

///
/// @brief
///    获得下一个FT3控制块
///
/// @param[in]  pCurFt3Cb 当前FT3控制块的指针
///
/// @return
///     CFT3Cb*    成功时返回FT3控制块指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::NextFt3Cb(CFT3Cb* pCurFt3Cb)
{
    CFT3Cb*                       pFt3Cb   = NULL;
    std::list<CFT3Cb*>::iterator  iter;


    iter = m_listFt3CbGroup.begin();
    while (m_listFt3CbGroup.end() != iter)
    {
        pFt3Cb = *iter;

        // 查找当前控制块的位置
        if (pFt3Cb == pCurFt3Cb)
        {
            // 获得下一个控制块的位置
            iter++;
            if (m_listFt3CbGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得上一个FT3控制块
///
/// @param[in]  pCurFt3Cb 当前FT3控制块的指针
///
/// @return
///     CFT3Cb*    成功时返回FT3控制块指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::PrevFt3Cb(CFT3Cb* pCurFt3Cb)
{
    CFT3Cb*                               pFt3Cb   = NULL;
    std::list<CFT3Cb*>::reverse_iterator  rIter;


    rIter = m_listFt3CbGroup.rbegin();
    while (m_listFt3CbGroup.rend() != rIter)
    {
        pFt3Cb = *rIter;

        // 查找当前控制块的位置
        if (pFt3Cb == pCurFt3Cb)
        {
            // 获得上一个控制块的位置
            rIter++;
            if (m_listFt3CbGroup.rend() == rIter)
            {
                return NULL;
            }

            return (*rIter);
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    获得最后一个FT3控制块
///
/// @return
///     CFT3Cb*    成功时返回FT3控制块指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::LastFt3Cb()
{
    std::list<CFT3Cb*>::reverse_iterator rIter = m_listFt3CbGroup.rbegin();
    if (m_listFt3CbGroup.rend() != rIter)
    {
        return (*rIter);
    }

    return NULL;
}

///
/// @brief
///     获得指定APPID的FT3控制块
///
/// @param[in]  
///     iAPPID  -   控制块的APPID
///
/// @return
///     CFT3Cb*     成功时返回控制块的指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::__GetFt3Cb(int iAPPID)
{
    CFT3Cb*                          pFt3Cb   = NULL;
    std::list<CFT3Cb*>::iterator     iter;


    iter = m_listFt3CbGroup.begin();
    while (m_listFt3CbGroup.end() != iter)
    {
        pFt3Cb = *iter;
        if (pFt3Cb && (pFt3Cb->GetAPPID() == iAPPID))
        {
            return pFt3Cb;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得被选中的FT3控制块个数
///
/// @return
///     int    被选中的控制块个数
///
int CParamConfigSMVSend::GetSelectedFt3CbCount()
{
    int                             iCount  = 0;
    CFT3Cb*                         pFt3Cb   = NULL;
    std::list<CFT3Cb*>::iterator    iter;


    iter = m_listFt3CbGroup.begin();
    while (m_listFt3CbGroup.end() != iter)
    {
        pFt3Cb = *iter;

        // 判断控制块是否被选中
        if (pFt3Cb && pFt3Cb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}

///
/// @brief
///    添加FT3控制块
///
/// @return
///     CFT3Cb*    新添加的FT3控制块对象的指针
///
CFT3Cb* CParamConfigSMVSend::AddFt3Cb()
{
    // new一个FT3控制块对象
    CFT3Cb*  pFt3Cb = new CFT3Cb(this);
    if (NULL == pFt3Cb)
    {
        return NULL;
    }

    //
    // 如果当前没有FT3控制块，那么添加默认控制块及默认通道
    // 如果当前存在FT3控制块，那么拷贝最后一个控制块
    //
    if (0 == m_listFt3CbGroup.size())
    {
        // 使用模板填充控制块
        if (!pFt3Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO))
        {
            delete pFt3Cb;
            return NULL;
        }
    }
    else
    {
        // 拷贝已存在的最后一个控制块
        pFt3Cb->__Copy(LastFt3Cb());

        // APPID加1
        pFt3Cb->m_iAPPID               += 1;
    }

    // 尝试勾选控制块
    if (GetSelectedFt3CbCount() < MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX)
    {
        pFt3Cb->Selected(true);
    }

    // 添加FT3控制块到容器中
    if (!__AddFt3Cb(pFt3Cb))
    {
        delete pFt3Cb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pFt3Cb;
}

///
/// @brief
///     添加FT3控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pBaseCb   -   控制块指针
///
/// @return
///     CFT3Cb*   成功时返回新添加的FT3控制块指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::AddFt3Cb(CBaseCb* pBaseCb)
{
    CFT3Cb*       pNewFt3Cb    = NULL;


    // 检查输入参数
    if (NULL == pBaseCb)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    if (__GetFt3Cb(pBaseCb->GetAPPID()))
    {
        return NULL;
    }

    // new一个控制块
    pNewFt3Cb = new CFT3Cb(this);
    if (NULL == pNewFt3Cb)
    {
        return NULL;
    }

    // 深度拷贝控制块
    if (!pNewFt3Cb->CBaseCb::__Copy(pBaseCb))
    {
        delete pNewFt3Cb;
        return NULL;
    }

    // 限制控制块通道数
    pNewFt3Cb->SetChannelCount(12);

    // 设置发送端口
    pNewFt3Cb->m_ePortName = FT3_PORT_LIGHT_C;

    // 尝试勾选控制块
    if (GetSelectedFt3CbCount() < MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX)
    {
        pNewFt3Cb->Selected(true);
    }

    // 将新的控制块添加到容器中
    if (!__AddFt3Cb(pNewFt3Cb))
    {
        delete pNewFt3Cb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pNewFt3Cb;
}

///
/// @brief
///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pDetectedFt3Item   -   扫描列表中的SMV控制块指针
///
/// @return
///     CFT3Cb*    成功时返回新添加的FT3控制块指针，失败时返回NULL
///
CFT3Cb* CParamConfigSMVSend::AddFt3Cb(CDetectItemFrame* pDetectedFt3Item)
{
    CFT3Cb*         pNewFt3Cb       = NULL;
    NanRuiFT3Msg*   pFt3Msg         = NULL;
    bool            bFlag           = false;


    // 检查输入参数
    if (NULL == pDetectedFt3Item)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    CFt3DetectItem* pFt3DetectItem  = (CFt3DetectItem*)pDetectedFt3Item->GetDetectItem();
    if ((NULL == pFt3DetectItem) || __GetFt3Cb(pFt3DetectItem->nLDName))
    {
        return NULL;
    }

    // new一个控制块
    pNewFt3Cb = new CFT3Cb(this);
    if (NULL == pNewFt3Cb)
    {
        return NULL;
    }

    // 设置发送端口
    pNewFt3Cb->m_ePortName = FT3_PORT_LIGHT_C;

    // 创建FT3报文的编解码接口对象
    pFt3Msg = CreateNanRuiFT3MsgImpl();
    if (NULL == pFt3Msg)
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 对FT3报文帧进行解码
    if (0 == pFt3Msg->Decode((const uint16_t*)pDetectedFt3Item->GetFrameData(), pDetectedFt3Item->GetFrameLen() / 2))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 深度拷贝扫描列表中的FT3控制块
    if (!pNewFt3Cb->__Copy(pFt3Msg))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 限制控制块通道数
    pNewFt3Cb->SetChannelCount(12);

    // 尝试勾选控制块
    if (GetSelectedFt3CbCount() < MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX)
    {
        pNewFt3Cb->Selected(true);
    }

    // 将新的FT3控制块添加到容器中
    if (!__AddFt3Cb(pNewFt3Cb))
    {
        bFlag = true;
        goto ExitHandle;
    }

ExitHandle:
    // 如果有错误出现
    if (bFlag)
    {
        // 如果添加失败，那么释放控制块对象
        if (pNewFt3Cb)
        {
            delete pNewFt3Cb;
            pNewFt3Cb = NULL;
        }
    }
    else
    {
        // 标记为已修改
        SetModified(true);
    }

    // 释放FT3报文的编解码接口对象
    if(pFt3Msg)
    {
        ReleaseNanRuiFT3MsgImpl(pFt3Msg);
    }

    return pNewFt3Cb;
}

///
/// @brief
///    删除FT3控制块
///
/// @param[in]  pCurFt3Cb FT3控制块指针
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteFt3Cb(CFT3Cb* pCurFt3Cb)
{
    // 检查输入参数
    if (NULL == pCurFt3Cb)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
    while (m_listFt3CbGroup.end() != iter)
    {
        CFT3Cb* pFt3Cb = *iter;
        if (pFt3Cb  == pCurFt3Cb)
        {
            m_listFt3CbGroup.remove(pFt3Cb);
            delete pFt3Cb;

            // 标记为已修改
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    删除指定APPID（即LDName）的FT3控制块
///
/// @param[in]  iAPPID 待删除的控制块APPID
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteFt3Cb(int iAPPID)
{
    // 检查输入参数
    if (0 >= iAPPID)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
    while (m_listFt3CbGroup.end() != iter)
    {
        CFT3Cb* pFt3Cb = *iter;
        if (pFt3Cb && (pFt3Cb->GetAPPID() == iAPPID))
        {
            m_listFt3CbGroup.remove(pFt3Cb);
            delete pFt3Cb;

            // 标记为已修改
            SetModified(true);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    清除所有FT3控制块
///
void CParamConfigSMVSend::ClearAllFt3Cb()
{
    // 遍历容器，释放对象
    std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
    while (m_listFt3CbGroup.end() != iter)
    {
        CFT3Cb* pFt3Cb = *iter;
        if (pFt3Cb)
        {
            delete pFt3Cb;
        }

        iter++;
    }

    if (m_listFt3CbGroup.size())
    {
        // 清空子通道
        m_listFt3CbGroup.clear();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    获得第一个FT3扩展控制块
///
/// @return
///     CFT3ExpandCb*    成功时返回CFT3扩展控制块指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::FirstFt3ExpandCb()
{
    std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
    if (m_listFt3ExpandCbGroup.end() != iter)
    {
        return (*iter);
    }

    return NULL;
}

///
/// @brief
///    获得下一个FT3扩展控制块
///
/// @param[in]  pCurFt3ExpandCb 当前FT3扩展控制块的指针
///
/// @return
///     CFT3ExpandCb*    成功时返回FT3扩展控制块指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::NextFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb)
{
    CFT3ExpandCb*                       pFt3ExpandCb   = NULL;
    std::list<CFT3ExpandCb*>::iterator  iter;


    iter = m_listFt3ExpandCbGroup.begin();
    while (m_listFt3ExpandCbGroup.end() != iter)
    {
        pFt3ExpandCb = *iter;

        // 查找当前控制块的位置
        if (pFt3ExpandCb == pCurFt3ExpandCb)
        {
            // 获得下一个控制块的位置
            iter++;
            if (m_listFt3ExpandCbGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得上一个FT3扩展控制块
///
/// @param[in]  pCurFt3ExpandCb 当前FT3扩展控制块的指针
///
/// @return
///     CFT3ExpandCb*    成功时返回FT3扩展控制块指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::PrevFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb)
{
    CFT3ExpandCb*                               pFt3ExpandCb   = NULL;
    std::list<CFT3ExpandCb*>::reverse_iterator  rIter;


    rIter = m_listFt3ExpandCbGroup.rbegin();
    while (m_listFt3ExpandCbGroup.rend() != rIter)
    {
        pFt3ExpandCb = *rIter;

        // 查找当前控制块的位置
        if (pFt3ExpandCb == pCurFt3ExpandCb)
        {
            // 获得上一个控制块的位置
            rIter++;
            if (m_listFt3ExpandCbGroup.rend() == rIter)
            {
                return NULL;
            }

            return (*rIter);
        }

        rIter++;
    }

    return NULL;
}

///
/// @brief
///    获得最后一个FT3扩展控制块
///
/// @return
///     CFT3ExpandCb*    成功时返回FT3扩展控制块指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::LastFt3ExpandCb()
{
    std::list<CFT3ExpandCb*>::reverse_iterator rIter = m_listFt3ExpandCbGroup.rbegin();
    if (m_listFt3ExpandCbGroup.rend() != rIter)
    {
        return (*rIter);
    }

    return NULL;
}

///
/// @brief
///     获得指定APPID的FT3扩展控制块
///
/// @param[in]  
///     iAPPID  -       控制块的APPID
///
/// @return
///     CFT3ExpandCb*   成功时返回控制块的指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::__GetFt3ExpandCb(int iAPPID)
{
    CFT3ExpandCb*                       pFt3ExpandCb    = NULL;
    std::list<CFT3ExpandCb*>::iterator  iter;


    iter = m_listFt3ExpandCbGroup.begin();
    while (m_listFt3ExpandCbGroup.end() != iter)
    {
        pFt3ExpandCb = *iter;
        if (pFt3ExpandCb && (pFt3ExpandCb->GetAPPID() == iAPPID))
        {
            return pFt3ExpandCb;
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得被选中的FT3扩展控制块个数
///
/// @return
///     int    被选中的控制块个数
///
int CParamConfigSMVSend::GetSelectedFt3ExpandCbCount()
{
    int                                     iCount  = 0;
    CFT3ExpandCb*                           pFt3ExpandCb   = NULL;
    std::list<CFT3ExpandCb*>::iterator      iter;


    iter = m_listFt3ExpandCbGroup.begin();
    while (m_listFt3ExpandCbGroup.end() != iter)
    {
        pFt3ExpandCb = *iter;

        // 判断控制块是否被选中
        if (pFt3ExpandCb && pFt3ExpandCb->IsSelected())
        {
            iCount++;
        }

        iter++;
    }

    return iCount;
}

///
/// @brief
///    添加FT3扩展控制块
///
/// @return
///     CFT3ExpandCb*    新添加的FT3扩展控制块对象的指针
///
CFT3ExpandCb* CParamConfigSMVSend::AddFt3ExpandCb()
{
    // new一个控制块对象
    CFT3ExpandCb*  pFt3ExpandCb = new CFT3ExpandCb(this);
    if (NULL == pFt3ExpandCb)
    {
        return NULL;
    }

    //
    // 如果当前没有FT3扩展控制块，那么添加默认控制块及默认通道
    // 如果当前存在FT3扩展控制块，那么拷贝最后一个控制块
    //
    if (0 == m_listFt3ExpandCbGroup.size())
    {
        // 使用模板填充控制块
        if (!pFt3ExpandCb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO))
        {
            delete pFt3ExpandCb;
            return NULL;
        }
    }
    else
    {
        // 拷贝已存在的最后一个控制块
        pFt3ExpandCb->CFT3Cb::__Copy(LastFt3ExpandCb());

        // APPID加1
        pFt3ExpandCb->m_iAPPID         += 1;
    }

    // 尝试勾选控制块
    if (GetSelectedFt3ExpandCbCount() < MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX)
    {
        pFt3ExpandCb->Selected(true);
    }

    // 添加控制块到容器中
    if (!__AddFt3ExpandCb(pFt3ExpandCb))
    {
        delete pFt3ExpandCb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pFt3ExpandCb;
}

///
/// @brief
///     添加FT3扩展控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pBaseCb   -   控制块指针
///
/// @return
///     CFT3ExpandCb*   成功时返回新添加的FT3扩展控制块指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::AddFt3ExpandCb(CBaseCb* pBaseCb)
{
    CFT3ExpandCb*       pNewFt3ExpandCb    = NULL;


    // 检查输入参数
    if (NULL == pBaseCb)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    if (__GetFt3ExpandCb(pBaseCb->GetAPPID()))
    {
        return NULL;
    }

    // new一个控制块
    pNewFt3ExpandCb = new CFT3ExpandCb(this);
    if (NULL == pNewFt3ExpandCb)
    {
        return NULL;
    }

    // 深度拷贝控制块
    if (!pNewFt3ExpandCb->CBaseCb::__Copy(pBaseCb))
    {
        delete pNewFt3ExpandCb;
        return NULL;
    }

    // 限制控制块通道数
    pNewFt3ExpandCb->SetChannelCount(22);

    // 设置发送端口
    pNewFt3ExpandCb->m_ePortName = FT3_PORT_LIGHT_C;

    // 尝试勾选控制块
    if (GetSelectedFt3ExpandCbCount() < MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX)
    {
        pNewFt3ExpandCb->Selected(true);
    }

    // 将新的控制块添加到容器中
    if (!__AddFt3ExpandCb(pNewFt3ExpandCb))
    {
        delete pNewFt3ExpandCb;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pNewFt3ExpandCb;
}

///
/// @brief
///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
///
/// @param[in]
///     pDetectedFt3ExpandItem   -   扫描列表中的SMV控制块指针
///
/// @return
///     CFT3ExpandCb*    成功时返回新添加的FT3扩展控制块指针，失败时返回NULL
///
CFT3ExpandCb* CParamConfigSMVSend::AddFt3ExpandCb(CDetectItemFrame* pDetectedFt3ExpandItem)
{
    CFT3ExpandCb*   pNewFt3ExpandCb         = NULL;
    GuoWangFT3Msg*  pFt3ExpandMsg           = NULL;
    bool            bFlag                   = false;


    // 检查输入参数
    if (NULL == pDetectedFt3ExpandItem)
    {
        return NULL;
    }

    // 检查指定APPID的控制块是否已存在
    CFt3DetectItem* pFt3DetectItem  = (CFt3DetectItem*)pDetectedFt3ExpandItem->GetDetectItem();
    if ((NULL == pFt3DetectItem) || __GetFt3ExpandCb(pFt3DetectItem->nLDName))
    {
        return NULL;
    }

    // new一个控制块
    pNewFt3ExpandCb = new CFT3ExpandCb(this);
    if (NULL == pNewFt3ExpandCb)
    {
        return NULL;
    }

    // 设置发送端口
    pNewFt3ExpandCb->m_ePortName = FT3_PORT_LIGHT_C;

    // 创建FT3扩展报文的编解码接口对象
    pFt3ExpandMsg = CreateGuoWangFT3MsgImpl();
    if (NULL == pFt3ExpandMsg)
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 对FT3扩展报文帧进行解码
    if (0 == pFt3ExpandMsg->Decode((const uint16_t *)pDetectedFt3ExpandItem->GetFrameData(), pDetectedFt3ExpandItem->GetFrameLen() / 2))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 深度拷贝扫描列表中的FT3扩展控制块
    if (!pNewFt3ExpandCb->__Copy(pFt3ExpandMsg))
    {
        bFlag = true;
        goto ExitHandle;
    }

    // 限制控制块通道数
    pNewFt3ExpandCb->SetChannelCount(22);

    // 尝试勾选控制块
    if (GetSelectedFt3ExpandCbCount() < MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX)
    {
        pNewFt3ExpandCb->Selected(true);
    }

    // 将新的FT3扩展控制块添加到容器中
    if (!__AddFt3ExpandCb(pNewFt3ExpandCb))
    {
        bFlag = true;
        goto ExitHandle;
    }

ExitHandle:
    // 如果有错误出现
    if (bFlag)
    {
        // 如果添加失败，那么释放控制块对象
        if (pNewFt3ExpandCb)
        {
            delete pNewFt3ExpandCb;
            pNewFt3ExpandCb = NULL;
        }
    }
    else
    {
        // 标记为已修改
        SetModified(true);
    }

    // 释放FT3扩展报文的编解码接口对象
    if(pFt3ExpandMsg)
    {
        ReleaseGuoWangFT3MsgImpl(pFt3ExpandMsg);
    }

    return pNewFt3ExpandCb;
}

///
/// @brief
///    删除FT3扩展控制块
///
/// @param[in]  pCurFt3ExpandCb FT3扩展控制块指针
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb)
{
    // 检查输入参数
    if (NULL == pCurFt3ExpandCb)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
    while (m_listFt3ExpandCbGroup.end() != iter)
    {
        CFT3ExpandCb* pFt3ExpandCb = *iter;
        if (pFt3ExpandCb == pCurFt3ExpandCb)
        {
            m_listFt3ExpandCbGroup.remove(pFt3ExpandCb);
            delete pFt3ExpandCb;

            // 标记为已修改
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    删除指定APPID（即LDName）的FT3扩展控制块
///
/// @param[in]  iAPPID 待删除的控制块APPID（即LDName）
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigSMVSend::DeleteFt3ExpandCb(int iAPPID)
{
    // 检查输入参数
    if (0 >= iAPPID)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
    while (m_listFt3ExpandCbGroup.end() != iter)
    {
        CFT3ExpandCb* pFt3ExpandCb = *iter;
        if (pFt3ExpandCb && (pFt3ExpandCb->GetAPPID() == iAPPID))
        {
            m_listFt3ExpandCbGroup.remove(pFt3ExpandCb);
            delete pFt3ExpandCb;

            // 标记为已修改
            SetModified(true);
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    清除所有FT3扩展控制块
///
void CParamConfigSMVSend::ClearAllFt3ExpandCb()
{
    // 遍历容器，释放对象
    std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
    while (m_listFt3ExpandCbGroup.end() != iter)
    {
        CFT3ExpandCb* pFt3ExpandCb = *iter;
        if (pFt3ExpandCb)
        {
            delete pFt3ExpandCb;
        }

        iter++;
    }

    if (m_listFt3ExpandCbGroup.size())
    {
        // 清空子通道
        m_listFt3ExpandCbGroup.clear();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    更新所有控制块中电压通道的一次额定值
///
/// @param[in]  dNewValue    新的电压一次额定值（V）
///
void CParamConfigSMVSend::UpdateRatedPrimaryVoltage( double dNewValue )
{
    // IEC61850-9-1
    {
        // 遍历容器
        std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
        while (m_listIEC9_1CbGroup.end() != iter)
        {
            CIEC9_1Cb* pIEC9_1Cb = *iter;
            if (pIEC9_1Cb)
            {
                pIEC9_1Cb->UpdateRatedPrimaryVoltage(dNewValue);
            }

            iter++;
        }
    }

    // IEC61850-9-2
    {
        // 遍历容器
        std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
        while (m_listIEC9_2CbGroup.end() != iter)
        {
            CIEC9_2Cb* pIEC9_2Cb = *iter;
            if (pIEC9_2Cb)
            {
                pIEC9_2Cb->UpdateRatedPrimaryVoltage(dNewValue);
            }

            iter++;
        }
    }

    // FT3
    {
        // 遍历容器
        std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
        while (m_listFt3CbGroup.end() != iter)
        {
            CFT3Cb* pFt3Cb = *iter;
            if (pFt3Cb)
            {
                pFt3Cb->UpdateRatedPrimaryVoltage(dNewValue);
            }

            iter++;
        }
    }

    // FT3扩展
    {
        // 遍历容器
        std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
        while (m_listFt3ExpandCbGroup.end() != iter)
        {
            CFT3ExpandCb* pFt3ExpandCb = *iter;
            if (pFt3ExpandCb)
            {
                pFt3ExpandCb->UpdateRatedPrimaryVoltage(dNewValue);
            }

            iter++;
        }
    }
}

///
/// @brief
///    更新所有控制块中电压通道的二次额定值
///
/// @param[in]  dNewValue    新的电压二次额定值（V）
///
void CParamConfigSMVSend::UpdateRatedSecondaryVoltage( double dNewValue )
{
    // IEC61850-9-1
    {
        // 遍历容器
        std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
        while (m_listIEC9_1CbGroup.end() != iter)
        {
            CIEC9_1Cb* pIEC9_1Cb = *iter;
            if (pIEC9_1Cb)
            {
                pIEC9_1Cb->UpdateRatedSecondaryVoltage(dNewValue);
            }

            iter++;
        }
    }

    // IEC61850-9-2
    {
        // 遍历容器
        std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
        while (m_listIEC9_2CbGroup.end() != iter)
        {
            CIEC9_2Cb* pIEC9_2Cb = *iter;
            if (pIEC9_2Cb)
            {
                pIEC9_2Cb->UpdateRatedSecondaryVoltage(dNewValue);
            }

            iter++;
        }
    }

    // FT3
    {
        // 遍历容器
        std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
        while (m_listFt3CbGroup.end() != iter)
        {
            CFT3Cb* pFt3Cb = *iter;
            if (pFt3Cb)
            {
                pFt3Cb->UpdateRatedSecondaryVoltage(dNewValue);
            }

            iter++;
        }
    }

    // FT3扩展
    {
        // 遍历容器
        std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
        while (m_listFt3ExpandCbGroup.end() != iter)
        {
            CFT3ExpandCb* pFt3ExpandCb = *iter;
            if (pFt3ExpandCb)
            {
                pFt3ExpandCb->UpdateRatedSecondaryVoltage(dNewValue);
            }

            iter++;
        }
    }
}

///
/// @brief
///    更新所有控制块中电流通道的一次额定值
///
/// @param[in]  dNewValue    新的电流一次额定值（A）
///
void CParamConfigSMVSend::UpdateRatedPrimaryCurrent( double dNewValue )
{
    // IEC61850-9-1
    {
        // 遍历容器
        std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
        while (m_listIEC9_1CbGroup.end() != iter)
        {
            CIEC9_1Cb* pIEC9_1Cb = *iter;
            if (pIEC9_1Cb)
            {
                pIEC9_1Cb->UpdateRatedPrimaryCurrent(dNewValue);
            }

            iter++;
        }
    }

    // IEC61850-9-2
    {
        // 遍历容器
        std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
        while (m_listIEC9_2CbGroup.end() != iter)
        {
            CIEC9_2Cb* pIEC9_2Cb = *iter;
            if (pIEC9_2Cb)
            {
                pIEC9_2Cb->UpdateRatedPrimaryCurrent(dNewValue);
            }

            iter++;
        }
    }

    // FT3
    {
        // 遍历容器
        std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
        while (m_listFt3CbGroup.end() != iter)
        {
            CFT3Cb* pFt3Cb = *iter;
            if (pFt3Cb)
            {
                pFt3Cb->UpdateRatedPrimaryCurrent(dNewValue);
            }

            iter++;
        }
    }

    // FT3扩展
    {
        // 遍历容器
        std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
        while (m_listFt3ExpandCbGroup.end() != iter)
        {
            CFT3ExpandCb* pFt3ExpandCb = *iter;
            if (pFt3ExpandCb)
            {
                pFt3ExpandCb->UpdateRatedPrimaryCurrent(dNewValue);
            }

            iter++;
        }
    }
}

///
/// @brief
///    更新所有控制块中电流通道的二次额定值
///
/// @param[in]  dNewValue    新的电流二次额定值（A）
///
void CParamConfigSMVSend::UpdateRatedSecondaryCurrent( double dNewValue )
{
    // IEC61850-9-1
    {
        // 遍历容器
        std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
        while (m_listIEC9_1CbGroup.end() != iter)
        {
            CIEC9_1Cb* pIEC9_1Cb = *iter;
            if (pIEC9_1Cb)
            {
                pIEC9_1Cb->UpdateRatedSecondaryCurrent(dNewValue);
            }

            iter++;
        }
    }

    // IEC61850-9-2
    {
        // 遍历容器
        std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
        while (m_listIEC9_2CbGroup.end() != iter)
        {
            CIEC9_2Cb* pIEC9_2Cb = *iter;
            if (pIEC9_2Cb)
            {
                pIEC9_2Cb->UpdateRatedSecondaryCurrent(dNewValue);
            }

            iter++;
        }
    }

    // FT3
    {
        // 遍历容器
        std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
        while (m_listFt3CbGroup.end() != iter)
        {
            CFT3Cb* pFt3Cb = *iter;
            if (pFt3Cb)
            {
                pFt3Cb->UpdateRatedSecondaryCurrent(dNewValue);
            }

            iter++;
        }
    }

    // FT3扩展
    {
        // 遍历容器
        std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
        while (m_listFt3ExpandCbGroup.end() != iter)
        {
            CFT3ExpandCb* pFt3ExpandCb = *iter;
            if (pFt3ExpandCb)
            {
                pFt3ExpandCb->UpdateRatedSecondaryCurrent(dNewValue);
            }

            iter++;
        }
    }
}

///
/// @brief
///    更新所有控制块中时间通道的额定值
///
/// @param[in]  dNewValue    新的MU额定延时（us）
///
void CParamConfigSMVSend::UpdateMuRatedDelay( double dNewValue )
{
    // IEC61850-9-1
    {
        // 遍历容器
        std::list<CIEC9_1Cb*>::iterator iter = m_listIEC9_1CbGroup.begin();
        while (m_listIEC9_1CbGroup.end() != iter)
        {
            CIEC9_1Cb* pIEC9_1Cb = *iter;
            if (pIEC9_1Cb)
            {
                pIEC9_1Cb->UpdateMuRatedDelay(dNewValue);
            }

            iter++;
        }
    }

    // IEC61850-9-2
    {
        // 遍历容器
        std::list<CIEC9_2Cb*>::iterator iter = m_listIEC9_2CbGroup.begin();
        while (m_listIEC9_2CbGroup.end() != iter)
        {
            CIEC9_2Cb* pIEC9_2Cb = *iter;
            if (pIEC9_2Cb)
            {
                pIEC9_2Cb->UpdateMuRatedDelay(dNewValue);
            }

            iter++;
        }
    }

    // FT3
    {
        // 遍历容器
        std::list<CFT3Cb*>::iterator iter = m_listFt3CbGroup.begin();
        while (m_listFt3CbGroup.end() != iter)
        {
            CFT3Cb* pFt3Cb = *iter;
            if (pFt3Cb)
            {
                pFt3Cb->UpdateMuRatedDelay(dNewValue);
            }

            iter++;
        }
    }

    // FT3扩展
    {
        // 遍历容器
        std::list<CFT3ExpandCb*>::iterator iter = m_listFt3ExpandCbGroup.begin();
        while (m_listFt3ExpandCbGroup.end() != iter)
        {
            CFT3ExpandCb* pFt3ExpandCb = *iter;
            if (pFt3ExpandCb)
            {
                pFt3ExpandCb->UpdateMuRatedDelay(dNewValue);
            }

            iter++;
        }
    }
}

///
/// @brief
///     尝试勾选指定的IEC61850-9-1控制块
///
/// @param[in]  pIec9_1Cb   指向控制块对象的指针
///
/// @return
///     bool    勾选成功返回true
///
bool CParamConfigSMVSend::TryToSelectIec9_1Cb( CIEC9_1Cb* pIec9_1Cb )
{
    int     iSelCount;
    bool    bFlag = true;


    // 获得当前勾选的IEC9_1控制块数
    iSelCount = GetSelectedIEC9_1CbCount();

    // 如果采样率为4K
    if (FT3_SAMP_RATE_4K == m_eFrequency)
    {
        if (iSelCount >= MSGPARAM_SMV_SEND_91_4K_SELECTED_MAX)
        {
            bFlag = false;
        }
    }
    // 如果采样率为12.8k
    else if (FT3_SAMP_RATE_12P8K == m_eFrequency)
    {
        if (iSelCount >= MSGPARAM_SMV_SEND_91_12P8K_SELECTED_MAX)
        {
            bFlag = false;
        }
    }

    pIec9_1Cb->Selected(bFlag);
    return bFlag;
}

///
/// @brief
///     尝试勾选指定的IEC61850-9-2控制块
///
/// @param[in]  pIec9_2Cb   指向控制块对象的指针
///
/// @return
///     bool    勾选成功返回true
///
bool CParamConfigSMVSend::TryToSelectIec9_2Cb( CIEC9_2Cb* pIec9_2Cb )
{
    int     iSelCount;
    bool    bFlag = true;


    // 获得当前勾选的IEC9_2控制块数
    iSelCount = GetSelectedIEC9_2CbCount();

    // 如果采样率为4K
    if (FT3_SAMP_RATE_4K == m_eFrequency)
    {
        if (iSelCount >= MSGPARAM_SMV_SEND_92_4K_SELECTED_MAX)
        {
            bFlag = false;
        }
    }
    // 如果采样率为12.8k
    else if (FT3_SAMP_RATE_12P8K == m_eFrequency)
    {
        if (iSelCount >= MSGPARAM_SMV_SEND_92_12P8K_SELECTED_MAX)
        {
            bFlag = false;
        }
    }

    pIec9_2Cb->Selected(bFlag);
    return bFlag;
}

///
/// @brief
///     修正各种类型控制块的勾选数
///
void CParamConfigSMVSend::ReviseSelectedCbCount()
{
    int     iSelCount;
    int     iMaxSel9_1CbCount;
    int     iMaxSel9_2CbCount;
    int     iMaxSelFt3CbCount;
    int     iMaxSelFt3ExtCbCount;


    // 根据不同的采样率，获得控制块的最大勾选数
    switch (m_eFrequency)
    {
    case FT3_SAMP_RATE_4K:
        iMaxSel9_1CbCount   = MSGPARAM_SMV_SEND_91_4K_SELECTED_MAX;
        iMaxSel9_2CbCount   = MSGPARAM_SMV_SEND_92_4K_SELECTED_MAX;
        iMaxSelFt3CbCount   = MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX;
        iMaxSelFt3ExtCbCount= MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX;
        break;

    case FT3_SAMP_RATE_12P8K:
        iMaxSel9_1CbCount   = MSGPARAM_SMV_SEND_91_12P8K_SELECTED_MAX;
        iMaxSel9_2CbCount   = MSGPARAM_SMV_SEND_92_12P8K_SELECTED_MAX;
        iMaxSelFt3CbCount   = MSGPARAM_SMV_SEND_FT3_CTRL_SELECTED_MAX;
        iMaxSelFt3ExtCbCount= MSGPARAM_SMV_SEND_FT3_EXT_CTRL_SELECTED_MAX;
        break;

    default:
        return;
    }

    // 修正IEC61850-9-1控制块勾选数
    iSelCount = GetSelectedIEC9_1CbCount();
    if (iSelCount > iMaxSel9_1CbCount)
    {
        int i = iSelCount - iMaxSel9_1CbCount;
        std::list<CIEC9_1Cb*>::reverse_iterator rIter = m_listIEC9_1CbGroup.rbegin();
        for (; (i > 0) && (rIter != m_listIEC9_1CbGroup.rend()); rIter++)
        {
            CIEC9_1Cb* pIEC9_1Cb = *rIter;
            if (pIEC9_1Cb && pIEC9_1Cb->IsSelected())
            {
                pIEC9_1Cb->Selected(false);
                i--;
            }
        }
    }

    // 修正IEC61850-9-2控制块勾选数
    iSelCount = GetSelectedIEC9_2CbCount();
    if (iSelCount > iMaxSel9_2CbCount)
    {
        int i = iSelCount - iMaxSel9_2CbCount;
        std::list<CIEC9_2Cb*>::reverse_iterator rIter = m_listIEC9_2CbGroup.rbegin();
        for (; (i > 0) && (rIter != m_listIEC9_2CbGroup.rend()); rIter++)
        {
            CIEC9_2Cb* pIEC9_2Cb = *rIter;
            if (pIEC9_2Cb && pIEC9_2Cb->IsSelected())
            {
                pIEC9_2Cb->Selected(false);
                i--;
            }
        }
    }

    // 修正FT3控制块勾选数
    iSelCount = GetSelectedFt3CbCount();
    if (iSelCount > iMaxSelFt3CbCount)
    {
        int i = iSelCount - iMaxSelFt3CbCount;
        std::list<CFT3Cb*>::reverse_iterator rIter = m_listFt3CbGroup.rbegin();
        for (; (i > 0) && (rIter != m_listFt3CbGroup.rend()); rIter++)
        {
            CFT3Cb* pFt3Cb = *rIter;
            if (pFt3Cb && pFt3Cb->IsSelected())
            {
                pFt3Cb->Selected(false);
                i--;
            }
        }
    }

    // 修正FT3扩展控制块勾选数
    iSelCount = GetSelectedFt3ExpandCbCount();
    if (iSelCount > iMaxSelFt3ExtCbCount)
    {
        int i = iSelCount - iMaxSelFt3ExtCbCount;
        std::list<CFT3ExpandCb*>::reverse_iterator rIter = m_listFt3ExpandCbGroup.rbegin();
        for (; (i > 0) && (rIter != m_listFt3ExpandCbGroup.rend()); rIter++)
        {
            CFT3ExpandCb* pFt3ExpandCb = *rIter;
            if (pFt3ExpandCb && pFt3ExpandCb->IsSelected())
            {
                pFt3ExpandCb->Selected(false);
                i--;
            }
        }
    }
}
