#include "AsyncTCP_RW.hpp"
#include "mStream.hpp"

AsyncTCP_RW::AsyncTCP_RW(AsyncSSLClient* client)
    : mClient(client)
    , _rxSize(0)
    , _rxTerminator(0)
    , _cbDone(nullptr)
    , _cbDisconnect(nullptr)
    , _RXmode(ATB_RX_MODE_NONE)
    , _RXbuffer(new mStream)
    , _rxReadStringPtr(nullptr)
{
    _attachCallbacks();
}

void AsyncTCP_RW::updateTCPClient(AsyncSSLClient* client)
{
    mClient = client;
    _rxSize = 0;
    _rxTerminator = 0;
    _cbDone = nullptr;
    _cbDisconnect = nullptr;
    _RXmode = ATB_RX_MODE_NONE;
    _RXbuffer->removeBuffer();
    _rxReadStringPtr = nullptr;
    _attachCallbacks();
}

void AsyncTCP_RW::clearTCPCallbacks()
{
    mClient->onData(nullptr, nullptr);
    mClient->onTimeout(nullptr, nullptr);
    mClient->onDisconnect(nullptr, nullptr); 
}

bool AsyncTCP_RW::connected() 
{
    // if (mClient == nullptr) 
    //     return false;
    // else
        return mClient->connected();
}

AsyncTCP_RW::~AsyncTCP_RW()
{
    delete _RXbuffer;
}

void AsyncTCP_RW::_attachCallbacks()
{
    mClient->onData([](void *obj, AsyncSSLClient* c, void *buf, size_t len) {
        (void)c;
        AsyncTCP_RW* b = ((AsyncTCP_RW*)(obj));
        b->_rxData((uint8_t *)buf, len);
    }, this);

    mClient->onTimeout([](void *obj, AsyncSSLClient* c, uint32_t time){
        (void)time;
        AsyncTCP_RW* b = ((AsyncTCP_RW*)(obj));
        c->close();
        b->_rxSize = 0;
        b->_rxTerminator = 0;
        b->_cbDone = nullptr;
        b->_RXmode = ATB_RX_MODE_NONE;
        b->_RXbuffer->removeBuffer();
        b->_rxReadStringPtr = nullptr;
    }, this);

    mClient->onDisconnect([](void *obj, AsyncSSLClient* c) {
        AsyncTCP_RW* b = ((AsyncTCP_RW*)(obj));
        // b->mClient = NULL;
        bool del = true;
        if(b->_cbDisconnect) {
            del = b->_cbDisconnect(b);
        }
        // delete c;
        if(del) {
            delete b;
        }
    }, this);
}

size_t AsyncTCP_RW::write(String & data)
{
    return write(data.c_str(), data.length());
}

size_t AsyncTCP_RW::write(uint8_t data)
{
    return write(&data, 1);
}

size_t AsyncTCP_RW::write(const char* data)
{
    return write((const uint8_t *) data, strlen(data));
}

size_t AsyncTCP_RW::write(const char *data, size_t len)
{
    return write((const uint8_t *) data, len);
}

size_t AsyncTCP_RW::write(const uint8_t *data, size_t len)
{
    if (mClient == nullptr) return 0;
    size_t rest_len = len;
    size_t to_send = 0;

    while(rest_len > 0) 
    {
        if (!mClient->connected()) return 0;

        size_t available = mClient->space();

        if (rest_len <= available)
        {
            to_send = rest_len;
        }
        else
        {
            to_send = available;
        }

        while(!mClient->canSend())
        {
            vTaskDelay(pdMS_TO_TICKS(1));
            if (!mClient->connected()) return 0;
        }

        size_t sended = mClient->write((const char*) data + (len - rest_len), to_send);

        if (sended > 0) rest_len -= to_send;
    }
    return len;
}

void AsyncTCP_RW::readStringUntil(char terminator, String * str, AsyncTCPbufferDoneCb done)
{
    if (mClient == nullptr) return;
    _cbDone = done;
    _rxReadStringPtr = str;
    _rxTerminator = terminator;
    _rxSize = 0;
    _RXmode = ATB_RX_MODE_TERMINATOR_STRING;
    mClient->setRxTimeout(5);
}

void AsyncTCP_RW::readBytes(uint8_t *buffer, size_t length, AsyncTCPbufferDoneCb done)
{
    if (mClient == nullptr) return;
    _rxSize = length;
    _cbDone = done;
    _RXmode = (length == 0) ? ATB_RX_MODE_READ_FREE : ATB_RX_MODE_READ_BYTES;
    _RXbuffer->loadBuffer(buffer, length);
    // mClient->setRxTimeout(5);
}

void AsyncTCP_RW::_rxData(uint8_t *buf, size_t len)
{
    mStream localBuf(buf, len);
    while(localBuf.free_space() && _RXmode != ATB_RX_MODE_NONE)
    {
        _handleRxBuffer(localBuf);
    }
}

void AsyncTCP_RW::_handleRxBuffer(mStream& buf)
{
    if (mClient == nullptr) return;

    if (_RXmode == ATB_RX_MODE_READ_BYTES)
    {
        _rxSize -= _RXbuffer->writeBytes(buf);
        if (_rxSize == 0)
        {
            // mClient->setRxTimeout(0);
            _RXmode = ATB_RX_MODE_NONE;
            if (_cbDone != nullptr)
            {
                _cbDone(true, nullptr);
            }
            // _cbDone = nullptr;
            
            // _RXbuffer->removeBuffer();
        }
    }
    else if (_RXmode == ATB_RX_MODE_TERMINATOR_STRING)
    {
        for (int i = 0; i < buf.free_space(); i++)
        {
            char c = buf.get();
            if(c == _rxTerminator || c == 0x00) 
            {
                mClient->setRxTimeout(0);
                _RXmode = ATB_RX_MODE_NONE;
                //_rxReadStringPtr = nullptr;
                //_rxTerminator = 0;
                if (_cbDone != nullptr) _cbDone(true, _rxReadStringPtr);
                return;
            } 
            else 
            {
                (*_rxReadStringPtr) += c;
            }
        }
    }
    else if (_RXmode == ATB_RX_MODE_READ_FREE) //______________________TODO___________________
    {
        _RXbuffer->writeBytes(buf);
        if (_rxSize == 0)
        // mClient->setRxTimeout(0);
        if (_cbDone != nullptr)
        {
            _cbDone(true, nullptr);
        }
    }
}

void AsyncTCP_RW::stop()
{
    if(!mClient) return;

    mClient->stop();
    // mClient = NULL;

    // if(_cbDone) {
    //     switch(_RXmode) {
    //         case ATB_RX_MODE_READ_BYTES:
    //         case ATB_RX_MODE_TERMINATOR_STRING:
    //             _RXmode = ATB_RX_MODE_NONE;
    //             _cbDone(false, NULL);
    //             break;
    //         default:
    //           break;
    //     }
    // }
    _RXmode = ATB_RX_MODE_NONE;
}

void AsyncTCP_RW::onDisconnect(AsyncTCPbufferDisconnectCb cb)
{
    _cbDisconnect = cb;
}

IPAddress AsyncTCP_RW::remoteIP() 
{
    if(!mClient) {
        return IPAddress(0U);
    }
    return mClient->remoteIP();
}