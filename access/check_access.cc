// DODSMySQLAuthenticate.cc

#include <time.h>
#include <iostream>
#include <string>
#include <fstream>

using std::cerr ;
using std::cout ;
using std::endl ;
using std::string ;
using std::fstream ;
using std::ios ;

#include "DODSMySQLQuery.h"
#include "DODSException.h"

#define AUTH_USERS_TIMEOUT 15

//static fstream cedar_access_log( "check_access_log.out", ios::out ) ;

int
main( int argC, char **argV )
{
    if( argC != 2 )
    {
	cerr << "Usage: " << argV[0] << " <ip address>" << endl ;
	return 1 ;
    }

    string user_name = argV[1] ;
    //cedar_access_log << "user_name passed in = \"" << user_name << "\"" << endl ;

    DODSMySQLQuery *query = 0 ;
    try
    {
	//cedar_access_log << "building query object" << endl ;
	query = new DODSMySQLQuery( "localhost", "cedardb", "11011010011110110100011100010001001011000100111000100100001111001010110110100011010111000100100111100010101011101100010111011101", "wikidb" ) ;
	//cedar_access_log << "done building query object" << endl ;
    }
    catch( DODSException &e )
    {
	//cedar_access_log << "Unable to check access for " << user_name << endl ;
	//cedar_access_log << e.get_error_description() << endl ;
	cerr << "Unable to check access for " << user_name << endl ;
	cerr << e.get_error_description() << endl ;
	return 1 ;
    }
    catch( ... )
    {
	//cedar_access_log << "Unable to check access to " << user_name << endl ;
	//cedar_access_log << "Unknow exception caught" << endl ;
	cerr << "Unable to check access to " << user_name << endl ;
	cerr << "Unknow exception caught" << endl ;
	return 1 ;
    }

    string query_str = "select USER_NAME from cedar_sessions where USER_NAME=\"" ;
    query_str += user_name ;
    query_str += "\";" ;
    try
    {
	//cedar_access_log << "running query " << query_str << endl ;
	query->run_query( query_str );
	//cedar_access_log << "done running query" << endl ;
    }
    catch( DODSException &e )
    {
	//cedar_access_log << "Unable to check access to " << user_name << endl ;
	//cedar_access_log << e.get_error_description() << endl ;
	cerr << "Unable to check access to " << user_name << endl ;
	cerr << e.get_error_description() << endl ;
	return 1 ;
    }
    catch( ... )
    {
	//cedar_access_log << "Unable to check access to " << user_name << endl ;
	//cedar_access_log << "Unknow exception caught" << endl ;
	cerr << "Unable to check access to " << user_name << endl ;
	cerr << "Unknow exception caught" << endl ;
	return 1 ;
    }

    if( !query->is_empty_set() )
    {
	/*
	if( query->get_nrows() != 1 )
	{
	    cerr << "More than one user logged in at ip " << ip << endl ;
	}
	else if( query->get_nfields() != 3 )
	{
	    cerr << "Invalid response to query, " << query->get_nfields()
	         << " fields returned" << endl ;
	}
	else
	{
	    query->first_row() ;
	    query->first_field() ;
	    string curr_time = query->get_field() ;
	    query->next_field() ;
	    string user_name = query->get_field() ;
	    query->next_field() ;
	    string heart_beat = query->get_field() ;

	    struct tm curr_tm ;
	    strptime( curr_time.c_str(), "%Y-%m-%d %T", &curr_tm ) ;
	    time_t curr_t = mktime( &curr_tm ) ;

	    struct tm heart_tm ;
	    strptime( heart_beat.c_str(), "%Y-%m-%d %T", &heart_tm ) ;
	    time_t heart_t = mktime( &heart_tm ) ;

	    double duration = difftime( curr_t, heart_t ) ;

	    if( duration <= AUTH_USERS_TIMEOUT )
	    {
		cout << user_name << endl ;
	    }
	    else
	    {
		cout << "NO_ONE" << endl ;
	    }
	}
	*/
	//cedar_access_log << "\"" << user_name << "\"" << endl ;
	cout << user_name << endl ;
    }
    else
    {
	// this user is not logged in
	//cedar_access_log << "\"NO_ONE\"" << endl ;
	cout << "NO_ONE" << endl ;
    }

    //cedar_access_log << "cleaning up" << endl ;
    // Clean up
    if( query ) delete query ;
    query = 0 ;

    //cedar_access_log << "leaving" << endl ;
    // Leave
    return 0 ;
}

