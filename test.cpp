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
    Layer* layerBackground = manager->generateLayer(100, 50);

    for (int i = 0; i < 100*50; i++)
    {
        layerBackground->getBuffer()[i] = '.';
    }

    manager->changeZOrderTop(layerBackground);

    Layer* layer1 = manager->generateLayer(16, 16);


    for (int i = 0; i < 16*16; i++)
    {
        layer1->getBuffer()[i] = 'a';
    }
    Layer* layer2 = manager->generateLayer(8, 30);
    for (int i = 0; i < 8*30; i++)
    {
        layer2->getBuffer()[i] = 'b';
    }

    Layer* layer3 = manager->generateLayer(5, 5);

    for (int i = 0; i < 5*5; i++)
    {
        int a = 0;
        layer3->getBuffer()[i] = 'c';
    }


    layer1->setPosition(27, 27);
    layer2->setPosition(3, 3);

    layer2->setPosition(10, 10);
    layer3->setPosition(6, 6);

    layer3->setPosition(30, 30);
    layer2->setPosition(25, 25);


    manager->changeZOrderTop(layer1);
    manager->changeZOrderTop(layer2);
    manager->changeZOrderTop(layer3);
    manager->changeZOrderTop(layer1);
    layer1->setPosition(5, 5);
    layer3->setPosition(5, 5);
    manager->changeZOrderTop(layer3);
    layer3->setPosition(11, 5);
    layer3->setPosition(17, 5);
    layer3->setPosition(23, 5);
    layer3->setPosition(29, 5);
    layer3->setPosition(36, 5);
    extern unsigned char* globalVram;
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            cout << globalVram[i*100+j];
        }
        cout << endl;
    }
    
    while(true)
    {
        char command;
        int layer, value;
        cin >> command >> layer >>value;
        switch(command)
        {
        case 'm':
            manager->m_layerContainer[layer].setPosition(manager->m_layerContainer[layer].getX() + value,manager->m_layerContainer[layer].getY());
            break;
        case 'c':
            manager->changeZOrderTop(&manager->m_layerContainer[layer]);
            break;
        }
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                cout << globalVram[i*100+j];
            }
            cout << endl;
        }

    }

}

void test6()
{
        LayerManager* manager = LayerManager::getLayerManager();
        int a[4] = {27, 27, 20, 10};
        int b[4] = {25, 25, 20, 10};
        manager->rectClip(a, b);
        cout << a[0] << a[1] << a[2] << a[3]<< endl;
}

void test7()
{
        LayerManager* manager = LayerManager::getLayerManager();
        int a[4] = {27, 27, 16, 16};
        int b[4] = {30, 30, 20, 10};
        manager->rectClip(a, b);
        cout << a[0] <<' ' << a[1] <<' '<< a[2] <<' '<< a[3]<< endl;
}


int main()
{
    test5();
}
