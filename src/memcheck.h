#ifndef MEMCHACK_H
#define MEMCHACK_H

#include <map>
#include <string>

//#define PRINT_DEFAULT
#define MEMCHECK Memcheck::getInstance()



class Memcheck
{
public:
	static Memcheck& getInstance() {
        static Memcheck  instance;
        return instance;
    } 
	void add_ptr(void* ptr);
	void add_ptr(void* ptr, std::string str);
	void rm_ptr(void* ptr);
	void print_ptr();
private:
    Memcheck() {}
    Memcheck(const Memcheck&); 
    Memcheck& operator = (Memcheck&);
    
	std::map<unsigned, std::string> mptr;
};


#endif
