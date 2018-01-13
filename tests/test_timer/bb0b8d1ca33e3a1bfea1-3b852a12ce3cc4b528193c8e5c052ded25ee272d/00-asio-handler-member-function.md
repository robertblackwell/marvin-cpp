
# Using member function as boost::asio handler

This example demonstrates how to implement `boost::asio` handler
callback as a member functions in a safe way, even while the ASIO
timer or socket object is itself also contained in the same object.

## Problem

When deleting the object containing both the handler method and ASIO
timer (or socket object), one last call is triggered by ASIO to the
handler.  For example in case of timer, the handler will be called
with error code `Operation cancelled`.  However at this point of time
the object has already been destroyed, leading to access of memory
that was already freed.

## Solution

The example uses `shared_from_this()`, together with `weak_ptr` to
guard the call to the handler method.  Non-member function is
registered as a "proxy" handler, that will first check the `weak_ptr`
status, if it is expired (i.e. the referred object has been deleted)
the call to actual handler function is skipped.

## Example code

The first example `01-asio-timer-with-weak-ptr.cpp` implements handler callback
guard as lambda function inside arm() method.

The second example `02-asio-timer-with-weak-ptr.cpp` implements handler callback
guard in a more generic way as weak_callback() template function.
