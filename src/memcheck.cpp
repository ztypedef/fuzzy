#include "memcheck.h"
#include <stdio.h>
#include <algorithm>

void Memcheck::add_ptr(void* ptr)
{
	mptr.insert(std::pair<unsigned, std::string>((unsigned)ptr, ""));
}

void Memcheck::add_ptr(void* ptr, std::string str)
{
	mptr.insert(std::pair<unsigned, std::string>((unsigned)ptr, str));
}

void Memcheck::rm_ptr(void* ptr)
{
	std::map<unsigned, std::string>::iterator it;
	it = mptr.find((unsigned)ptr);
	if(it == mptr.end()) 
	{
		printf("[MEMCHECK] pointer no found: 0x%x", (unsigned)ptr);
		return;
	}
	mptr.erase(it); 
}

void Memcheck::print_ptr()
{
	if(mptr.size() == 0)
	{
			printf("[MEMCHECK] no pointer\n");
			return;
	}
	
	std::map<unsigned, std::string>::iterator it = mptr.begin();
	for(it = mptr.begin(); it != mptr.end(); ++it)
		printf("[MEMCHECK] pointer: 0x%x is %s\n", it->first, (it->second).c_str());
				
    /*
	for(unsigned i = 0; i < mptr.size(); i++)
	{
		printf("[MEMCHECK] pointer: 0x%x\n",vptr[i]);
	}*/
}
