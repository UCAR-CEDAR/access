// DODSMySQLChannel.cc

#include "DODSMySQLChannel.h"

DODSMySQLChannel::DODSMySQLChannel()
{
    _channel_open = false ;
    _has_log = false ;
    _the_channel = 0 ;
    mysql_init( &_mysql ) ;
}

DODSMySQLChannel::~DODSMySQLChannel()
{
    if( _channel_open )
    {
	mysql_close( _the_channel ) ;
    }
}

bool
DODSMySQLChannel::open( const char* server, const char* user,
			const char* password, const char* database)
{
    if( _channel_open )
	return true ;
    else
    {
	_server = server ;
	_user = user ;
	_database = database ;
	_the_channel = mysql_real_connect( &_mysql, server, user,
					   password, database, 0, 0, 0 ) ;
	if( !_the_channel )
	    return false ;
	else
	{
	    _channel_open = true ;
	    return true ;
	}
    }
}

void
DODSMySQLChannel::close()
{
    if( _channel_open )
    {
	mysql_close( _the_channel ) ;
	_channel_open = 0 ;
    }
}

string
DODSMySQLChannel::get_error()
{
    if( _channel_open )
	_error = mysql_error( _the_channel ) ;      
    else
	_error = mysql_error( &_mysql ) ;
    return _error ;
}

