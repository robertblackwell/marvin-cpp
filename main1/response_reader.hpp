#ifndef request_reader_hpp
#define request_reader_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "mock_buffer.hpp"
#include "Message.hpp"

typedef std::function<void(Error& err)>                                 BodyEndCbType;
typedef std::function<void(Error& err, std::size_t bytes_transfered)>   BodyCbType;
typedef std::function<void(Error& err, MBuffer& chunk)>                 ChunkCbType;
typedef std::function<void(Error& err, MessageInterface* msg)>          HeadersCbType;
typedef std::function<void(Error& err, MessageInterface* msg)>          ResponseCbType;

/**
 * Instances of this class read an incoming http(s) response message from a stream
 *
 * @TODO - trying to wworkout how to manage the body
 * @TODO - want the response to message to BE this class so waht to base
 * ResponseREader on MessageBase
 */
class ResponseReader : public Parser, public MessageBase
{
public:
    ResponseReader(Connection& conn, boost::asio::io_service& io): _io(io), _conn(conn)
    {
        _headersComplete = false;
        _messageComplete = false;
        _cachedBodyDataLength = 0;
        _cachedBodyData  = nullptr;
        _readBodyStarted = false;
        
    }
    ~ResponseReader()
    {
        delete readBuffer;
    }
    void OnParseBegin(){
        std::cout << __FUNCTION__ << std::endl;
        Parser::OnParseBegin();
    }
    void OnHeadersComplete(MessageInterface* msg){
        std::cout << __FUNCTION__ << std::endl;
        _headersComplete = true;
    }
    void OnMessageComplete(HTTPMessage* msg){
        std::cout << __FUNCTION__ << std::endl;
        _messageComplete = true;
    }
    void OnBodyData(void* buf, int len)
    {
        //
        // I would like to be able to guarentee that the memory pointed to by "buf"
        // would stay valid long enough to be used without memcpy'ing it
        //
        if( ! _readBodyStarted ){
            _cachedBodyData = malloc(len+1);
            memcpy(_cachedBodyData, buf, len),
            ((char*)_cachedBodyData)[len] = (char)0;
            _cachedBodyDataLength   = len;
        }else{
            // we are getting body data from a read that was explicitly initiated.
            // maybe we want to store it as what is passed in is "de-chunked" data
            
            //
            // Need to do something better here - add start and end pointers to the readBuffer
            //
            
            if( _dechunkedData != nullptr ){
                free(_dechunkedData);
                _dechunkedDataLength = 0;
            }
            _dechunkedData = malloc(len);
            memcpy(_dechunkedData, buf, len);
            _dechunkedDataLength = len;
        }
        std::cout << "ResponseReader::" << __FUNCTION__ << " :" << std::string((char*)buf, len) << std::endl;
    };
    MessageInterface* currentMessage(){
        MessageInterface* m = this;
        return this;
    }
    void readChunk(ChunkCbType cb)
    {
        this->_chunkCb = cb;
    }
    void readBodyData(MBuffer& mb, AsyncReadCallback cb)
    {
        _readBodyStarted = true; // signal an explicit readBosyData has been issued
        //
        // the first readBodyData should return any cached data from the previous
        // read that completed the headers
        //
        this->_bodyCb = cb;
        if( _cachedBodyDataLength > 0 ){
            std::cout << __FUNCTION__ << std::endl;
            Error* er = ( isFinishedMessage() )? Error::end_of_message() : Error::success();
            void* p = mb.data();
            std::size_t sz = _cachedBodyDataLength;
            memcpy(p, _cachedBodyData, sz);
            mb.setSize(_cachedBodyDataLength);
            auto pf = std::bind(_bodyCb, *er, mb.size());
            _cachedBodyDataLength = 0; // signals that the cache has been used
            _io.post(pf);
            
        }else{
            if( _cachedBodyData != nullptr ) {
                free(_cachedBodyData); _cachedBodyData = nullptr;
            }
            _conn.asyncRead(mb, cb);
        }
    }
    void readBodyHandler(MBuffer& mb){
    }
    void onError(std::string err)
    {
        Error* er = new Error(err);
        this->_responseCb(*er, NULL);
    }

    void startRead(){
        auto h = std::bind(&ResponseReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
        _conn.asyncRead(*readBuffer, h);
    }
    void asyncReadHandler(Error er, std::size_t bytes_transfered){
        readBuffer->setSize(bytes_transfered);
        MBuffer& mb = *readBuffer;
        bool saved_EOH = isFinishedHeaders();
        bool saved_EOM = isFinishedMessage();
        int nparsed;
        if( mb.size() == 0 ){
            this->appendEOF();
        }else{
            nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
            char* tt = (char*) mb.data();
            if( isFinishedHeaders() && ! saved_EOH ){
                Error* er = Error::success();
                MessageInterface* m = currentMessage();
                auto pf = std::bind(_responseCb, *er, m);
                //
                // This buffer finished the headers.
                // See if any body data came in  and if so cache it
                //
                if( isFinishedMessage() ){
                    // We have all of the message so save the body data that has been captured
                    // in _cachedBodyData and mark message as ended
                    std::cout << "got EOM as well" << std::endl;
                    
                }else if( _cachedBodyDataLength != 0 ){
                    // we dont have all the body but we do have some. Cache the body data
                    // and prepare for readBodyData calls
                    int ofset = (int)mb.size() - (int)_cachedBodyDataLength;
                    char* bdy = ((char*)mb.data()) + ofset;
                    std::cout << "not EOM but did get some body data" << std::endl;
                } else {
                    std::cout << "not EOM and no body data" << std::endl;
                    // we dont have any of the body data - maybe there is NON ??
                    // @TODO - need some experiments to see if parser takes content-length header
                    // into account when signalling EOM
                }
                _io.post(pf);
            }else if(! isFinishedHeaders() ) {
                // finish the headers
                startRead();
            }else if( ! saved_EOM ){
                // now doing the body OR maybe should not be here
                std::cout << __FUNCTION__ << std::endl;
                Error er = ( isFinishedMessage() )? Error::eom() : Error::ok();
                // There is an issue here - need to know when the body cb is finished so can start the next read
                auto pf = std::bind(_bodyCb, er, bytes_transfered);
                _io.post(pf);

            }else{
                std::cout << __FUNCTION__ << std::endl;
            }
            
        }
    }
    void readResponse(ResponseCbType cb)
    {
        // need to set up a buffer
        readBuffer = new MBuffer(10000);
        
        int nparsed;
        this->_responseCb = cb;
        startRead();
    }
        
private:
    Connection&                 _conn;
    bool                        _headersComplete;
    bool                        _messageComplete;
    boost::asio::io_service&    _io;
    
    //read buffer
    MBuffer*        readBuffer;
    
    // These 3 properties handle the situation where body data arrives while still processing the headers
    bool            _readBodyStarted;
    void*           _cachedBodyData;
    std::size_t     _cachedBodyDataLength;
    
    // These 2 properties provide access to the de-chunked data that is provided by the c parser
    void*           _dechunkedData;
    std::size_t     _dechunkedDataLength;
    
    BodyEndCbType   _bodyEndCb;
    BodyCbType      _bodyCb;
    ChunkCbType     _chunkCb;
    HeadersCbType   _headersCb;
    ResponseCbType  _responseCb;
};

#endif