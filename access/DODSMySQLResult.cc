// DODSMySQLResult.cc

#include "DODSMySQLResult.h"

DODSMySQLResult::DODSMySQLResult( const int &n, const int &f )
    : _nrows( n ),
      _nfields( f )
{
    _matrix=new matrix ;
    _matrix->reserve( _nrows ) ;
    row r ;
    r.reserve( _nfields ) ;
    for( register int j=0; j<_nrows; j++ )
	_matrix->push_back( r ) ;
}

DODSMySQLResult::~DODSMySQLResult()
{
    delete _matrix;
}

void
DODSMySQLResult::set_field( const char *s )
{
    string st = s ;
    (*_matrix)[_row_position].push_back( st ) ;
}

string
DODSMySQLResult::get_field()
{
    return (*_matrix)[_row_position][_field_position];
}

bool
DODSMySQLResult::first_field()
{
    if( _nfields > 0 )
    {
	_field_position = 0 ;
	return true ;
    }
    return false ;
}

bool
DODSMySQLResult::next_field()
{
    if( ++_field_position < _nfields )
	return true ;
    return false ;
}

bool
DODSMySQLResult::next_row()
{
    if( ++_row_position < _nrows )
	return true ;
    return false ;
}

bool
DODSMySQLResult::first_row()
{
    if( _nrows > 0 )
    {
	_row_position = 0 ;
	return true ;
    }
    return false ;
}

