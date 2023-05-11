#pragma once
#include "AsyncTCP_SSL.hpp"
#include "mStream.hpp"

class mStream;

class AsyncTCP_RW
{
private:
    typedef enum {
        ATB_RX_MODE_NONE,
        ATB_RX_MODE_READ_FREE,
        ATB_RX_MODE_READ_BYTES,
        ATB_RX_MODE_TERMINATOR_STRING
    } atbRxMode_t;

public:
    typedef std::function<void(bool ok, void * ret)> AsyncTCPbufferDoneCb;
    typedef std::function<bool(AsyncTCP_RW * obj)> AsyncTCPbufferDisconnectCb;

public:
    explicit AsyncTCP_RW(AsyncSSLClient* client);
    ~AsyncTCP_RW();

    void updateTCPClient(AsyncSSLClient* client);
    void clearTCPCallbacks();

    size_t write(String & data);
    size_t write(uint8_t data);
    size_t write(const char* data);
    size_t write(const char *data, size_t len);
    size_t write(const uint8_t *data, size_t len);

    void readStringUntil(char terminator, String * str, AsyncTCPbufferDoneCb done);
    void readBytes(uint8_t *buffer, size_t length, AsyncTCPbufferDoneCb done);

    bool connected();// {return mClient->connected();}

    AsyncSSLClient* getAsyncTCP() {return mClient;}

    void onDisconnect(AsyncTCPbufferDisconnectCb cb);

    void stop();

    IPAddress remoteIP();

private:
    void _attachCallbacks();
    void _rxData(uint8_t *buf, size_t len);
    void _handleRxBuffer(mStream& buf);

private:
    AsyncSSLClient*            mClient;
    size_t                     _rxSize;
    char                       _rxTerminator;
    AsyncTCPbufferDoneCb       _cbDone;
    AsyncTCPbufferDisconnectCb _cbDisconnect;
    atbRxMode_t                _RXmode;
    mStream*                   _RXbuffer;
    String*                    _rxReadStringPtr;
};