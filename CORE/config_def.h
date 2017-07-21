#ifndef config_def_h__
#define config_def_h__

#define CONFNIG_RAW_DATA_SIZE 4

enum OTA_FLAG_ENUM {
	OTA_FLAG_INIT,
	OTA_FLAG_DOWNLOAD,
	OTA_FLAG_DOWNLOAD_FINISH,
};

struct CONFIG_DATA {
	char version;
	
	char ipaddress[32];
	char ipport[8];
	
	unsigned int TIME1;
	unsigned int TIME2;
	unsigned int LASTTIME_PZ;
	unsigned int LASTTIME_COMM;
	
	unsigned char alarm_flag;		//0： 1： 2：已经解除报警
	
	char trace[512];
	unsigned int trace_time;
	unsigned short last_voltage;
	
	char OTA_FLAG;
	//
};




#endif



