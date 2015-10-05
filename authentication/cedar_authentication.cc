#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

// C++ includes
#include <iostream>
#include <string>
//#include <vector>
#include <fstream>

using std::string ;
using std::endl ;
using std::cerr ;
//using std::vector ;
using std::ofstream ;
using std::ios ;
using std::auto_ptr ;
 
ofstream dbgstrm( "./cedar_authen.dbg", ios::trunc ) ;

// MySQL++ includes
#include <mysql++.h>
#include <result.h>

using namespace::mysqlpp ;

// IDL include.
#ifdef __cplusplus
extern "C" {
#include "export.h" 
char * ltoa(long val, char *buf,int base);
}
#endif

// Local includes
#include "CEDARException.h"

bool debug=false;
bool use_tcp=false;

// Timeout in seconds
#define CEDAR_USERS_TIMEOUT 20


extern "C" void toggle_debug(int argc, IDL_VPTR argv[]) 
{
    if(debug)
	dbgstrm<<"setting debug to false"<<endl;
    else
	dbgstrm<<"setting debug to true"<<endl;

    debug=!debug;
}

#define MYSQL_USER "cedardb"
#define MYSQL_PASSWORD "gr8!gaz00"
#define MYSQL_DATABASE "wikidb"
#define MYSQL_HOST "localhost"
#define MYSQL_HOME getenv("MYSQL_HOME")
#define MYSQL_UNIX_SOCKET "/tmp/mysql.sock"

//typedef vector<string> Policy;

unsigned long execute_query(const char *q, Result *res) 
{
    if( debug )
    {
	dbgstrm << "execute_query: " << q << endl ;
    }
    try
    {  
	static Connection con ;
	if( !con.connected() )
	{
	    if( use_tcp )
	    {
		if(debug)
		{
		    dbgstrm << "  opening connection type TCP/IP - 3306" << endl ;
		}
		con.connect( MYSQL_DATABASE, MYSQL_HOST, MYSQL_USER,
			     MYSQL_PASSWORD, 3306 ) ;
	    }
	    else
	    {
		string mysql_socket ;

		string mysql_home = MYSQL_HOME ;
		if( mysql_home != "" )
		{
		    mysql_socket = mysql_home + "/mysql.sock" ;
		}
		else
		{
		    mysql_socket = MYSQL_UNIX_SOCKET ;
		}
		if(debug)
		{
		    dbgstrm << "  opening connection via unix socket" << endl ;
		    dbgstrm << "  database = " << MYSQL_DATABASE << endl ;
		    dbgstrm << "  host = " << MYSQL_HOST << endl ;
		    dbgstrm << "  user = " << MYSQL_USER << endl ;
		    dbgstrm << "  password = " << MYSQL_PASSWORD << endl ;
		    dbgstrm << "  unix socket = " << mysql_socket << endl ;
		}
		con.connect( MYSQL_DATABASE, MYSQL_HOST, MYSQL_USER,
			     MYSQL_PASSWORD, 0, 0, 5, mysql_socket.c_str() ) ;
	    }
	}
	if( debug )
	{
	    dbgstrm << "  Server info: " << con.server_info() << endl ;
	    dbgstrm << "  Server stats: " << con.stat() << endl ;
	    dbgstrm << "  Server proto info: " << con.proto_info() << endl ;
	    dbgstrm << "  Client info: "<< con.client_info() << endl ;
	    dbgstrm << "  Host info: " << con.host_info() << endl ;
	}
	Query query = con.query() ;
	if( res )
	{
	    query << q ;
	    *res = query.store() ;
	}
	else
	{
	    query.exec(string(q));
	}

	if( debug )
	{
	    dbgstrm << "  " << __FILE__ << ":" << __LINE__
	            << ": query executed successfully" << endl ;
	    dbgstrm << con.info() ;
	}

	return 0;    
    }

    catch (BadConversion &er) // handle bad conversions 
    {
	dbgstrm << "error converting" << endl ;
	cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" 
	     << er.type_name << "\"." << endl ; 
	return 4ul ; 
    }
    catch( Exception &er ) 
    { 
	dbgstrm << "Error: " << er.what() << endl ;
	cerr << "Error: " << er.what() << endl ; 
	return 3ul ; 
    }
    catch(...)
    {
	dbgstrm << "Unknown exception..." << endl ;
	cerr << __FILE__ << ":" << __LINE__
	     << ": Unknown exception..." << endl ;
	return 5ul ;
    }
}

extern "C" IDL_VPTR cedar_authenticate_user(int argc, IDL_VPTR argv[]) 
{
    if( debug )
    {
	dbgstrm << "cedar_authenticate_user" << endl ;
    }
    // Allocate memory for return variable.
    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); 
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;

    try
    {
	if( argc!=2 )
	{
	    lReturn->value.ul = 6ul;
	    return lReturn ;
	}

	if( argv[0]->type != IDL_TYP_STRING )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Expected a string as argument 0\n" ;
	    lReturn->value.ul = 7ul ;
	    return lReturn ;
	}

	if( argv[1]->type != IDL_TYP_STRING )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Expected a string as argument 1\n" ;
	    lReturn->value.ul = 8ul ;
	    return lReturn ;
	}

	char *username = (char *)argv[0]->value.str.s ;
	if( !username )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Null pointer, where is the username???\n" ;
	    lReturn->value.ul = 7ul ;
	    return lReturn ;
	}
	username[0] = toupper( username[0] ) ;

	char *password = (char *)argv[1]->value.str.s ;
	if( !password )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Null pointer, where is the password???\n" ;
	    lReturn->value.ul = 8ul ;
	    return lReturn ;
	}

	string query = string( "SELECT user_id,user_password FROM user WHERE user_name = '" ) + username + "';" ;

	if( debug )
	{
	    dbgstrm << "  Executing this query: " << query << endl ;
	    dbgstrm << "  username = " << username << endl ;
	    dbgstrm << "  password = " << password << endl ;
	}

	// Object to hold the query results...
	Result *r = new Result ;

	// Attach the pointer so we destroy no matter how we leave this function
	auto_ptr<Result> ptr( r ) ;
	//CEDARAutoPtr<Result> ptr(r);

	lReturn->value.ul = execute_query( query.c_str(), r ) ;

	if( lReturn->value.ul != 0 )
	{
	    return lReturn;
	}

	if  (r->size()==0)
	{
	    if( debug )
	    {
		dbgstrm << "**user " << username << " does not exist" << endl ;
	    }
	    lReturn->value.ul = 9ul ;
	    return lReturn ;
	}

	if( r->size() != 1 )
	{
	    dbgstrm << "**number of rows returned not 1" << endl ;
	    cerr << __FILE__ << ":" << __LINE__
	         << ": The number of rows returned from the database is not 1"
		 << endl ;
	    lReturn->value.ul = 10ul ;
	    return lReturn ;
	}

	Result::iterator i ;
	i = r->begin() ;
	Row row ;
	row = *i ;

	string user_id ;
	const char *user_id_c = (const char *)row.at(0) ;
	if( user_id_c )
	    user_id = user_id_c ;

	if( user_id.empty() )
	{
	    dbgstrm << "**database user_id is empty" << endl ;
	    cerr << __FILE__ << ":" << __LINE__
	         << ": The user_id in the database is empty"
		 << endl ;
	    lReturn->value.ul = 14ul ;
	    return lReturn ;
	}

	string db_password ;
	const char *db_password_c = (const char *)row.at(1) ;
	if( db_password_c )
	    db_password = db_password_c ;

	if( db_password.empty() )
	{
	    dbgstrm << "**database password is empty" << endl ;
	    cerr << __FILE__ << ":" << __LINE__
	         << ": The password in the database is empty"
		 << endl ;
	    lReturn->value.ul = 15ul ;
	    return lReturn ;
	}

	if( debug )
	{
	    dbgstrm << "  Got id from database: " << user_id << endl ;
	    dbgstrm << "  Got password from database: " << db_password << endl ;
	}
	char salt[14] ;
	for( int salt_i = 0; salt_i < 14; salt_i++ )
	{
	    salt[salt_i] = db_password.c_str()[salt_i] ;
	}
	salt[13] = '\0' ;

	string encrypted_password( crypt( password, salt ) ) ;
	if( debug )
	{
	    dbgstrm << "  Generated password from incoming data: "
	            << encrypted_password << endl ;
	}

	if( db_password != encrypted_password )
	{
	    if( debug )
	    {
		dbgstrm << "**Password do not match" << endl ;
	    }
	    lReturn->value.ul = 11ul ;
	    return lReturn ;
	}
	else
	{
	    if( debug )
	    {
		dbgstrm << "  Password match" << endl ;
	    }
	}

	query = "SELECT * FROM user_groups WHERE ug_user = " + user_id
	        + " AND ug_group = 'Cedar';" ;

	Result *r2 = new Result ;
	ptr.reset( r2 ) ;

	lReturn->value.ul = execute_query( query.c_str(), r2 ) ;

	if( lReturn->value.ul != 0 )
	{
	    return lReturn;
	}

	if  (r2->size()==0)
	{
	    if( debug )
	    {
		dbgstrm << "**user " << username
		        << " does not have permission" << endl ;
	    }
	    lReturn->value.ul = 12ul ;
	    return lReturn ;
	}

	lReturn->value.ul = 0ul ;
	return lReturn ;
    }
    catch(...)
    {
	if( debug )
	{
	    dbgstrm << "**Unknown exception..." << endl ;
	}
	cerr << __FILE__ << ":" << __LINE__
	     << ": Unknown exception..." << endl ;
	lReturn->value.ul = 5ul ;
	return lReturn ;
    }
}

extern "C" IDL_VPTR cedar_logoff_user(int argc, IDL_VPTR argv[]) 
{
    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); // Allocate memory for return variable.
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;

    try
    {
	if( argc != 2 )
	{
	    lReturn->value.ul = 6ul ;
	    return lReturn ;
	}

	if( argv[0]->type != IDL_TYP_STRING )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Expected a string as argument 0\n" ;
	    lReturn->value.ul = 7ul ;
	    return lReturn ;
	}

	char *username = (char *)argv[0]->value.str.s ;
	if( !username )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Null pointer, where is the username???\n" ;
	    lReturn->value.ul = 8ul ;
	    return lReturn ;
	}
	username[0] = toupper( username[0] ) ;

	if( argv[1]->type != IDL_TYP_STRING )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Expected a string as argument 1\n" ;
	    lReturn->value.ul = 7ul ;
	    return lReturn ;
	}

	char *ipaddress = (char *)argv[1]->value.str.s ;
	if( !ipaddress )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Null pointer, where is the ipaddress???\n" ;
	    lReturn->value.ul = 8ul ;
	    return lReturn ;
	}

	// make sure the given user exist in the database
	string query =string( "select USER_NAME from user " ) ;
	query += string( "where USER_NAME=\"" ) + username + "\";" ;

	if( debug )
	{
	    dbgstrm << "Executing this query: " << query << endl ;
	}

	// Object to hold the query results...
	Result *r = new Result ;
	if( debug )
	{
	    dbgstrm<<"Got result object at address: "<<r<<endl;
	}

	// Attach the pointer so we destroy no matter how we leave this function
	auto_ptr<Result> ptr(r);
	//CEDARAutoPtr<Result> ptr(r);

	lReturn->value.ul = execute_query( query.c_str(), r ) ;
	if( lReturn->value.ul != 0 )
	{
	    return lReturn ;
	}

	if( r->size() == 0 )
	{
	    if( debug )
	    {
		dbgstrm << "The user " << username
		     << " does not exist in the database" << endl ;
	    }
	    lReturn->value.ul = 9ul;
	    return lReturn ;
	}

	query = string( "delete from cedar_sessions " ) ;
	query += string( "where USER_NAME=\"" ) + username + "\"" ;
	query += string( " AND CLIENT_IP=\"") + ipaddress + "\";" ;
	if( debug )
	{
	    dbgstrm << "Executing this query: " << query << endl ;
	}
	lReturn->value.ul = execute_query( query.c_str(), 0 ) ;

	return lReturn ;
    }
    catch(...)
    {
	cerr << __FILE__ << ":" << __LINE__
	     << ": Unknown exception..." << endl ;
	lReturn->value.ul = 5ul ;
	return lReturn ;
    }
}

extern "C" IDL_VPTR cedar_create_session(int argc, IDL_VPTR argv[]) 
{
    if( debug )
    {
	dbgstrm << "cedar_create_session" << endl ;
    }
    // A user may be logged in to the system at different machines. If the
    // user is logged in at this machine then delete the current session and
    // add a new session. If the user is not logged in at this machine then
    // simply add a new session for the user at this machine

    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); // Allocate memory for return variable.
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;

    try
    {
	if(argc!=2)
	{
	    lReturn->value.ul = 6ul;
	    return lReturn;
	}


	if(argv[0]->type != IDL_TYP_STRING)
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Expected a string as argument 0\n" ; 
	    lReturn->value.ul = 7ul;
	    return lReturn;
	}

	if(argv[1]->type != IDL_TYP_STRING)
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Expected a string as argument 1\n" ; 
	    lReturn->value.ul = 7ul;
	    return lReturn;
	}

	char *username = (char *)argv[0]->value.str.s ;
	if( !username )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Null pointer, where is the username???\n" ;
	    lReturn->value.ul = 8ul;
	    return lReturn;
	}
	if( debug )
	{
	    dbgstrm << "  creating session for user " << username << endl ;
	}

	char *ipaddress = (char *)argv[1]->value.str.s ;
	if( !ipaddress )
	{
	    cerr << __FILE__ << ":" << __LINE__
	         << " Null pointer, where is the ip address???\n" ; 
	    lReturn->value.ul = 8ul;
	    return lReturn;
	}
	if( debug )
	{
	    dbgstrm << "  creating session for ip " << ipaddress << endl ;
	}

	string query = string( "select CLIENT_IP from cedar_sessions " ) ;
	query += string( "where USER_NAME=\"" ) + username + "\";" ;
	if( debug )
	{
	    dbgstrm << "  Executing this query: " << query << endl ;
	}

	// Object to hold the query results...
	Result *r1 = new Result ;
	if( debug )
	{
	    dbgstrm << "  Got result object at address: " << r1 << endl ;
	}

	// Attach the pointer so we destroy no matter how we leave function
	auto_ptr<Result> ptr1( r1 ) ;
	//CEDARAutoPtr<Result> ptr1( r1 ) ;

	unsigned long k = execute_query( query.c_str(), r1 ) ;
	if(k!=0)
	{
	    lReturn->value.ul = k ;
	    return lReturn ;
	}

	if( r1->size() != 0 )
	{
	    if( debug )
	    {
		dbgstrm << "  user exists, delete from database" << endl ;
	    }
	    // delete the user at this ip address. it will be added back
	    Result::iterator i ;
	    i = r1->begin() ;
	    for( i; i != r1->end(); i++ )
	    {
		Row row ;
		row = *i ;
		if( string( ipaddress ) == string( (const char *)row.at(0) ) )
		{
		    query = string( "delete from cedar_sessions " ) ;
		    query += string( "where USER_NAME=\"" ) + username + "\"" ;
		    query += string( " AND CLIENT_IP=\"" ) + ipaddress + "\";" ;
		    if(debug)
		    {
			dbgstrm << "  Executing this query: " << query << endl ;
		    }
		    k = execute_query( query.c_str(), 0 ) ;
		}
	    }
	}

	if( debug )
	{
	    dbgstrm << "  insert user into cedar_sessions" << endl ;
	}
	// add the user at this ip address
	query = string( "insert into cedar_sessions " ) ;
	query += " set CLIENT_IP=\"" + string( ipaddress ) + "\"" ;
	query += ", USER_NAME=\"" + string( username ) + "\"" ;
	query += ", LAST_HEART_BEAT=now();" ;

	if(debug)
	{
	    dbgstrm << "  Executing this query: " << query << endl ;
	}
	lReturn->value.ul = execute_query( query.c_str(), 0 ) ;

	return lReturn;
    }
    catch (BadQuery &er) 
    {
	if( debug )
	{
	    dbgstrm << "Error: " << er.what() << endl ;
	}
	cerr << "Error: " << er.what() << endl ; 
	lReturn->value.ul = 3ul ;
	return lReturn ; 
    }
    catch (BadConversion &er) // handle bad conversions 
    {
	if( debug )
	{
	    dbgstrm << "Conversion error" << endl ;
	}
	cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" 
	     << er.type_name << "\"." << endl ; 
	lReturn->value.ul = 4ul ;
	return lReturn ;
    }
    catch (CEDARException &e)
    {
	if( debug )
	{
	    dbgstrm << "Cedar Exception: " << e.get_error_description() << endl;
	}
	cerr << e.get_error_description() << endl ;
	lReturn->value.ul = 5ul ;
	return lReturn ;
    }
    catch(...)
    {
	if( debug )
	{
	    dbgstrm << "Unknown Exception" << endl;
	}
	cerr << __FILE__ << ":" << __LINE__ << ": Unknown exception..." <<endl ;
	lReturn->value.ul = 5ul ;
	return lReturn ;
    }
}

#if 0
unsigned long check_if_user_is_login(const string &username, const string &ipaddress, short int &seconds)
{
    // get sure the given user exist in the database
    string query=string("select USER_REAL_NAME from tbl_users where USER_NAME=\"") + username + ("\";");
    
    if(debug)
	dbgstrm<<"Executing this query: "<<query<<endl;
    
    // Object to hold the query results...
    Result *r=new Result;
    if(debug)
	dbgstrm<<"Got result object at address: "<<r<<endl;
    
    // Attach the pointer so we destroy no matter how we leave this function
    auto_ptr<Result> ptr(r);
    //CEDARAutoPtr<Result> ptr(r);
    
    unsigned long k=execute_query(query.c_str(),r);
    
    
    if(k!=0)
	return k;
    
    if  (r->size()==0)
	{
	    if(debug)
		dbgstrm<<"The user "<<username<<" does not exist in the database"<<endl;
	    return 9ul;
	}
    
    query=string("select now(), LAST_HEART_BEAT from cedar_sessions where USER_NAME=\"")+ username + string("\" and CLIENT_IP=\"") + ipaddress + string("\";");
    if(debug)
	dbgstrm<<"Executing this query: "<<query<<endl;
    // Object to hold the query results...
    Result *r1=new Result;
    if(debug)
	dbgstrm<<"Got result object at address: "<<r1<<endl;
    
    // Attach the pointer so we destroy no matter how we leave this function
    auto_ptr<Result> ptr1(r1);
    //CEDARAutoPtr<Result> ptr1(r1);
    
    k=execute_query(query.c_str(),r1);
    
    if(k!=0)
	return k;
    
    if  (r1->size()==0)
      return 12ul;

     if  (r1->size()>1)
	{
	    cerr<<__FILE__<<":"<<__LINE__<<": The number of rows returned from the database bigger than 1"<<endl;
	    return 10ul;
	}
    Result::iterator i;
    
    i = r1->begin();
    Row row; 
    row = *i;
    
    string user_last_login, now;
    now = (const char *)row.at(0);
    user_last_login = (const char *)row.at(1);
    
    if(debug)
	dbgstrm<<"User last login was: "<<user_last_login<<" and now is: "<<now<<endl;
    DateTime lt(user_last_login.c_str());
    DateTime ct(now.c_str());
    
    if ((ct.year-lt.year)>0)
	{
	    if(debug)
		dbgstrm<<"The user logged more than a year ago"<<endl;
	    return 13ul;
	}
    
    if ((ct.month-lt.month)>0)
	{
	    if(debug)
		dbgstrm<<"The user logged more than a month ago"<<endl;
	    return 13ul;
	}
    
    if ((ct.day-lt.day)>0)
	{
	    if(debug)
		dbgstrm<<"The user logged more than a day ago"<<endl;
	    return 13ul;
	}
    
    short int hours= (short int)(ct.hour-lt.hour);
    if (hours>1)
	{
	    if(debug)
		dbgstrm<<"The user logged more than an hour ago"<<endl;
	    return 13ul;
	}
    
    unsigned long minutes=((short int)ct.minute) - ((short int)lt.minute);
    
    if(minutes>1)
      {
	if(debug)
	  dbgstrm<<"The user logged more than a minute ago"<<endl;
	return 13ul;
	}

    if(minutes==1)
      seconds=60 + ((short int)ct.second) - ((short int)lt.second);
    else
      seconds=((short int)ct.second) - ((short int)lt.second);    
    if(seconds>CEDAR_USERS_TIMEOUT)
	{
	    if(debug)
		dbgstrm<<"The user logged too long ago, its current diff is: "<<seconds<<" seconds"<<endl;
	    return 13ul;
	}
    
    if(debug)
	dbgstrm<<"The user still OK, its current diff is: "<<seconds<<" seconds"<<endl;
    
    return 0ul;
}

extern "C" IDL_VPTR cedar_is_login(int argc, IDL_VPTR argv[]) 
{
  
  IDL_VPTR lReturn;
  lReturn = IDL_Gettmp(); // Allocate memory for return variable.
  lReturn->type = IDL_TYP_ULONG;
  lReturn->value.ul = 0ul;
  
  try
      {
	  if(argc!=2)
	      {
		  lReturn->value.ul = 6ul;
		  return lReturn;
	      }
	  
	  
	  if(argv[0]->type != IDL_TYP_STRING)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		  lReturn->value.ul = 7ul;
		  return lReturn;
	      }
	  
	  char *username=(char *) argv[0]->value.str.s;
	  if(!username)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the username???\n"; 
		  lReturn->value.ul = 8ul;
		  return lReturn;
	      }
	      username[0] = toupper( username[0] ) ;
	   if(argv[1]->type != IDL_TYP_STRING)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		  lReturn->value.ul = 7ul;
		  return lReturn;
	      }
	  
	  char *ipaddress=(char *) argv[1]->value.str.s;
	  if(!ipaddress)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the ipaddress???\n"; 
		  lReturn->value.ul = 8ul;
		  return lReturn;
	      }
	  short int secs;
	  lReturn->value.ul =check_if_user_is_login(username, ipaddress, secs);
	  return lReturn;
      }
  catch(...)
      {
	  cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	  lReturn->value.ul=5ul;
	  return lReturn;
      } 
  
}

void list_sessions()
{
    string query = string( "select USER_NAME, CLIENT_IP from cedar_sessions;" ) ;
    Result *r1 = new Result ;
    Result::iterator i ;
    i = r1->begin() ;
    for( i; i != r1->end(); i++ )
    {
	Row row ;
	row = *i ;
	dbgstrm << "User " << string( (const char *)row.at(0) )
	     << "logged in at addresse:" << string( (const char *)row.at(1) )
	     << endl ;
    }
}

extern "C" IDL_VPTR cedar_who_is_login(int argc, IDL_VPTR argv[]) 
{
    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); // Allocate memory for return variable.
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;
    
    try
	{
	    if(argc!=2)
		{
		    lReturn->value.ul = 6ul;
		    return lReturn;
		}
	    
	    
	    if(argv[0]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	    if(argv[1]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 1\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	    char *ipaddress=(char *) argv[0]->value.str.s;
	    if(!ipaddress)
	      {
		cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the ipaddress???\n"; 
		lReturn->value.ul = 8ul;
		return lReturn;
	      }
	    if(!((char *) argv[1]->value.str.s))
	      {
		cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the space for the name???\n"; 
		lReturn->value.ul = 8ul;
		return lReturn;
	      }
	    string query=string("select  USER_NAME from cedar_sessions where CLIENT_IP=\"") + ipaddress + string("\";");
	    if(debug)
	      dbgstrm<<"Executing this query: "<<query<<endl;
	    // Object to hold the query results...
	    Result *r1=new Result;
	    if(debug)
	      dbgstrm<<"Got result object at address: "<<r1<<endl;
    
	    // Attach the pointer so we destroy no matter how we leave function
	    auto_ptr<Result> ptr1(r1);
	    //CEDARAutoPtr<Result> ptr1(r1);
	    
	    unsigned long k=execute_query(query.c_str(),r1);
    
	    if(k!=0)
	      {
		lReturn->value.ul=k;
		return lReturn;
	      }
	    if (r1->size()!=0)
	      {
		if (r1->size()==1) 
		  {
		    Result::iterator i;
		    i = r1->begin();
		    Row row; 
		    row = *i;
		    string name((const char *)row.at(0));
		    int the_length=name.length();
		    memcpy( ((char *) argv[1]->value.str.s), name.c_str(), the_length);
		    ((char *) argv[1]->value.str.s)[the_length]='\0';
		    argv[1]->value.str.slen=the_length+1;
		    
		  }
		else
		  {
		    cerr<<__FILE__<<":"<<__LINE__<<": The number of rows returned from the database is bigger than 1"<<endl;
		    lReturn->value.ul = 10ul;
		    return lReturn;
		  }
	      }
	    else
	      {
		 string name("nobody");
		 int the_length=name.length();
		 memcpy( ((char *) argv[1]->value.str.s), name.c_str(), the_length);
		 ((char *) argv[1]->value.str.s)[the_length]='\0';
		 argv[1]->value.str.slen=the_length+1;
	      }

	    return lReturn;
	}

    catch (BadQuery &er) 
	{ 
	    cerr << "Error: " << er.what() << endl; 
	    lReturn->value.ul=3ul;
	    return lReturn; 
	}
    catch (BadConversion &er) // handle bad conversions 
	{ 
	    cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" 
		 << er.type_name << "\"." << endl; 
	    lReturn->value.ul=4ul;
	    return lReturn;
	} 
    catch (CEDARException &e)
	{
	    cerr <<e.get_error_description()<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
    catch(...)
	{
	    cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
}

extern "C" IDL_VPTR cedar_create_user(int argc, IDL_VPTR argv[]) 
{
  
  IDL_VPTR lReturn;
  lReturn = IDL_Gettmp(); // Allocate memory for return variable.
  lReturn->type = IDL_TYP_ULONG;
  lReturn->value.ul = 0ul;
  
  try
      {
	  if(argc!=2)
	      {
		  lReturn->value.ul = 6ul;
		  return lReturn;
	      }
	  
	  
	  if(argv[0]->type != IDL_TYP_STRING)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		  lReturn->value.ul = 7ul;
		  return lReturn;
	      }
	  if(argv[1]->type != IDL_TYP_STRING)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 1\n"; 
		  lReturn->value.ul = 7ul;
		  return lReturn;
	      }
  
	  char *username=(char *) argv[0]->value.str.s;
	  if(!username)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the username???\n"; 
		  lReturn->value.ul = 8ul;
		  return lReturn;
	      }
	  
	  char *password=(char *) argv[1]->value.str.s;
	  if(!password)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the password???\n"; 
		  lReturn->value.ul = 8ul;
		  return lReturn;
	      }
  
	  string query=string("insert into tbl_users set USER_NAME=\"")+ username +string("\" , USER_PASSWORD=ENCRYPT(\"")+password+"\");";
	  if(debug)
	      dbgstrm<<"Executing this query: "<<query<<endl;
	  lReturn->value.ul=execute_query(query.c_str(),0);
	  
	  return lReturn;
	  
      }
  catch(...)
      {
	  cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	  lReturn->value.ul=5ul;
	  return lReturn;
      } 
}

extern "C" IDL_VPTR cedar_delete_user(int argc, IDL_VPTR argv[]) 
{
  
  IDL_VPTR lReturn;
  lReturn = IDL_Gettmp(); // Allocate memory for return variable.
  lReturn->type = IDL_TYP_ULONG;
  lReturn->value.ul = 0ul;

  try
      {
	  
	  if(argc!=1)
	      {
		  lReturn->value.ul = 6ul;
		  return lReturn;
	      }
	  
	  
	  if(argv[0]->type != IDL_TYP_STRING)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		  lReturn->value.ul = 7ul;
		  return lReturn;
	      }
	  
	  
	  char *username=(char *) argv[0]->value.str.s;
	  if(!username)
	      {
		  cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the username???\n"; 
		  lReturn->value.ul = 8ul;
		  return lReturn;
	      }

	  // get sure the given user exist in the database
	  string query=string("select USER_PASSWORD from tbl_users where USER_NAME=\"") + username + ("\";");
	  
	  if(debug)
	      dbgstrm<<"Executing this query: "<<query<<endl;
	  
	  // Object to hold the query results...
	  Result *r=new Result;
	  if(debug)
	      dbgstrm<<"Got result object at address: "<<r<<endl;
	  
	  // Attach the pointer so we destroy no matter how we leave function
	  auto_ptr<Result> ptr(r);
	  //CEDARAutoPtr<Result> ptr(r);
	  
	  lReturn->value.ul=execute_query(query.c_str(),r);
	  
	  
	  if(lReturn->value.ul!=0)
	      return lReturn;
	  
	  if  (r->size()==0)
	      {
		  if(debug)
		      dbgstrm<<"The user "<<username<<" does not exist in the database"<<endl;
		  lReturn->value.ul = 9ul;
		  return lReturn;
	      }
	  
	  
	  query=string("delete from tbl_users where USER_NAME=\"")+ username +string("\";");
	  if(debug)
	      dbgstrm<<"Executing this query: "<<query<<endl;
	  lReturn->value.ul=execute_query(query.c_str(),0);
	  
	  return lReturn;
	  
      }
  catch(...)
      {
	  cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	  lReturn->value.ul=5ul;
	  return lReturn;
      } 
}

inline void parse_policy (const string &privileges, Policy &p)
{
    string temp=privileges;
    int pos=temp.find(",");
    while (pos>0)
	{
	    string k= string(temp,0,pos);
	    p.push_back(k);
	    temp=string(temp,pos+1,temp.size());
	    pos=temp.find(",");
	}
    p.push_back(temp);
    if (debug)
	{
	    for (int h=0; h<p.size(); h++)
		dbgstrm<<p[h]<<endl;
	}
    
    
}

unsigned long load_policy(const char* username, Policy &p) throw(CEDARException)
{
    try
	{
	    unsigned long rvalue=0;
	    if (!username)
		throw CEDARException("Null pointer");
	    
	    // get sure the given user exist in the database
	    string query=string("select USER_GROUP from tbl_users where USER_NAME=\"") + username + ("\";");
	    
	    if(debug)
		dbgstrm<<"Executing this query: "<<query<<endl;
	    
	    // Object to hold the query results...
	    Result *r=new Result;
	    if(debug)
		dbgstrm<<"Got result object at address: "<<r<<endl;
	    
	    // Attach the pointer so we destroy no matter how we leave function
	    auto_ptr<Result> ptr(r);
	    //CEDARAutoPtr<Result> ptr(r);
	    
	    rvalue=execute_query(query.c_str(),r);
	    if(rvalue!=0)
		return rvalue;
	    
	    if  (r->size()==0)
		{
		    if(debug)
			dbgstrm<<"The user "<<username<<" does not exist in the database"<<endl;
		    rvalue = 9ul;
		    return rvalue;
		}
	    
	    Result::iterator i;
	    
	    i = r->begin();
	    Row row; 
	    row = *i;
	    
	    string group;
	    group = (const char *)row.at(0);
	    
	    query=string("select LEVEL from tbl_groups where  GROUP_NAME=\"") + group + ("\";");
	    
	    if(debug)
		dbgstrm<<"Executing this query: "<<query<<endl;
	    
	    // Object to hold the query results...
	    Result *r1=new Result;
	    if(debug)
		dbgstrm<<"Got result object at address: "<<r<<endl;
	    
	    // Attach the pointer so we destroy no matter how we leave function
	    auto_ptr<Result> ptr1(r1);
	    //CEDARAutoPtr<Result> ptr1(r1);
	    
	    rvalue=execute_query(query.c_str(),r1);
	    if(rvalue!=0)
		return rvalue;
	    
	    if  (r1->size()==0)
		{
		    if(debug)
			dbgstrm<<"The group "<<group<<" does not exist in the database"<<endl;
		    throw CEDARException("Data out of balance, please contact jgarcia@ucar.edu");
		}
	    
	    i = r1->begin();
	    row = *i;
	    
	    string level;
	    level = (const char *)row.at(0);
	    
	    if(debug)
		dbgstrm<<"The user's group is: "<<group<<" and its level is "<<level<<endl;
	    
	    int lev=atoi(level.c_str());
	    string privileges="";
	    for (int j=0; j<=lev; j++)
		{
		    char t[10];
		    ltoa(j,t,10);
		    query="select GROUP_PRIVILEGES from tbl_groups where level=\"" + string(t) + ("\";");
		    if(debug)
			dbgstrm<<"Executing this query: "<<query<<endl;
		    
		    // Object to hold the query results...
		    Result *r2=new Result;
		    if(debug)
			dbgstrm<<"Got result object at address: "<<r<<endl;
		    
		    // Attach the pointer so we destroy no matter how we leave
		    // this function
		    auto_ptr<Result> ptr2(r2);
		    //CEDARAutoPtr<Result> ptr2(r2);
		    
		    rvalue=execute_query(query.c_str(),r2);
		    if(rvalue!=0)
			return rvalue;
		    
		    if  (r2->size()==0)
			throw CEDARException("Data out of balance, refers levels, please contact jgarcia@ucar.edu");
		    
		    i = r2->begin();
		    row = *i;
		    
		    string privi;
		    privi = (const char *)row.at(0);
		    privileges+=privi;
		    if(j<lev)
			privileges+=",";
		}
	    
	    parse_policy(privileges,p);

	    return rvalue;
	}
    catch(...)
	{
	    throw CEDARException("Undefined body exception.");
	}
    
    
    
}

extern "C" IDL_VPTR cedar_is_user_authorized(int argc, IDL_VPTR argv[]) 
{
    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); // Allocate memory for return variable.
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;

    try
	{
	  
	    if(argc!=2)
		{
		    lReturn->value.ul = 6ul;
		    return lReturn;
		}
	  
	  
	    if(argv[0]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	  
	  
	    char *username=(char *) argv[0]->value.str.s;
	    if(!username)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the username???\n"; 
		    lReturn->value.ul = 8ul;
		    return lReturn;
		}

	    if(argv[1]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	  
	  
	    char *privilege=(char *) argv[1]->value.str.s;
	    if(!privilege)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the privilege???\n"; 
		    lReturn->value.ul = 8ul;
		    return lReturn;
		}

	    Policy policy;
	    lReturn->value.ul=load_policy(username, policy);
	    if(lReturn->value.ul!=0)
		return lReturn;
	    for (int h=0; h<policy.size(); h++)
		{
		    if (privilege==policy[h])
			return lReturn;
		}
	  
	    if(debug)
		dbgstrm<<"The user "<<username<<" is not authorized for "<<privilege<<endl;
	    lReturn->value.ul=13;
	    return lReturn;
	}
	  
    catch (BadQuery &er) 
	{ 
	    cerr << "Error: " << er.what() << endl; 
	    lReturn->value.ul=3ul;
	    return lReturn; 
	}
    catch (BadConversion &er) // handle bad conversions 
	{ 
	    cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" 
		 << er.type_name << "\"." << endl; 
	    lReturn->value.ul=4ul;
	    return lReturn;
	} 
    catch (CEDARException &e)
	{
	    cerr <<e.get_error_description()<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
    catch(...)
	{
	    cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
}

extern "C" IDL_VPTR cedar_change_password(int argc, IDL_VPTR argv[]) 
{
    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); // Allocate memory for return variable.
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;

    try
	{
	 if(argc!=3)
		{
		    lReturn->value.ul = 6ul;
		    return lReturn;
		}
	  
	  
	    if(argv[0]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	  
	  
	    char *username=(char *) argv[0]->value.str.s;
	    if(!username)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the username???\n"; 
		    lReturn->value.ul = 8ul;
		    return lReturn;
		}

	    if(argv[1]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	  
	  
	    char *oldpassword=(char *) argv[1]->value.str.s;
	    if(!oldpassword)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the old password???\n"; 
		    lReturn->value.ul = 8ul;
		    return lReturn;
		}
	    
	    if(argv[2]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	  
	  
	    char *newpassword=(char *) argv[2]->value.str.s;
	    if(!newpassword)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the new password???\n"; 
		    lReturn->value.ul = 8ul;
		    return lReturn;
		}
	    
	    // get sure the given user exist in the database
	    string query=string("select USER_PASSWORD from tbl_users where USER_NAME=\"") + username + ("\";");
	    
	    if(debug)
		dbgstrm<<"Executing this query: "<<query<<endl;
	    
	    // Object to hold the query results...
	    Result *r=new Result;
	    if(debug)
		dbgstrm<<"Got result object at address: "<<r<<endl;
	    
	    // Attach the pointer so we destroy no matter how we leave function
	    auto_ptr<Result> ptr(r);
	    //CEDARAutoPtr<Result> ptr(r);
	    
	    lReturn->value.ul=execute_query(query.c_str(),r);
	    
	    
	    if(lReturn->value.ul!=0)
		return lReturn;
	    
	    if  (r->size()==0)
		{
		    if(debug)
			dbgstrm<<"The user "<<username<<" does not exist in the database"<<endl;
		    lReturn->value.ul = 9ul;
		    return lReturn;
		}

	     
	    if  (r->size()!=1)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<": The number of rows returned from the database is not 1"<<endl;
		    lReturn->value.ul = 10ul;
		    return lReturn;
		}
	    
	    
	    Result::iterator i;
	    
	    i = r->begin();
	    Row row; 
	    row = *i;
	    
	    string db_password;
	    db_password = (const char *)row.at(0);
	    
	    if(debug)
		dbgstrm<<"Got password from database: "<<db_password<<endl;
	    char salt[3];
	    salt[0]=db_password.c_str()[0];
	    salt[1]=db_password.c_str()[1];
	    salt[2]='\0';
	    
	    string encrypted_password(crypt(oldpassword,salt));
	    if(debug)
		dbgstrm<<"Generated password from incoming data: "<<encrypted_password<<endl;
	    
	    if( db_password!=encrypted_password )
		{
		    if (debug)
			dbgstrm<<"Password do not match"<<endl;
		    lReturn->value.ul = 11ul;
		    return lReturn;
		}
	    else
		{
		    if (debug)
			dbgstrm<<"Password match"<<endl;
		}

	    query=string("update tbl_users set USER_PASSWORD=ENCRYPT(\"")+newpassword+string("\") where USER_NAME=\"")+ username +string("\";");
	    if(debug)
		dbgstrm<<"Executing this query: "<<query<<endl;
	    lReturn->value.ul=execute_query(query.c_str(),0);
	    
	    return lReturn;
	    
	}
    catch (BadQuery &er) 
	{ 
	    cerr << "Error: " << er.what() << endl; 
	    lReturn->value.ul=3ul;
	    return lReturn; 
	}
    catch (BadConversion &er) // handle bad conversions 
	{ 
	    cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" 
		 << er.type_name << "\"." << endl; 
	    lReturn->value.ul=4ul;
	    return lReturn;
	} 
    catch (CEDARException &e)
	{
	    cerr <<e.get_error_description()<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
    catch(...)
	{
	    cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
}

extern "C" IDL_VPTR cedar_refresh_login(int argc, IDL_VPTR argv[]) 
{
    IDL_VPTR lReturn;
    lReturn = IDL_Gettmp(); // Allocate memory for return variable.
    lReturn->type = IDL_TYP_ULONG;
    lReturn->value.ul = 0ul;
    
    try
	{
	    if(argc!=2)
		{
		    lReturn->value.ul = 6ul;
		    return lReturn;
		}
	    
	    
	    if(argv[0]->type != IDL_TYP_STRING)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		    lReturn->value.ul = 7ul;
		    return lReturn;
		}
	    
	    char *username=(char *) argv[0]->value.str.s;
	    if(!username)
		{
		    cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the username???\n"; 
		    lReturn->value.ul = 8ul;
		    return lReturn;
		}
	    
	    if(argv[1]->type != IDL_TYP_STRING)
	      {
		cerr<<__FILE__<<":"<<__LINE__<<" Expected a string as argument 0\n"; 
		lReturn->value.ul = 7ul;
		return lReturn;
	      }
	    
	    char *ipaddress=(char *) argv[1]->value.str.s;
	    if(!ipaddress)
	      {
		cerr<<__FILE__<<":"<<__LINE__<<" Null pointer, where is the ipaddress???\n"; 
		lReturn->value.ul = 8ul;
		return lReturn;
	      }
	    
	    short int secs;
	    if( (check_if_user_is_login(username, ipaddress, secs)!=0) && (check_if_user_is_login(username, ipaddress, secs)!=13))
	      {
		lReturn->value.ul = 14ul;
		return lReturn;
	      }
	    
	    string query=string("update cedar_sessions set LAST_HEART_BEAT=now() where USER_NAME=\"")+ username + string("\" and CLIENT_IP=\"") + string(ipaddress) + string("\";");
	    if(debug)
		dbgstrm<<"Executing this query: "<<query<<endl;
	    lReturn->value.ul=execute_query(query.c_str(),0);
	    
	    return lReturn;
	}

    catch (BadQuery &er) 
	{ 
	    cerr << "Error: " << er.what() << endl; 
	    lReturn->value.ul=3ul;
	    return lReturn; 
	}
    catch (BadConversion &er) // handle bad conversions 
	{ 
	    cerr << "Error: Tried to convert \"" << er.data << "\" to a \"" 
		 << er.type_name << "\"." << endl; 
	    lReturn->value.ul=4ul;
	    return lReturn;
	} 
    catch (CEDARException &e)
	{
	    cerr <<e.get_error_description()<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
    catch(...)
	{
	    cerr<<__FILE__<<":"<<__LINE__<<": Unknown exception..."<<endl;
	    lReturn->value.ul=5ul;
	    return lReturn;
	}
}
#endif

