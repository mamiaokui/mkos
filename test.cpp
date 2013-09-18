#include "MemoryManager.h"
#include "LayerManager.h"
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
    unsigned int addr1 = m.malloc(1024*1024 - 50);
    unsigned int addr2 = m.malloc(1024*1024 - 50);
    unsigned int addr3 = m.malloc(1024*1024 - 50);
    unsigned int addr4 = m.malloc(1024*1024 - 50);

    m.free(addr1, 1024*1024 - 50);
    m.report(foo);
    cout << endl;
    m.free(addr3, 1024*1024 - 50);
    m.report(foo);
    cout << endl;
    m.free(addr2, 1024*1024 - 50);
    m.report(foo);
    cout << endl;
    m.free(addr4, 1024*1024 - 50);
    m.report(foo);
    cout << endl;
}

void test5()
{

    LayerManager* manager = LayerManager::getLayerManager();
    Layer* layer1 = manager->generateLayer();
    layer1->m_x = 0;
    layer1->m_y = 0;
    layer1->m_width = 20;
    layer1->m_height = 10;
    layer1->init(20, 10);
    for (int i = 0; i < 20*10; i++)
    {
        layer1->m_buffer[i] = 'a';
    }
    Layer* layer2 = manager->generateLayer();
    layer2->m_x = 3;
    layer2->m_y = 3;
    layer2->m_width = 20;
    layer2->m_height = 10;
    layer2->init(20, 10);
    for (int i = 0; i < 20*10; i++)
    {
        layer2->m_buffer[i] = 'b';
    }

    Layer* layer3 = manager->generateLayer();
    layer3->m_x = 6;
    layer3->m_y = 6;
    layer3->m_width = 20;
    layer3->m_height = 10;
    layer3->init(20, 10);

    for (int i = 0; i < 20*10; i++)
    {
        layer3->m_buffer[i] = 'c';
    }

    extern unsigned char* globalVram;
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            globalVram[i*100+j] = '0';
        }
    }

    manager->changeZOrderTop(layer1);
    manager->changeZOrderTop(layer2);
    manager->changeZOrderTop(layer3);
    manager->changeZOrderTop(layer1);

    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            cout << globalVram[i*100+j];
        }
        cout << endl;
    }
}

int main()
{
    test5();
}
