#ifndef request_reader_hpp
#define request_reader_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "buffer.hpp"
#include "Message.hpp"
#include "Parser.hpp"

typedef std::function<void(Error& err)>                                 BodyEndCbType;
typedef std::function<void(Error& err, std::size_t bytes_transfered)>   BodyCbType;
typedef std::function<void(Error& err, MBuffer& chunk)>                 ChunkCbType;
typedef std::function<void(Error& err, MessageInterface* msg)>          HeadersCbType;
typedef std::function<void(Error& err, MessageInterface* msg)>          ResponseCbType;

typedef std::function<void(Error& err, FBufferUniquePtr bPtr)>                AsyncReadBodyCallback;

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
        _dechunkedData   = nullptr;
        _dechunkedDataLength = 0;

    }
    ~ResponseReader()
    {
        delete readBuffer;
    }
    void OnParseBegin(){
        std::cout << __FUNCTION__ << std::endl;
        Parser::OnParseBegin();
    }
    void OnParserError(){
        std::cout << __FUNCTION__ << std::endl;        
    }
    void OnHeadersComplete(MessageInterface* msg){
        std::cout << __FUNCTION__ << std::endl;
        _headersComplete = true;
    }
    void OnMessageComplete(MessageInterface* msg){
        std::cout << __FUNCTION__ << std::endl;
        _messageComplete = true;
    }
    void OnBodyData(void* buf, int len)
    {
        //
        // I would like to be able to guarentee that the memory pointed to by "buf"
        // would stay valid long enough to be used without memcpy'ing it
        //
        std::string tmp((char*)buf, len);
        std::cout << "ResponseReader::OnBodyData : " << tmp << std::endl;
        std::cout << " buf: " << std::hex << (long)buf << std::endl;
        std::cout << " readBuffer: " << std::hex << (long)(readBuffer->data()) << std::endl;
//        std::cout << " bodyBuffer: " << std::hex << (long)(bodyBuffer->data()) << std::endl;
        std::cout << " buf+len: " << std::hex << (long)((char*)buf+len) << std::endl;
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
    void readBody(AsyncReadBodyCallback cb)
    {
        _bodyCallback = cb;
        _bodyMBuffer = new MBuffer(10000);
        std::unique_ptr<FBuffer> tmp(new FBuffer(_bodyMBuffer));
        _bodyFBufferUniquePtr = std::move(tmp);
        
        
    }
    void readBodyData(MBuffer& mb, AsyncReadCallback cb)
    {
        _readBodyStarted = true; // signal an explicit readBodyData has been issued
        bodyBuffer = &mb;
        //
        // the first readBodyData should return any cached data from the previous
        // read that completed the headers
        //
        this->_bodyCb = cb;
        if( _cachedBodyDataLength > 0 ){
            std::cout << "ResponseReader::" << __FUNCTION__ << " cachedBody " << _cachedBodyDataLength << " ";
            std::cout << std::hex << (long)_cachedBodyData;
            std::cout << std::endl;
            
            Error* er = ( isFinishedMessage() )? Error::end_of_message() : Error::success();
            void* p = mb.data();
            std::size_t sz = _cachedBodyDataLength;
            memcpy(p, _cachedBodyData, sz);
            mb.setSize(_cachedBodyDataLength);
            auto pf = std::bind(_bodyCb, *er, mb.size());
            _cachedBodyDataLength = 0; // signals that the cache has been used
            _io.post(pf);
            
        }else{
            std::cout << "ResponseReader::" << __FUNCTION__ << " NOT cachedBody " << std::endl;
            if( _cachedBodyData != nullptr ) {
                free(_cachedBodyData); _cachedBodyData = nullptr;
            }
            startRead();
//            _conn.asyncRead(mb, cb);
        }
    }
    void readBodyHandler(MBuffer& mb){
    }
    void onError(std::string err)
    {
        Error* er = new Error(err);
        this->_responseCb(*er, NULL);
    }

    void handleError(Error er){
        
    }
    void handleParseError(){
        
    }
    
    void startRead(){
        auto h = std::bind(&ResponseReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
        _conn.asyncRead(*readBuffer, h);
    }
    void asyncReadHandler(Error er, std::size_t bytes_transfered){
        std::cout << "ResponseReader::" << __FUNCTION__ << std::endl;
//        if( er != Error::success() ){
//            handleError(er);
//            return;
//        }
        readBuffer->setSize(bytes_transfered);
        MBuffer& mb = *readBuffer;
        bool saved_EOH = isFinishedHeaders();
        bool saved_EOM = isFinishedMessage();
        int nparsed;
        int sz = (int)mb.size();
        std::cout << "ResponseReader::" << __FUNCTION__ << ": " << sz << std::endl;
        if( sz == 0 ){
            std::cout << "zero " << std::endl;
            nparsed = sz;
            this->appendEOF();
        }else{
            nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
        }
        bool ee = this->isError();

        if( ee ){
            std::cout << "ResponseReader::" << __FUNCTION__ << " parse error " << std::endl;
            handleParseError();
            return;
        }
        
        if( ! isFinishedHeaders() ){
            // finish the headers - keep reading
            startRead();
        }else if( isFinishedHeaders() && ! saved_EOH ){
            // headers are finishing with this block of read data
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
                // we have SOME of the the body but NOT ALL. Cache the body data
                // and prepare for readBodyData calls
                int ofset = (int)mb.size() - (int)_cachedBodyDataLength;
                char* bdy = ((char*)mb.data()) + ofset;
                std::cout << "not EOM but did get some body data" << std::endl;
            } else {
                std::cout << "not EOM and no body data" << std::endl;
                // we dont have any of the body data - maybe there is NONE ??
                // @TODO - need some experiments to see if parser takes content-length header
                // into account when signalling EOM
            }
            _io.post(pf);
        }else if( ! saved_EOM ){
            // now doing the body OR maybe should not be here
            std::cout << "NOT EOM" << std::endl;
            Error er = ( isFinishedMessage() )? Error::eom() : Error::ok();
            //
            // There is an issue here - need to know when the body cb is finished so can start the next read
            // Another issue - we are passing the chunked data to the call back
            //
            auto pf = std::bind(_bodyCb, er, bytes_transfered);
            _io.post(pf);

        }else{
            std::cout << __FUNCTION__ << std::endl;
        }
        std::cout << "ResponseReader::" << __FUNCTION__ << std::endl;

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
    MBuffer*        bodyBuffer;
    
    // These 3 properties handle the situation where body data arrives while still processing the headers
    bool            _readBodyStarted;
    void*           _cachedBodyData;
    std::size_t     _cachedBodyDataLength;
    
    // These 2 properties provide access to the de-chunked data that is provided by the c parser
    void*           _dechunkedData;
    std::size_t     _dechunkedDataLength;
    
    // These two are used for buffering body data
    MBuffer*            _bodyMBuffer;
    FBufferUniquePtr    _bodyFBufferUniquePtr;
    
    BodyEndCbType           _bodyEndCb;
    BodyCbType              _bodyCb;
    AsyncReadBodyCallback   _bodyCallback;
    ChunkCbType             _chunkCb;
    HeadersCbType           _headersCb;
    ResponseCbType          _responseCb;
};

#endif