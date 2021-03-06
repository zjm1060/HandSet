/// @file
///
/// @brief
///     定义DeviceIoControl使用的命令代码
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.30
///
/// 修改历史：
///

#ifndef __CTRL_CODE_DEF_H
#define __CTRL_CODE_DEF_H

//使用 CTL_CODE 宏,sdk中暂时没有这个文件，自己拷贝到工程目录
#include "windev.h"

//应用层操作宏指令
#define IOCTRL_GET_SYNCTIME_MODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_SYNCTIME_MODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_START_FPGA  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0306, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_LOWER_POWER  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0309, METHOD_BUFFERED, FILE_ANY_ACCESS)

//获取驱动的版本号  格式固定为"Vx.xx"
#define IOCTRL_GET_DRV_VERSION  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0307, METHOD_BUFFERED, FILE_ANY_ACCESS)
//获取当前FPGA的UTC时间
#define IOCTRL_GET_UTC_TIME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0308, METHOD_BUFFERED, FILE_ANY_ACCESS)
//设置当前FPGA的UTC时间
#define IOCTRL_SET_UTC_TIME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030A, METHOD_BUFFERED, FILE_ANY_ACCESS)
//获取光以太网口的光功率值
#define IOCTRL_GET_OPTICAL_POWER  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030B, METHOD_BUFFERED, FILE_ANY_ACCESS)
// 获取光口的连接状态
#define IOCTRL_GET_OPTICAL_STATE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030C, METHOD_BUFFERED, FILE_ANY_ACCESS)
//获取B码时钟的控制码元值
#define IOCTRL_GET_IRIGB_CONTROL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030D, METHOD_BUFFERED, FILE_ANY_ACCESS)
//设置FT3发送端口发送报文的类型
#define IOCTRL_SET_FT3_TX_STYLE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030E, METHOD_BUFFERED, FILE_ANY_ACCESS)
//设置时钟偏差值
#define IOCTRL_SET_ADJUST_VAL  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030F, METHOD_BUFFERED, FILE_ANY_ACCESS)


//接收报文相关处理命令
//设置接受数据的配置信息
#define IOCTRL_DETECT   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0381, METHOD_BUFFERED, FILE_ANY_ACCESS)
//设置接受数据的配置信息
#define IOCTRL_FILTER   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0382, METHOD_BUFFERED, FILE_ANY_ACCESS)
//设置接受数据
#define IOCTRL_SET_RECV_LOOP   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0383, METHOD_BUFFERED, FILE_ANY_ACCESS)
//停止接受数据
#define IOCTRL_STOP_RECV   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0384, METHOD_BUFFERED, FILE_ANY_ACCESS)
//设置接收超时
#define IOCTRL_SET_RX_TIMEOUT \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0385, METHOD_BUFFERED, FILE_ANY_ACCESS)

//发送报文相关控制命令
//应用程序请求发送报文
#define IOCTRL_SEND_FRAME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0389, METHOD_BUFFERED, FILE_ANY_ACCESS)
//应用程序配置发送参数        
#define IOCTRL_SEND_HEADER \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038A, METHOD_BUFFERED, FILE_ANY_ACCESS)
//更改MU的缓冲区
#define IOCTRL_CHANGE_MU_FRAME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038B, METHOD_BUFFERED, FILE_ANY_ACCESS)
//应用程序请求发送报文
#define IOCTRL_STOP_SEND  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038C, METHOD_BUFFERED, FILE_ANY_ACCESS)

//设置1588主时钟
#define IOCTRL_IEEE1588_MAIN_CLOCK  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0390, METHOD_BUFFERED, FILE_ANY_ACCESS)

//硬件定时器
#define IOCTL_TMR_SETPERIOD \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TMR_REGISTER  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TMR_UNREGISTER \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0802, METHOD_BUFFERED, FILE_ANY_ACCESS)

//定义各个接口的连通，通讯状态位在状态字中的位置数值
#define NET_PORT_LIGHT_A_LINK                (1<<0)
#define NET_PORT_LIGHT_A_ACTIVE              (1<<1)
#define NET_PORT_LIGHT_B_LINK                (1<<2)
#define NET_PORT_LIGHT_B_ACTIVE              (1<<3)
#define NET_PORT_LIGHT_ELEC_LINK             (1<<4)
#define NET_PORT_LIGHT_ELEC_ACTIVE           (1<<5)
#define FT3_PORT_A_ACTIVE                    (1<<7)
#define FT3_PORT_B_ACTIVE                    (1<<8)
#define FT3_PORT_C_ACTIVE                    (1<<6)
#define LIGHT_PORT_IRIGB_ACTIVE              (1<<9)

#endif //end __CTRL_CODE_DEF_H