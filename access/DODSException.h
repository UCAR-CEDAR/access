// DODSException.h

#ifndef DODSException_h_
#define DODSException_h_ 1

#include <string>

using std::string ;

class DODSException
{
protected:
    string		_description;
public:
    			DODSException() { _description = "UNDEFINED" ; }
    			DODSException( const string &s ) { _description = s ; }
    virtual		~DODSException() {}

    virtual void	set_error_description( const string &s )
			    { _description = s ; }
    virtual string	get_error_description()
			    { return _description ; }
};

#endif // DODSException_h_ 

