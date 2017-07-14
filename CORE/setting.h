#ifndef __setting_h__
#define __setting_h__

#define BUILDTIME __DATETIME__

// 版本号定义
#define MAJOR_VERSION_NUMBER			0
#define MINOR_VERSION_NUMBER			0

#define REVISION_NUMBER						0


#define BUILD_NUMBER							0

//图片分包大小
#define IMG_PKG_SIZE 							(512)

//包重传次数
#define PKG_RETRANS_COUNT					10
#define WAIT_UDP_PKG_TIME					10000

//服务器信息 
#define SERVER_ADDR								"52.80.4.27"
#define SERVER_PORT								29100
//#define SERVER_ADDR							"118.190.173.234"
//#define SERVER_PORT							8888

//拍照上传配置
#define PAIZHAO_COUNT							1 //拍照次数
#define PAIZHAO_INTERVAL					5 //拍照时间间隔

#define VOL_REF										3300 //ADC电压基准

#define FLASH_TRACE								//使用flash作为Trace记录器,生产环境请务必注释掉，因为会增加待机功耗，并减少flash读写寿命
//#define NO_ALARM								//关闭房差报警功能

#define FW_VERSION								0x0000

//#define OLD_BRD

#endif