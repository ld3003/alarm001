#include "config.h"
#include "JSON/cjson.h"
#include "flash.h"
#include "common.h"
#include "mem.h"

char * data_2_json(struct CONFIG_DATA * in_data)
{
	char * p;
	cJSON * pJsonRoot = NULL;
	char tmpstr[32];
 
//user_app_conf
	
	DEBUG_ERROR(0);

	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot){return 0;}
	
	cJSON_AddNumberToObject(pJsonRoot, "version", mem->cfg_data.version);
	cJSON_AddStringToObject(pJsonRoot, "ipaddress", mem->cfg_data.ipaddress);
	cJSON_AddStringToObject(pJsonRoot, "ipport", mem->cfg_data.ipport);
	
	cJSON_AddNumberToObject(pJsonRoot, "TIME1", mem->cfg_data.TIME1);
	cJSON_AddNumberToObject(pJsonRoot, "TIME2", mem->cfg_data.TIME2);
	cJSON_AddNumberToObject(pJsonRoot, "LASTTIME_PZ", mem->cfg_data.LASTTIME_PZ);
	cJSON_AddNumberToObject(pJsonRoot, "LASTTIME_COMM", mem->cfg_data.LASTTIME_COMM);
	//LASTTIME_PZ
	//LASTTIME_COMM
	
	cJSON_AddNumberToObject(pJsonRoot, "last_voltage", mem->cfg_data.last_voltage);
	
	cJSON_AddNumberToObject(pJsonRoot, "alarm_flag", mem->cfg_data.alarm_flag);
	cJSON_AddNumberToObject(pJsonRoot, "trace_time", mem->cfg_data.trace_time);
	cJSON_AddStringToObject(pJsonRoot, "trace",  mem->cfg_data.trace);
	
	cJSON_AddStringToObject(pJsonRoot, "JSON_FLAG", "JSON");
	DEBUG_ERROR(0);
	p = cJSON_Print(pJsonRoot);
	DEBUG_ERROR(0);
	// else use : 
	// char * p = cJSON_PrintUnformatted(pJsonRoot);
	if(NULL == p)
	{
			//convert json list to string faild, exit
			//because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
			DEBUG_ERROR(0);
			cJSON_Delete(pJsonRoot);
			DEBUG_ERROR(0);
			return 0;
	}
	//free(p);
	DEBUG_ERROR(0);
	cJSON_Delete(pJsonRoot);
	
	DEBUG_ERROR(0);
	
	return p;
	//
}
void json_2_data(char * in_json , struct CONFIG_DATA * out_data)
{
		cJSON * pSub;
		cJSON * pJson;
    if(NULL == in_json){return;}
    pJson = cJSON_Parse(in_json);
    if(NULL == pJson){return;}
		
		pSub = cJSON_GetObjectItem(pJson, "version");
    if(NULL != pSub)
    {mem->cfg_data.version = pSub->valueint;}
		
		pSub = cJSON_GetObjectItem(pJson, "ipaddress");
    if(NULL != pSub)
    {snprintf(mem->cfg_data.ipaddress,sizeof(mem->cfg_data.ipaddress),"%s",pSub->valuestring);}
		
		pSub = cJSON_GetObjectItem(pJson, "trace");
    if(NULL != pSub)
    {snprintf(mem->cfg_data.trace,sizeof(mem->cfg_data.trace),"%s",pSub->valuestring);}
		
		pSub = cJSON_GetObjectItem(pJson, "ipport");
    if(NULL != pSub)
    {snprintf(mem->cfg_data.ipaddress,sizeof(mem->cfg_data.ipport),"%s",pSub->valuestring);}
		
		pSub = cJSON_GetObjectItem(pJson, "TIME1");
    if(NULL != pSub)
    {mem->cfg_data.TIME1 = pSub->valueuint;}
		
		pSub = cJSON_GetObjectItem(pJson, "TIME2");
    if(NULL != pSub)
    {mem->cfg_data.TIME2 = pSub->valueuint;}
		
		pSub = cJSON_GetObjectItem(pJson, "LASTTIME_PZ");
    if(NULL != pSub)
    {mem->cfg_data.LASTTIME_PZ = pSub->valueuint;}
		
		pSub = cJSON_GetObjectItem(pJson, "LASTTIME_COMM");
    if(NULL != pSub)
    {mem->cfg_data.LASTTIME_COMM = pSub->valueuint;}
		
		pSub = cJSON_GetObjectItem(pJson, "last_voltage");
    if(NULL != pSub)
    {mem->cfg_data.last_voltage = pSub->valueuint;}
		
		pSub = cJSON_GetObjectItem(pJson, "alarm_flag");
    if(NULL != pSub)
    {mem->cfg_data.alarm_flag = pSub->valueuint;}
		
		pSub = cJSON_GetObjectItem(pJson, "trace_time");
    if(NULL != pSub)
    {mem->cfg_data.trace_time = pSub->valueuint;}
		
		cJSON_Delete(pJson);
		
		return ;
	//
}

void write_config(void)
{
	char *json;
	//json = (char*)alloc_mem(1024*2);
	json =data_2_json(&mem->cfg_data);
	
	if (json > 0)
	{
	
		printf("make json %d -- %s \r\n",strlen(json) , json);
		
		if (strlen(json) < (CONFIG_DATA_SIZE - CONFNIG_RAW_DATA_SIZE - 4))
		{
			FLASH_ProgramStart(CONFIG_DATA_ADDR,CONFIG_DATA_SIZE);
			FLASH_AppendBuffer(mem->cfg_rawdata,CONFNIG_RAW_DATA_SIZE);
			FLASH_AppendBuffer((unsigned char*)json,strlen(json));
			FLASH_AppendEnd();
			FLASH_ProgramDone();
			
		}else{
			
		}
		
		free(json); //json free;
		
		//free_mem(__FILE__,__LINE__,(unsigned char*)json);
	}
}
struct CONFIG_DATA *read_config(void)
{
	char *json;
	json = (char*)alloc_mem(__FILE__,__LINE__,CONFIG_DATA_SIZE - CONFNIG_RAW_DATA_SIZE);
	//json = (char*)CONFIG_DATA_ADDR;
	
	memcpy(mem->cfg_rawdata,(char*)CONFIG_DATA_ADDR,CONFNIG_RAW_DATA_SIZE);
	memcpy(json,(char*)CONFIG_DATA_ADDR + CONFNIG_RAW_DATA_SIZE , CONFIG_DATA_SIZE - CONFNIG_RAW_DATA_SIZE);
	json[CONFIG_DATA_SIZE - CONFNIG_RAW_DATA_SIZE - 1] = 0x0;
	
	//检查是不是合法的JSON字符串
	if (strstr(json,"JSON_FLAG"))
	{
		json_2_data(json,&mem->cfg_data);
	}else{
		
		//如果不是合法的字符串恢复默认
		memset(&mem->cfg_data,0x0,sizeof(struct CONFIG_DATA));
		sprintf(mem->cfg_data.ipaddress,"");
		sprintf(mem->cfg_data.ipport,"");
		
	}
	free_mem(__FILE__,__LINE__,(unsigned char*)json);
	
	return &mem->cfg_data;
}