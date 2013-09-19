class MouseDataDecoder 
{
private:
    int m_mouseState;
    int m_mouseRawData[3];
    int m_mouseDecodeData[3];
public:
    MouseDataDecoder()
    {
        m_mouseState = 0;
        for (int i = 0; i < 3; i++)
        {
            m_mouseRawData[i] = 0;
            m_mouseDecodeData[i] = 0;
        }
    }

    bool receiveMouseInterruption(int data)
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

    int* getMouseData()
    {
        return m_mouseDecodeData;
    }
};
