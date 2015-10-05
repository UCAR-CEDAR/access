// DODSMySQLQuery.cc

// 2004 Copyright University Corporation for Atmospheric Research

#include "DODSMySQLQuery.h"
#include "DODSException.h"
#include "DODSMySQLConnect.h"
#include "DODSMySQLChannel.h"

DODSMySQLQuery::DODSMySQLQuery(const string &server, const string &user,
			       const string &password, const string &dataset)
{
    _my_connection = 0 ;
    _results = 0 ;

    _my_connection = new DODSMySQLConnect ;

    try
    {
	_my_connection->open( server, user, password, dataset ) ;
    }
    catch( DODSException &ce )
    {
	if( _my_connection ) delete _my_connection ;
	_my_connection = 0 ;
	throw ce ;
    }
}

DODSMySQLQuery::~DODSMySQLQuery()
{
    if( _results ) 
    {
	delete _results;
    }
    if( _my_connection )
    {
	delete _my_connection;
    }
}

void
DODSMySQLQuery::run_query(const string &query)
{
    MYSQL *sql_channel = _my_connection->get_channel() ;
    if( mysql_query( sql_channel, query.c_str() ) )
    {
	DODSException qe ;
	qe.set_error_description( _my_connection->get_error() ) ;
	throw qe ;
    }
    else
    {
	if( _results ) delete _results ;
	_results = 0 ;
	MYSQL_RES *result = 0 ;
	MYSQL_ROW row ;
	result = mysql_store_result( sql_channel ) ;
	int n_rows = mysql_num_rows( result) ;
	int n_fields = mysql_num_fields( result ) ;
	_results = new DODSMySQLResult( n_rows, n_fields ) ;
	_results->first_row() ;
	while( (row=mysql_fetch_row( result )) != NULL )
	{
	    _results->first_field() ;
	    for( int i=0; i<n_fields; i++ )
	    {
		_results->set_field( row[i] ) ;
		_results->next_field() ;
	    }
	    _results->next_row() ;
	}
	mysql_free_result( result ) ;
    }
}

// $Log: DODSMySQLQuery.cc,v $
// Revision 1.1  2005/07/07 19:23:07  pwest
// check_access executable for cedar_file_access.pl script
//
// Revision 1.2  2004/09/09 17:17:12  pwest
// Added copywrite information
//
// Revision 1.1  2004/06/30 20:16:24  pwest
// dods dispatch code, can be used for apache modules or simple cgi script
// invocation or opendap daemon. Built during cedar server development.
//
