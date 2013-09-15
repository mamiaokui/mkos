#include "MemoryManager.h"
#include "iostream"
using namespace std;
void foo(FreeItem& item)
{
    cout << std::hex << item.m_address << " " << std::dec << item.m_size/1024/1024<< endl;
}
void test1(MemoryManager& m) 
{
    m.malloc(1024*1024);
    m.report(foo);
}

void test2(MemoryManager& m) 
{
    m.malloc(1024*1024);
    m.malloc(1024*1024);
    m.report(foo);
}

void test3(MemoryManager& m) 
{
    m.malloc(1024*1024);
    m.malloc(1024*1024);
    m.malloc(1024*1024);
    m.report(foo);
}

void test4(MemoryManager& m) 
{
    unsigned int addr1 = m.malloc(1024*1024);
    unsigned int addr2 = m.malloc(1024*1024);
    unsigned int addr3 = m.malloc(1024*1024);
    unsigned int addr4 = m.malloc(1024*1024);

    m.free(addr1, 1024*1024);
    m.report(foo);
    cout << endl;
    m.free(addr3, 1024*1024);
    m.report(foo);
    cout << endl;
    m.free(addr2, 1024*1024);
    m.report(foo);
    cout << endl;
    m.free(addr4, 1024*1024);
    m.report(foo);
    cout << endl;
}

int main()
{
    MemoryManager m;
    test4(m);
}
