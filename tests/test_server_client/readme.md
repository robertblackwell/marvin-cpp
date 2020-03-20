#Testing in this folder

There are two sets of tests in this folder, named respectively __bb__
and __bf__.

##bf - body format

Tests the correct writing and reading of http messages using the three forms of body buffer, namely:

-   body provided as std::string
-   body provided as an MBuffer
-   body provided as a BufferChain[ ]

For this testing the actual MessageReader and MessageWriter classes are used together with the actual implementation of the Connection, HTTPServer and Client classes.

A custom test implementation of RequestHandler (tsc_rqeuest_handler.cpp/hpp) is provided to the server.

This test covers a lot of the functionality of the server, client and message transmission and reception.

Should be extended to test the management of connection limit within the
server.

##bb - body buffering

Tests the correct reading of http messages regardless of how those messages are allocated to actual buffers and TCP frames. 
As an added extra this test also verifies that messages are successfully `round-tripped` even when close/shutdown is used to indicate end-of-message rather than `content-length` or `chunked-encoding`.

To achieve this a test client object is required. This is a relatively
rudimentary client implementation and is only suitable for testing the correct operation of a single
`round-trip`.
