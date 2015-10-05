#ifndef CEDARAutoPtr_h_
#define CEDARAutoPtr_h_ 1

#include <iostream>

extern bool debug;

/**
   Control of pointers for multiple executions paths and memory management.
 */

template <class T>
class CEDARAutoPtr
{
    T* p;
    
    bool _is_vector;
    
    /// disable copy constructor.
    template <class U>
    
    CEDARAutoPtr(CEDARAutoPtr<U> &){};
    
    /// disable overloaded = operator.
    template <class U>
    CEDARAutoPtr<T>& operator= (CEDARAutoPtr<U> &){};

public:
    ///
    explicit CEDARAutoPtr(T* pointed=0, bool v=false)
    {
	p=pointed;
	_is_vector=v;
	if (debug)
	    cout<<"Took control of pointer "<<p<<endl;
    }

    ///
    ~CEDARAutoPtr(){
	if (debug)
	    cout<<"Destroying pointer "<<p<<endl;
	if (_is_vector) 
	    delete [] p; 
	else 
	    delete p;
	p=0;
    }

    ///
    T* set(T *pointed, bool v=false){
	T* temp=p;
	p=pointed;
	_is_vector=v;
	return temp;
    }

    ///
    T* get() const{
	return p;
    }

    ///
    T* operator ->() const{
	return p;
    }

    ///
    T& operator *() const{
	return *p;
    }

    ///
    T* release(){
	T* old = p;
	p=0;
	return old;
    }

    ///
    void reset(){
	if (_is_vector) 
	    delete [] p; 
	else 
	    delete p;
	p=0;
    }
};

#endif // CEDARAutoPtr_h_
