// DODSMySQLConnect.h

#ifndef DODSMySQLConnect_h_
#define DODSMySQLConnect_h_ 1

#include <string>

using std::string ;

#include <mysql.h>

class DODSMySQLQuery ;

class DODSMySQLConnect
{
private:
    string		_server, _user, _database ;
    string		_error ;
    bool		_channel_open ;
    bool		_has_log ;
    MYSQL *		_the_channel ;
    MYSQL		_mysql ;
    int			_count ;

public:
    			DODSMySQLConnect() ;
			~DODSMySQLConnect() ;
    void		open(const string &server, const string &user,
			     const string &password, const string &database ) ;
    void		close ();
    int			is_channel_open() const { return _channel_open ; }
    string		get_error();
    MYSQL *		get_channel() { return _the_channel ; }
    string		get_server() { return _server ; }
    string		get_user() { return _user ; }
    string		get_database() { return _database ; }
};

#endif // DODSMySQLConnect_h_

