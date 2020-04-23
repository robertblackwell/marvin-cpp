#ifndef marvin_configure_trog_hpp
#define marvin_configure_trog_hpp
///
/// To configure a logger that is different from the default this file should be included 
/// into every file that uses Trog in place of Trog.hpp.
///
///  The Trog logger is typedef/using that renames an instantiation of a templates class.
///
///  To configure a different logger that typedef/using must be pointed at a different instantiation
///  of the template class .. like below.
///  
///    using Trogger = Collector<Formatter, SinkDefault, Simple::Writer>;
///   
///

#include <trog/trog.hpp>

namespace Trog {

    /// Sink
    /// ====
    /// A sink is a class that represents where the log is written stdout, stderr or a file
    ///
    /// SinkFileT is a template that writes to a file but relies on a template parameter
    /// to tell it the name of the file. SinkFileName is the only such "naming" class Trog provides
    /// and it writes to a file caller trog-yyyy-mm-dd_hh-mm-ss.log in the current dirctory.
    ///
    /// Thus SinkIsAFile is a fully instantiated class that writes logs to the file described above.
    ///
    typedef SinkFileT<SinkFileName> SinkIsAFile;

    /// Sinkstdout and Sinkstderr are full classes that write to STDOUT and STDERR respoectively
    typedef Sinkstdout SinkIsStdout;

    /// Sink2 is a template class that combines 2 sinks into a single sink that writes to both places.
    /// the following typedef results in a fully instantiated class SinkIsAFileAndStdout
    /// will write to both a file and stdout.
    ///
    /// SinkDefault is the same as SinkIsAFileAndStdout
    ///
    typedef Sink2<Sinkstdout, SinkIsAFile> SinkIsAFileAndStdout;
    ///
    /// If I was more expert in templates I would provide a family of templates Sink1, Sink2, Sink3 .... SinkN
    
    /// 
    /// Writer
    /// ======
    /// A writer is a class that passed data from the TROG_XXXXX macros and arranges to pass it to 
    /// the active sink.
    ///
    /// There are currently two writes:
    ///
    ///     Trog::Simple::Writer -  it runs on the same thread as the TROG_XXXX macro that generated the log messsage.
    ///                             Since the application using Trog may be multi-threaded. This writer may have to operate
    ///                             on multiple threads. It uses a lock (mutex/condition variable) 
    ///                             to ensure that logs generated on different threads do not conflict.
    ///
    ///     Trog::Thread::Writer    is a writer that runs on a dedicated "background" thread. The TROG_XXXX macros call functions
    ///                             that pass log data from the TROG_XXXX macros via a queue to the bcakground thread. The background thread
    ///                             completes the formatting of the output message and writes to te sink.
    ///
    ///  Selecting the desireed Writer - pass either Simple::Writer or Thread::Writer as the last argument to the
    ///  typedef/using below. 
    ///
    ///    using Trogger = Collector<Formatter, SinkDefault, Thread::Writer>;
    ///

    using Trogger = Collector<Formatter, Sinkstdout, Simple::Writer>;

    /// Formatter
    /// =========
    ///
    /// Currently there is only one Formatter class. However since the Formatter class is an argument to the 
    /// Trogger/Collector template if a new one is developed it can easily be installed by replacing the
    /// first argument the above using/typedef
}

#endif