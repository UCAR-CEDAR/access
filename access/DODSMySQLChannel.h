// DODSMySQLChannel.h

#ifndef DODSMySQLChannel_h_ 
#define DODSMySQLChannel_h_ 1

#include <string>
#include <mysql.h>

using std::string ;


class DODSMySQLChannel
{
private:
    string		_server, _user, _database ;
    string		_error;
    bool		_channel_open;
    bool		_has_log;
    MYSQL *		_the_channel;
    MYSQL		_mysql;

public:
    			DODSMySQLChannel();
    			~DODSMySQLChannel();

    bool		open( const char* server, const char* user,
			      const char* password, const char* database ) ;
    void		close();
    int			is_channel_open() const { return _channel_open ; }
    string		get_error();
    MYSQL *		get_channel() { return _the_channel ; }
};

#endif // DODSMySQLChannel_h_

