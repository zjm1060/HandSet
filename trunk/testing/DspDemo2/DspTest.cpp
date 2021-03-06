#include "stdafx.h"
#include "DspTest.h"
extern "C"
{
#include <assert.h>
#include "../../dspsrc/DSM800/src/calc/calc.h"
};
#include "../../src/dspclient/include/Dsp.h"
#include "DataManager.h"

#include "src\service\datalayer\include\config.h"
#include "src\utils\PcapFile\include\pcap_file_i.h"
#include "src\utils\MessageCodec\include\msg_rapid_decoder.h"
#include "src\utils\MessageCodec\include\sav92_msg.h"


bool CDspTest::m_fRun = false;
DWORD CDspTest::SendSmvThread( LPVOID pArg )
{
    CDsp *pDsp = static_cast<CDsp *>(pArg);
    SmvData *pSd = new SmvData;
    pSd->nChannelCount = 30;
    pSd->nSmvPerCircle = 80;
    for(int i=0; i<30; i+=2)
    {
        pSd->fScaleFactor[i] = 1.0;
        pSd->fScaleFactor[i+1] = 1.0;
        pSd->nChannelVCType[i] = SMV_VCTYPE_VOLTAGE;
        pSd->nChannelVCType[i+1] = SMV_VCTYPE_CURRENT;
    }
    pSd->nChannelVCType[0] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[1] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[2] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[3] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[4] |= SMV_VCTYPE_PHASE_C;
    pSd->nChannelVCType[5] |= SMV_VCTYPE_PHASE_C;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={0,30,90,120};
    float fWidths[4]={220,30,50,20};
    sinSamplesCreate(pSd->fChannelData[0],256,4000, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);

    for(int i=1; i<30; i++)
    {
        memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
            256*sizeof(float));
    }

    fInitPhaseAngles[0] = 120;
    sinSamplesCreate(pSd->fChannelData[2],256,4000, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);

    fInitPhaseAngles[0] = 240;
    sinSamplesCreate(pSd->fChannelData[4],256,4000, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);

    while(m_fRun)
    {
        bool fRet = pDsp->SendSmv(*pSd);
    }
    return 0;
}
DWORD CDspTest::SendSmvThread1( LPVOID pArg )
{

    CDsp *pDsp = static_cast<CDsp *>(pArg);
    SmvData *pSd = new SmvData;
    pSd->nChannelCount = 30;
    pSd->nSmvPerCircle = 80;
    for(int i=0; i<30; i+=2)
    {
        pSd->fScaleFactor[i] = 1;
        pSd->fScaleFactor[i+1] = 1;
        pSd->nChannelVCType[i] = SMV_VCTYPE_VOLTAGE;
        pSd->nChannelVCType[i+1] = SMV_VCTYPE_CURRENT;
    }
    pSd->nChannelVCType[0] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[1] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[2] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[3] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[4] |= SMV_VCTYPE_PHASE_C;
    pSd->nChannelVCType[5] |= SMV_VCTYPE_PHASE_C;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};

    static float test =0;
    test += 5;

    sinSamplesCreate(pSd->fChannelData[0],800,4000, test,fOmigas, 
        fInitPhaseAngles, fWidths, 4);
    for(int i=1; i<30; i++)
    {
        memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
            80*sizeof(float));
    }

    SmvGenData *pSgd = new SmvGenData;

    float fRmsValues[4];
    for(int i=0; i<4; i++)
    {
        fRmsValues[i] = fWidths[i]/sqrt(3);
    }

    pSgd->nChannelCount = 12;
    pSgd->nSampleRate = 4000;
    pSgd->nSmvCount = 80;
    //memset(&pSgd->nChannelVCType[0], SMV_VCTYPE_VOLTAGE, 8*sizeof(uint8_t));
    //memset(&pSgd->nChannelVCType[8], SMV_VCTYPE_CURRENT, 8*sizeof(uint8_t));
    memset(pSgd->nHarmCount , 4, pSgd->nChannelCount*sizeof(uint8_t));
    for(int i=0; i<pSgd->nChannelCount; i++)
    {
        pSgd->fScaleFactor[i]=1.0;
        pSgd->fDCValue[i]=35.0;
        memcpy(pSgd->fRmsValue[i], fRmsValues, 4*sizeof(float));
        memcpy(pSgd->fPhaseAngle[i], fInitPhaseAngles, 4*sizeof(float));
        memcpy(pSgd->fFreq[i], fOmigas, 4*sizeof(float));
    }

    srand(GetTickCount());

    Config *pCf = new Config;
    //pCf->nSmvCalcCount = 800;
    pCf->nMsgTimeCalcCount = 4000;
    pCf->nASDUCount = 1;
    
    while(m_fRun)
    {
        test += 5;

        sinSamplesCreate(pSd->fChannelData[0],800,4000, test,fOmigas, 
            fInitPhaseAngles, fWidths, 4);
        for(int i=1; i<30; i++)
        {
            memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
                80*sizeof(float));
        }
        bool fRet = pDsp->SendSmv(*pSd);
        //pDsp->SendSmvGen(*pSgd);
        /*mt.nSecond = rand();
        mt.nMicrosecond = rand();
        pDsp->SendMsgTime(mt);*/
        //assert(fRet);
        //Sleep(5);
//         pDsp->SetConfig(*pCf);
//         pDsp->GetDspInfo();
//         pDsp->GetDspInfo();
//         pDsp->GetDspInfo();
//         pDsp->GetDspInfo();
    }   
    delete pSd;
    delete pSgd;
    return 0;
}
void CDspTest::SetConfig()
{
    Config *pCf = new Config;
    pCf->nMsgTimeCalcCount = 468;
    pCf->nASDUCount = 1;
    
    pCf->nSamplingFreq = 4000;
    pCf->nRatedFreq = 50;

    bool fRet = m_pDsp->SetConfig(*pCf);
    assert(fRet);
    delete pCf;
}

#if 0
///
/// 35+220*sin(2*pi*50*(n-1)/4000-60/180*pi)+30*sin(2*pi*100*(n-1)/4000+30/180*pi)
/// +50*sin(2*pi*200*(n-1)/4000+90/180*pi)+20*sin(2*pi*300*(n-1)/4000+120/180*pi)
/// 
/// −73.20508075688771,-68.98254325029008,−71.13634777211134,−77.20034096062287
void CDspTest::SendSmvGen()
{
    SmvGenData *pSgd = new SmvGenData;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};
    float fRmsValues[4];
    for(int i=0; i<4; i++)
    {
        fRmsValues[i] = fWidths[i]/sqrt(3);
    }
    
    pSgd->nChannelCount = 12;
    pSgd->fSampleRate = 4000;
    pSgd->nSmvCount = 256;
    memset(pSgd->nHarmCount , 4, pSgd->nChannelCount*sizeof(uint8_t));
    for(int i=0; i<pSgd->nChannelCount; i++)
    {
        pSgd->fScaleFactor[i]=1.0;
        pSgd->fDCValue[i]=35.0;
        memcpy(pSgd->fRmsValue[i], fRmsValues, 4*sizeof(float));
        memcpy(pSgd->fPhaseAngle[i], fInitPhaseAngles, 4*sizeof(float));
        memcpy(pSgd->fFreq[i], fOmigas, 4*sizeof(float));
    }

    bool fRet = m_pDsp->SendSmvGen(*pSgd);
    assert(fRet);  
    delete pSgd;
}
#endif

DWORD CDspTest::SendMsgTimeThread( LPVOID pArg )
{
    MsgTimeData mtd;
    mtd.nMsgTimeCount = 468;
    CDsp *pDsp = static_cast<CDsp *>(pArg);
    srand(GetTickCount());
    while(m_fRun)
    {
        // 模拟存储多个报文时间数据
        mtd.msgTimes[0].nSecond = 0;
        mtd.msgTimes[0].nMicrosecond = 0;
        for(int i = 1; i<mtd.nMsgTimeCount; i++)
        {
            mtd.msgTimes[i].nSecond = mtd.msgTimes[i-1].nSecond;
            mtd.msgTimes[i].nMicrosecond = mtd.msgTimes[i-1].nMicrosecond+rand()%499+1;
            //进位
            if(mtd.msgTimes[i].nMicrosecond>1000000)
            {
                mtd.msgTimes[i].nMicrosecond=1000000;
                mtd.msgTimes[i].nSecond++;
            }
            //TRACE(L"%d\t", mtd.msgTimes[i].nMicrosecond);
        }
        //TRACE(L"\r\n\r\n");
        bool fRet = pDsp->SendMsgTime(mtd);
        Sleep(30);
    }
    return 0;
}

void CDspTest::GetInfo()
{  
    bool fRet  = m_pDsp->GetDspInfo();
    assert(fRet);
}

void CDspTest::ResetCache()
{
    bool fRet = m_pDsp->ResetCache(0);
    assert(fRet);
	fRet = m_pDsp->ResetCache(1);
	assert(fRet);
}

CDspTest::CDspTest()
:m_hThreadSmv(NULL)
,m_hThreadMsgTime(NULL)
{

    CDspDataSet &dspDataSet = CDataManager::GetInstance().GetDspDataSet();
    m_pDsp = new CDsp(&dspDataSet);

    m_pDsp1 = new CDsp();
}

CDspTest::~CDspTest( void )
{
    m_fRun = false;
#if 1
    delete m_pDsp;
    delete m_pDsp1;
#endif
}

void CDspTest::RunTest()
{
    TRACE(L"FrameHead:%d\n",sizeof(FrameHead));
    TRACE(L"Error:%d\n",sizeof(Error));
    TRACE(L"DspInfo:%d\n",sizeof(DspInfo));
    TRACE(L"SmvData:%d\n",sizeof(SmvData));
    TRACE(L"SMV_DATA_CACL_RT:%d\n",sizeof(SMV_DATA_CALC_RT));
    TRACE(L"SMV_GEN_DATA_CACL_RT:%d\n",sizeof(SMV_GEN_DATA_CALC_RT));
    
    if(m_fRun == false)
    {
        m_fRun = true;
               
        m_pDsp->StartRead();

        ResetCache();
        SetConfig();

        //SmvGenCalc();
#if 1
        //m_hThreadSmv = CreateThread(NULL, 0, SendSmvThread, m_pDsp, 0, NULL);

        //m_hThreadSmv = CreateThread(NULL, 0, SendSmvThread1, m_pDsp, 0, NULL);
        m_hThreadSmv = CreateThread(NULL, 0, SendSmvThreadPcap, m_pDsp, 0, NULL);

//        m_hThreadMsgTime = CreateThread(NULL, 0, SendMsgTimeThread, m_pDsp, 0, NULL);
#endif
    }
    if(m_fRun == true)
    {        
#if 1             
		//SendSmv();
		//Sleep(1000);
        //ResetCache();
        //SetConfig();
        //GetInfo();
        //SendSmvGen();
#endif  
    }
    //SendSmv();
}

void CDspTest::ExitTest()
{
#if 1
    if(m_fRun == true)
    {
        m_fRun = false;        
#if 1
        DWORD dwResult = WaitForSingleObject(m_hThreadSmv,100);
        if(WAIT_OBJECT_0 != dwResult)
        {
            TerminateThread(m_hThreadSmv, -1);
        }
        dwResult = WaitForSingleObject(m_hThreadMsgTime, 100);
        if(WAIT_OBJECT_0 != dwResult)
        {
            TerminateThread(m_hThreadMsgTime, -1);
        }        
        CloseHandle(m_hThreadSmv);
        CloseHandle(m_hThreadMsgTime);    
        m_pDsp->StopRead();
#endif
    }
#endif
}

void CDspTest::SendSmv()
{
    CDsp *pDsp = this->m_pDsp;
    SmvData *pSd = new SmvData;
    pSd->nChannelCount = 30;
    pSd->nSmvPerCircle = 256;
    for(int i=0; i<30; i+=2)
    {
        pSd->fScaleFactor[i] = 1.0;
        pSd->fScaleFactor[i+1] = 1.0;
        pSd->nChannelVCType[i] = SMV_VCTYPE_VOLTAGE;
        pSd->nChannelVCType[i+1] = SMV_VCTYPE_CURRENT;
    }
    pSd->nChannelVCType[0] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[1] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[2] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[3] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[4] |= SMV_VCTYPE_PHASE_C;
    pSd->nChannelVCType[5] |= SMV_VCTYPE_PHASE_C;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};
    sinSamplesCreate(pSd->fChannelData[0],256,4000, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);
    for(int i=1; i<30; i++)
    {
        memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
            256*sizeof(float));
    }
    for(int i=0; i<10; i++)
    {
        pDsp->SendSmv(*pSd);
        Sleep(20);
    }
}
///
/// 35+220*sin(2*pi*50*(n-1)/4000-60/180*pi)+30*sin(2*pi*100*(n-1)/4000+30/180*pi)
/// +50*sin(2*pi*200*(n-1)/4000+90/180*pi)+20*sin(2*pi*300*(n-1)/4000+120/180*pi)
/// 
/// −73.20508075688771,-68.98254325029008,−71.13634777211134,−77.20034096062287
void CDspTest::SmvGenCalc()
{
    SMV_GEN_DATA_CALC_RT *pSgdRT = new SMV_GEN_DATA_CALC_RT;
    SmvGenData *pSgd = new SmvGenData;    
    
    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};
    float fRmsValues[4];
    for(int i=0; i<4; i++)
    {
        fRmsValues[i] = fWidths[i]/sqrt(3);
    }

    pSgd->nChannelCount = 12;
    pSgd->nSampleRate = 4000;
    pSgd->nSmvCount = 256;
    memset(pSgd->nHarmCount , 4, pSgd->nChannelCount*sizeof(uint8_t));
    for(int i=0; i<pSgd->nChannelCount; i++)
    {
        pSgd->fScaleFactor[i]=1.0;
        pSgd->fDCValue[i]=35.0;
        memcpy(pSgd->fRmsValue[i], fRmsValues, 4*sizeof(float));
        memcpy(pSgd->fPhaseAngle[i], fInitPhaseAngles, 4*sizeof(float));
        memcpy(pSgd->fFreq[i], fOmigas, 4*sizeof(float));
    }

    pSgd->fDCValue[0] = 0.0f;
    pSgd->fRmsValue[0][0] = 0.008;
    pSgd->fPhaseAngle[0][0] = 0.0f;
    pSgd->fFreq[0][0] = 50.0f;
    pSgd->nHarmCount[0] = 1;

    int fRet = m_pDsp1->SmvGenCalc(1000, *pSgd, pSgdRT);
    assert(fRet == 0);

    fRet = m_pDsp1->WaitForObject(CDsp::T_SMV_GEN, 1000);
    assert(fRet == 0);
    TRACE(L"SmvGenData[0]:%f\n", pSgdRT->fSmvGenData[0][0]);

    CDspDataSet &dspDataSet = CDataManager::GetInstance().GetDspDataSet();
    dspDataSet.Write(DSP2ARM_CONTROLCODE_SMV_GEN_RT, pSgdRT, sizeof(SMV_GEN_DATA_CALC_RT));

    delete pSgd;
    delete pSgdRT;
}


uint8_t g_nChannelType[NET_9_2_LE_MAX_CHANNELS_PER_ASDU] = 
{
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_B,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_C,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A,
    SMV_VCTYPE_VOLTAGE | SMV_VCTYPE_PHASE_A
};

DWORD CDspTest::SendSmvThreadPcap( LPVOID pArg )
{
    uint8_t pBuffer[NET_MAX_FRAME_LEN];
    std::wstring strPcap = L"\\FlashDisk2\\sv92_20000101_005452_216.pcap";
    uint32_t nSampleRate = 4000;
    uint32_t nFreq = 50;

    CDsp *pDsp = static_cast<CDsp *>(pArg);

    Config stDspConfig;
    stDspConfig.nMsgTimeCalcCount = 468;
    stDspConfig.nASDUCount = 1;
    stDspConfig.nSamplingFreq = 4000;
    stDspConfig.nRatedFreq = 50;
    if (!pDsp->SetConfig(stDspConfig))
    {
        RETAILMSG(1,(_T("Dsp Config Failed \r\n")));
        return 2;
    }

    Sav92Msg* pSav92Msg = CreateSav92MsgImpl();
    int32_t  p92SmvValue[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];
    uint32_t p92SmvQlt[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];
    uint32_t ntimes = 0;

    SmvData* pSmvData = new SmvData;
    IPcapFile *pCap = CreatePcapFileImpl();
    CDspDataSet &dspDataSet = CDataManager::GetInstance().GetDspDataSet();

    if(PCAP_FILE_SUCCESS == pCap->Open(strPcap, PFM_READ))
    {
        uint32_t nPackCount = pCap->GetPckNum();

        PCAP_PACKET_HEADER pckhead;
        uint32_t nPointIndex = 0;

        for (uint32_t i = 0; i < nPackCount; ++i)
        {
            pCap->SeekPckToRead(i);
            if (PCAP_FILE_SUCCESS != pCap->ReadOnePckHeader(pckhead))
            {
                OutputDebugString(_T("Pcap File read  pack Failed \n"));
                continue;
            }

            ASSERT(NET_MAX_FRAME_LEN > pckhead.capLen);

            // 获取帧数据
            if(PCAP_FILE_SUCCESS != pCap->ReadOnePckData(pBuffer, pckhead.capLen))
            {
                continue;
            }

            // 获取帧类型
            static MsgRapidDecoder rapidDecoder;
            enumFrameType enFrameType = rapidDecoder.GetMessageType(pBuffer);
            if (enFrameType == FRAME_TYPE_NET_9_2)
            {
                if (0 == pSav92Msg->Decode(pBuffer, pckhead.capLen))
                {
                    RETAILMSG(1,(_T("Sav9_2 Decode Failed \r\n")));
                    continue;
                }
                uint16_t noASDU = pSav92Msg->GetNoASDU();
                for ( uint16_t j = 0; j < noASDU; ++j)
                {
                    size_t nChannelCount = pSav92Msg->GetASDUSmpsNum(j);

                    if (nChannelCount > NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
                    {
                        RETAILMSG(1, (_T("9-2 Channel Count > 30 \r\n")));
                        continue;
                    }

                    pSav92Msg->GetASDUSmps(j, reinterpret_cast<uint32_t*>(p92SmvValue), p92SmvQlt, nChannelCount);

                    for (uint16_t k = 0; k < nChannelCount; ++k)
                    {
                        pSmvData->fChannelData[k][nPointIndex] = p92SmvValue[k]/841.0f;
                    }
                    nPointIndex++;
                    if (nPointIndex == nSampleRate/nFreq)
                    {
                        pSmvData->nChannelCount  = nChannelCount;
                        pSmvData->nSmvPerCircle = nPointIndex;
                        for (uint32_t m = 0; m < nChannelCount; ++m)
                        {
                            pSmvData->fScaleFactor[m] = 1.0f;
                            pSmvData->nChannelVCType[m] = g_nChannelType[m];
                        }
                        pDsp->SendSmv(*pSmvData);
                        nPointIndex = 0;
                        ntimes++;
                        static uint32_t nRecvCountLast = 0;
                        uint32_t nRecvCount = dspDataSet.GetDataCount();
                        if (nRecvCountLast != nRecvCount)
                        {
                            nRecvCountLast = nRecvCount;
                            RETAILMSG(1,(_T("Send Svm %d Times, Recv %d Times \r\n"), ntimes, nRecvCount));
                        }
                        Sleep(20);
                    }
                }
            }
        }
    }

    delete pSmvData;
    pSmvData = NULL;
    ReleaseSav92MsgImpl(pSav92Msg);
    ReleasePcapFileImpl(pCap);
    return 1;
}
