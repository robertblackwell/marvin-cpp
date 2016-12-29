#Using Marvin in an Objc app
This short file explains how to integrate c++ marvin into an Objc Cocoa app.

## the pieces of the puzzle

Go to folder `src/objc` and observe the files in that folder:

-	`marvin_objc.h` and `marvin_objc.mm` are objc++ files that implement a wraper for the c++  marvin code. These files need to know about a few of the c++ classes and in addition make use of
	
	-	`marvin_delegate_objc.h` and `marvin_delegate_objc.mm`, again objc++ files that implement a skelton delegate object that can be used to get back from the marvin proxy details of each http request/response exchange that was observed.
	
	-	`objc_collector.hpp` and `objc_collector.mm` provides a C++ class that has been compiled under objc++ and provides the packaging of a request/response exchange into a form consumable by object-c and calls the marvin delegate object.

-	Finally the folder `proxy_cocoa` contains `AppDelegate.h` and `AppDelegate.mm` and objectivec++ implementation of a skeleton app delegate that will connect an app to the marvin proxy.
	
##threads 	
This is one of the complexities to be overcome because the C++ components of marvin make use of boost::asio::io_service which is a form of run_loop which is distinct from the apple run_loop and apple dispatch queues. __We need to make sure these two async technologoes do not get in each otehrs way__.

So, how do we do that:

-	`marvin_objc.h` and `marviv_objc.mm` class invoke the C++ code on a background thread (via the `run` method using `performSelectorOnBackgroundThread`) without starting the apple runloop.		
-	the marvin C++ code then creates a number of otehr threads for its own purposes.
-	the `objc_collector.h/.mm` class calls the __delegates__ `notify` method on the main thread, again using a variety of `performSelector`. __Note__, we did not use some variation of:

```
dispath_async(displatch_get_main_queue, ...)
```

as (from Apples won documentation) the main queue and the main thread are not allways the same thing.