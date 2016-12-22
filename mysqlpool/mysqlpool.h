#ifndef __MYSQLPOOL_H__
#define __MYSQLPOOL_H__
#include <ResultSet.h>
#include "../types_define.h"

#define MYSQL_URL_STR	"mysql://localhost/footprints?user=root&password=112233&charset=utf8"
ConnectionPool_T mysql_pool;
URL_T url;
typedef void(*cmd_process_cb)(u8_t*) cmd_cb_t;

boolean create_mysqlpool(void);
void destroy_mysqlpool(void);
void do_mysql_process(void*);


#endif


