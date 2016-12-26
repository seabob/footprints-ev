#ifndef __OBD_STRUCTS_H__
#define __OBD_STRUCTS_H__

#include "types_define.h"

#define SET_FIELD(a,b,c) \
	{a->b = c;}

typedef struct OBD_head{
	u8_t head;
	u8_t cmd;
	u8_t id[4];
}head_t;

typedef struct OBD_head_without_id{
	u8_t head;
	u8_t cmd;
}head_no_id_t;

#define SET_HEAD_FIELD(h,filed,data) \
	SET_FIELD(h,field,data)

typedef struct OBD_end{
	u8_t checksum;
	u8_t end;
}end_t;

#define SET_END_FIELD(e,field,data) \
	SET_FIELD(e,field,data)

typedef struct cmd_struct{
	head_t head;
	end_t end;
}cmd_t;

typedef struct response_struct{
	head_t head;
	end_t end;
}response_t;


//typedef struct cmd_struct_data{}cmd_data_t;
//typedef struct response_struct_data{}response_data_t;

typedef struct OBD_cmd0x00{
	head_no_id_t head;
	u8_t mcuid[12];
	u8_t imei[15];
	u8_t vin[17];
	u8_t iccid[20];
	u8_t imsi[15];
	u8_t hw_version[2];
	u8_t sw_version[2];
	u8_t protocol_type[2];
	end_t end;
}cmd00_t;

typedef struct OBD_response0x00{
	head_no_id_t head;
	u8_t id[4];
	u8_t *ip;
	end_t end;
}response00_t;

typedef struct OBD_cmd0x01{
	head_t head;
	end_t end;
}cmd01_t;

typedef struct OBD_response0x01{
	head_t head;
	u8_t time[6];
	end_t end;
}response01_t;

typedef struct OBD_cmd0x02{
	head_t head;
	u8_t seed[4];
	end_t end;
}cmd02_t;

typedef struct OBD_response0x02{
	head_t head;
	u8_t private_key[4];
	end_t end;
}response02_t;

typedef struct OBD_cmd0x10{
	head_t head;
	u8_t frame_mark[4];
	u8_t param_id;
	u8_t*param; 
	end_t end;
}cmd10_t;

typedef struct OBD_response0x10{
	head_t head;
	u8_t frame_mark[4];
	u8_t param_id;
	u8_t *param;
	u8_t end;
}response10_t;

typedef struct OBD_cmd0x13{
	head_t head;
	end_t end;
}cmd13_t;
typedef struct OBD_response0x13{
	head_t head;
	u8_t mcuid[12];
	end_t end;
}response13_t;

typedef struct OBD_cmd0x14{
	head_t head;
	end_t end;
}cmd14_t;

typedef struct OBD_response0x14{
	head_t head;
	u8_t status;
	u8_t vin[17];
	end_t end;
}response14_t;

typedef struct OBD_cmd0x15{
	head_t head;
	end_t end;
}cmd15_t;

typedef struct OBD_response0x15{
	head_t head;
	u8_t protocol_algo;
	u8_t oil_algo;
	end_t end;
}response15_t;

typedef struct OBD_cmd0x16{
	head_t head;
	end_t end;
}cmd16_t;

typedef struct OBD_response0x16{
	head_t head;
	end_t end;
}response16_t;

typedef struct OBD_cmd0x17{
	head_t head;
	u8_t vin[17];
	end_t end;
}cmd17_t;

typedef struct OBD_response0x17{
	head_t head;
	u8_t vin[17];
	end_t end;
}response17_t;

typedef struct OBD_cmd0x18{
	head_t head;
	end_t end;
}cmd18_t;

typedef struct OBD_response0x18{
	head_t head;
	u8_t hw_version[2];
	u8_t sw_version[2];
	u8_t cat_type[2];
	end_t end;
}response18_t;

typedef struct OBD_cmd0x19{
	head_t head;
	end_t end;
}cmd19_t;

typedef struct OBD_response0x19{
	head_t head;
	u8_t iccid[20];
	end_t end;
}response19_t;

typedef struct OBD_cmd0x1a{
	head_t head;
	end_t end;
}cmd1a_t;

typedef struct OBD_response0x1a{
	head_t head;
	u8_t imsi[15];
	end_t end;
}response1a_t;

typedef struct OBD_cmd0x1b{
	head_t head;
	end_t end;
}cmd1b_t;

typedef struct OBD_response0x1b{
	head_t head;
	u8_t imei[15];
	end_t end;
}response1b_t;

typedef struct OBD_cmd0x22{
	head_t head;
	u8_t time[6];
	u8_t id[2];
	u8_t gps_state;
	u8_t lat_long[8];
	u8_t speed;
	u8_t direction[2];
	u8_t event_id;
	u8_t *param;
	end_t end;
}cmd22_t;

typedef struct OBD_response0x22{
	head_t head;
	u8_t time[6];
	u8_t event_id;
	end_t end;
}response22_t;

typedef struct OBD_cmd0x21{
	head_t head;
	u8_t operator_id[4];
	u8_t operator_password[4];
	u8_t action_id[2];
	u8_t action_type;
	end_t end;
}cmd21_t;

typedef struct OBD_response0x21{
	head_t head;
	u8_t operator_id[4];
	u8_t operator_password[4];
	u8_t action_id[2];
	u8_t action_type;
	u8_t result;
	end_t end;
}response21_t;

typedef struct OBD_cmd0x30{
	head_t head;
	u8_t group_id;
	end_t end;
}cmd30_t;

typedef struct OBD_response0x30{
	head_t head;
	u8_t time[6];
	u8_t trip_id[2];
	u8_t group_id;
	u8_t oil_quantity[2];
	u8_t idling_oil[2];
	u8_t mileage_quantity[2];
	u8_t high_speed;
	u8_t travel_time[4];
	u8_t hot_cat_time[4];
	u8_t trip_low_21km[2];
	u8_t trip_low_41km[2];
	u8_t trip_low_61km[2];
	u8_t trip_low_91km[2];
	u8_t trip_other[2];
	u8_t trip_score;
	u8_t dashboard_trip[4];
	end_t end;
}response30_t;

typedef struct OBD_cmd0x34{
	head_t head;
	u8_t clild_cmd;
	u8_t group_id;
	u8_t auto_upload_count;
	u8_t auto_upload_interval;
	end_t end;
}cmd34_t;

typedef struct OBD_response0x34{
	head_t head;
	u8_t child_cmd;
	u8_t group_id;
	u8_t auto_upload_count;
	u8_t auto_upload_interval;
	end_t end;
}response34_t;

typedef struct OBD_cmd0x42{
	head_t head;
	u8_t group_id;
	u8_t auto_upload_count;
	u8_t auto_upload_interval;
	u8_t param_count;
	u8_t *param_id[2];
	end_t end;
}cmd42_t;

typedef struct cmd0x42_param{
	u8_t id[2];
	u8_t support;
	u8_t *data;
}param42_t;

typedef struct OBD_response0x42{
	head_t head;
	u8_t time[6];
	u8_t group_id;
	u8_t param_count;
	param42_t *param;
	end_t end;
}response42_t;

typedef struct OBD_cmd0x43{
	head_t head;
	u8_t group_id;
	end_t end;
}cmd43_t;

typedef struct OBD_response0x43{
	head_t head;
	u8_t group_id;
	u8_t checksum;
	end_t end;
}response43_t;

typedef struct OBD_cmd0x32{
	head_t head;
	u8_t trip_id[2];
	u8_t gps_fix;
	u8_t longitude[4];
	u8_t latitude[4];
	u8_t gps_speed;
	u8_t gps_direction[2];
	u8_t engine_speed[2];
	u8_t obd_speed[2];
	u8_t gyro_direction[2];
	end_t end;
}cmd32_t;

typedef struct base_station_info{
	u8_t area_number[2];
	u8_t tower_number[2];
	s8_t singal_intensity;
}bstation_info_t;

typedef struct OBD_cmd0x33{
	head_t head;
	u8_t trip_id[2];
	u8_t country_number[2];
	u8_t carrieroperator_number[2];
	u8_t base_station_count;
	bstation_info_t station_info[6];
	u8_t engine_speed[2];
	u8_t obd_speed[2];
	u8_t gyro_direction[2];
	end_t end;
}cmd33_t;

typedef struct OBD_response0x33{
	head_t head;
	u8_t longitude[4];
	u8_t latitude[4];
	end_t end;
}response33_t;

typedef struct OBD_cmd0x50{
	head_t head;
	u8_t child_cmd;
	u8_t cmd_param;
	end_t end;
}cmd50_t;

typedef struct OBD_response0x50{
	head_t head;
	u8_t time[6];
	u8_t child_cmd;
	u8_t cmd_param;
	u8_t *fault_number;
	end_t end;
}response50_t;

typedef struct OBD_cmd0x07_00{
	head_t head;
	u8_t child_cmd;
	u8_t firmware_lens[4];
	u8_t firmware_crc32[4];
	end_t end;
}cmd07_00_t;

typedef struct OBD_response0x07_00{
	head_t head;
	u8_t child_cmd;
	u8_t cmd_param;
	end_t end;
}response07_00_t;

typedef struct OBD_cmd0x07_01{
	head_t head;
	u8_t child_cmd;
	u8_t data_lens[2];
	u8_t data_address[4];
	u8_t *data;
	end_t end;
}cmd07_01_t;

typedef struct OBD_response0x07_01{
	head_t head;
	u8_t child_cmd;
	u8_t cmd_param;
	end_t end;
}response07_01_t;

typedef struct OBD_cmd0x07_02{
	head_t head;
	u8_t child_cmd;
	end_t end;
}cmd07_02_t;

typedef struct OBD_response0x07_02{
	head_t head;
	u8_t child_cmd;
	u8_t cmd_param;
	end_t end;
}response07_02_t;

typedef struct OBD_cmd0x08_00{
	head_t head;
	u8_t child_cmd;
	u8_t longitude[4];
	u8_t latitude[4];
	u8_t altitude[4];
	end_t end;
}cmd08_00_t;

typedef struct OBD_cmd0x08_ff{
	head_t head;
	u8_t child_id;
	u8_t country_number[2];
	u8_t carrieroperator_number;
	bstation_info_t station_info[6];
	end_t end;
}cmd08_ff_t;

typedef struct OBD_cmd0x08_01{
	head_t head;
	u8_t child_cmd;
	u8_t frame_counter;
	u8_t *data;
	end_t end;
}cmd08_01_t;

typedef struct OBD_response0x08_01{
	head_t head;
	u8_t child_cmd;
	u8_t cmd_param;
	end_t end;
}response08_01_t;


static inline 











#endif
