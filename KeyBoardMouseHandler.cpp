#include "KeyBoardMouseHandler.h"
#include "MemoryManager.h"
#include "AsmTools.h"
#include "PaintPack.h"
#include "LayerManager.h"
#include "GdtIdt.h"
#include "InterruptionBuffer.h"
#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

KeyBoardMouseHandler* KeyBoardMouseHandler::m_handler = 0;

MouseDataDecoder::MouseDataDecoder()
{
    m_mouseState = 0;
    for (int i = 0; i < 3; i++)
    {
        m_mouseRawData[i] = 0;
        m_mouseDecodeData[i] = 0;
    }
}

bool MouseDataDecoder::receiveMouseInterruption(int data)
{
    bool ok = false;
    if (m_mouseState == 0)
    {
        if (data == 0xfa)
            m_mouseState = 1;
    }
    else if (m_mouseState == 1)
    {
        if ((data & 0xc8) == 0x08)
        {
            //right
            m_mouseRawData[0] = data;
            m_mouseState = 2;
        }
    }
    else if (m_mouseState == 2)
    {
        m_mouseRawData[1] = data;
        m_mouseState = 3;
    }
    else if (m_mouseState == 3)
    {
        m_mouseRawData[2] = data;
        m_mouseState = 1;
        m_mouseDecodeData[0] = m_mouseRawData[0] & 0x07;
        m_mouseDecodeData[1] = m_mouseRawData[1];
        m_mouseDecodeData[2] = m_mouseRawData[2];
        if ((m_mouseRawData[0] & 0x10) != 0)
        {
            m_mouseDecodeData[1] |= 0xffffff00;
        }
        else if ((m_mouseRawData[0] & 0x20) != 0)
        {
            m_mouseDecodeData[2] |= 0xffffff00;                
        }

        m_mouseDecodeData[2] = -m_mouseDecodeData[2];
        ok = true;
    }
    return ok;
}

int* MouseDataDecoder::getMouseData()
{
    return m_mouseDecodeData;
}

KeyBoardMouseHandler* KeyBoardMouseHandler::getHandler()
{
    if (m_handler == 0)
    {
        extern MemoryManager* globalMemoryManager;
        m_handler = (KeyBoardMouseHandler*) globalMemoryManager->malloc(sizeof(KeyBoardMouseHandler));
    }
    return m_handler;
}

void KeyBoardMouseHandler::waitKeyboardReady(void)
{
	for (;;) {
		if ((asmIn8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void KeyBoardMouseHandler::initKeyboard(void)
{
	waitKeyboardReady();
	asmOut8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	waitKeyboardReady();
	asmOut8(PORT_KEYDAT, KBC_MODE);
	return;
}


void KeyBoardMouseHandler::enableMouse(void)
{
	waitKeyboardReady();
	asmOut8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	waitKeyboardReady();
	asmOut8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return;
}

bool KeyBoardMouseHandler::receiveMouseInterruption(int data)
{
    return m_mouseDataDecoder.receiveMouseInterruption(data);
}

void KeyBoardMouseHandler::initMouseCursorImage(unsigned char* layerBuffer)
{
    unsigned char mouseCursorImage[256];
    initMouseCursor(mouseCursorImage);
    paintBlock(layerBuffer, 16, 16, 16, 0, 0, mouseCursorImage);
}

void KeyBoardMouseHandler::init(Layer* layerMouse)
{
    m_layerMouse = layerMouse;
    initKeyboard();
    enableMouse();
    initMouseCursorImage(m_layerMouse->getBuffer());
}

void KeyBoardMouseHandler::moveLayerToMiddle()
{
    LayerManager::getLayerManager()->moveLayerToMiddle(m_layerMouse);
    m_mouseX = m_layerMouse->getX();
    m_mouseY = m_layerMouse->getY();
}

bool KeyBoardMouseHandler::handleMouseInput(int data)
{
    bool output =  receiveMouseInterruption(data - 512);
    if (output)
    {
        int* outputData = getMouseData();

        m_mouseX += outputData[1];
        //fix me. don't know the true reason. mouse always receive large negative y move.
        //temp fix.
        if (outputData[2] < -200)
        {
            outputData[2] = 0;
        }
        m_mouseY += outputData[2];
        if (m_mouseX < 0)
        {
            m_mouseX = 0;
        }
        if (m_mouseY < 0)
        {
            m_mouseY = 0;
        }
        int screenWidth = LayerManager::getLayerManager()->getScreenWidth();
        int screenHeight = LayerManager::getLayerManager()->getScreenHeight();
        if (m_mouseX > screenWidth - 16)
            m_mouseX = screenWidth -16;

        if (m_mouseY > screenHeight - 16)
            m_mouseY = screenHeight -16;

        m_layerMouse->setPosition(m_mouseX, m_mouseY);
    }
    return output;
}

#define PORT_KEYBOARD		0x0060

void int21Handler(int* arg)
{
    asmOut8(PIC0_OCW2, 0x61);
    int data;
	data = asmIn8(PORT_KEYBOARD);
    //use the same interruption buffer, so add one number to change the level.
	InterruptionBuffer::getInterruptionBuffer()->inputInterruptionBuffer(data);
}


void int2cHandler(int* arg)
{
	int data;
	asmOut8(PIC1_OCW2, 0x64);
	asmOut8(PIC0_OCW2, 0x62);
	data = asmIn8(PORT_KEYBOARD);
    //same as int21
	InterruptionBuffer::getInterruptionBuffer()->inputInterruptionBuffer(data + 512);
	return;
}

