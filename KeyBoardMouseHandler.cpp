#include "KeyBoardMouseHandler.h"
#include "MemoryManager.h"
#include "AsmTools.h"
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
