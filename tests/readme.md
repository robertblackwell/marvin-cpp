#Tests

| Test             |               Description                                |
|------------------|----------------------------------------------------------|
|`test_buffers`| Standalone unit tests for classes Marvin::MBuffer,Marvin::BufferChain. There is no asyn or io requirement for these tests|
|`test_error`|unit tests of custom marvin error |
|`test_headers`| standalone unit tests for Marvin::Http::Headers and related free functions|
|`test_message`|unit tests for class MessageBase and Parser|
|`test_message_reader|unit tests for MessageReader and MessageWriter requires a mock of Socket class|
`test_roundtrip`|tests Client, MessageReader, MessageWtiter against a node http_server in tests/tools/js_server/server_01.|
|`test_uri`|unit test of Uri() class
|`test_server_v3`|runs a marvin/server_v3 on one thread and on another runs successive instances of Client to test message formating, parsing and error handling|


