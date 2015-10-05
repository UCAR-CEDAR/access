using std::string ;

#include <string>

#ifndef CEDARException_h_
#define CEDARException_h_ 1

class CEDARException
{
protected:
  string _description;
public:
    CEDARException(){_description="UNDEFINED";}
    CEDARException(const string &s){_description=s;}
    virtual void set_error_description(const string &s) {_description=s;};
    virtual string get_error_description(){return _description;}
};

#endif // CEDARException
