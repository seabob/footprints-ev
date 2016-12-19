#include <ResultSet.h>

boolean create_mysqlpool(void)
{
	url = URL_new(MYSQL_URL_STR);
	mysql_pool = ConnectionPool_new(url);
	ConnectionPool_start(mysql_pool);
	return TRUE;
}

void destroy_mysqlpool(void)
{
	ConnectionPool_free(&mysql_pool);
	URL_free(&url);
}

void do_mysql_process(void *data)
{
	Connection_T connect = ConnectionPool_getConnection(mysql_pool);
	char *buf = (char*)data;
	TRY
	{
		Connection_execute(conenction,buf);
	}
	CATCH(SQLException)
	{
		
	}
	FINALLY
	{

	}
	END_TRY;
}
