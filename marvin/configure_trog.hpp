#ifndef marvin_configure_trog_hpp
#define marvin_configure_trog_hpp

///
/// To configure a logger for you application should #include a custom version of this header
/// at the very top of every compilation unit.
///
/// This file selects the Formatter, Sink and Worker that you require for you application.
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
    /// Thus SinkIsAFile is a fully specialized class.
    ///
    typedef SinkFileT<SinkFileName> SinkIsAFile;

    /// Sinkstdout and Sinkstderr are fullly specialized classes that write to STDOUT and STDERR respoectively
    typedef Sinkstdout SinkIsStdout;

    /// Sink2 is a template class that combines 2 sinks into a single sink that writes to both places.
    /// the following typedef results in a fully instantiated class SinkIsAFileAndStdout
    /// will write to both a file and stdout.
    ///
    /// SinkDefault is the same as SinkIsAFileAndStdout
    ///
    typedef Sink2<Sinkstdout, SinkIsAFile> SinkIsAFileAndStdout;
    ///

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
    ///  Selecting the desired Writer - pass either Simple::Writer or Thread::Writer as the last argument to the
    ///  typedef/using below. 
    ///
    ///

    /// Formatter
    /// =========
    ///
    /// Currently there is only one Formatter class. However since the Formatter class is an argument to the 
    /// Trogger/Collector template if a new one is developed it can easily be installed by replacing the
    /// first argument the above using/typedef

    /// The logger logic is implemented in a class called Trog::Logger, the construction of which provides the customization
    /// point for selecting Formatters, Sinks and Worker.
    ///
    /// In order to gracefully handle exceptions (in some circumstances) we must make a "logging" class that is a singleton and whose
    /// single instance is created as load time before main() starts to execute. That way it will stay around after main() exists to
    /// clean up any un written log records. Trogger defined below is that class.

    ///
    /// Trogger : to change the choice of Sink and Worker change the details of this class.
    ///
    class Trogger : public Logger
    {
        Formatter       m_fmter;
        SinkIsStdout    m_sink;
        Simple::Writer  m_writer;// = new ::Trog::Simple::Writer(fmter, sink);
    public:
        static Trogger& get_instance()
        {
            static Trogger instance;
            return instance;
        }
        Trogger(): m_writer(Simple::Writer(&m_fmter, &m_sink)), Logger(&m_writer, &m_fmter)
        {
            std::cout << "hello" << std::endl;
        }
    };
}
#endif
