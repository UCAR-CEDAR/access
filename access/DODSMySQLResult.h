// DODSMySQLResult.h

#ifndef DODSMySQLResult_h_
#define DODSMySQLResult_h_ 1

#include <string>
#include <vector>

using std::vector ;
using std::string ;

class DODSMySQLResult
{
private:
    typedef vector<string> row;
    typedef vector<row> matrix;

    matrix *		_matrix;
    int			_nrows;
    int			_nfields;
    int			_row_position;
    int			_field_position;

    			DODSMySQLResult( const DODSMySQLResult& ) {}
public:
    			DODSMySQLResult( const int &n_rows,
					 const int &n_fields ) ; 
    			~DODSMySQLResult();

    int			get_nrows() { return _nrows ; }
    int			get_nfields() { return _nfields ; }
    void		set_field( const char *s ) ;
    string		get_field() ;
    bool		first_field() ;
    bool		next_field() ;
    bool		next_row() ;
    bool		first_row() ;
} ;

#endif //DODSMySQLResult_h_

