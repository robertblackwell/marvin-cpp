//
// This example implements handler callback guard as lambda function inside arm() method.
//
// compile with
//   g++ -std=c++11 -g -Wall 01-asio-timer-with-weak-ptr.cpp -lboost_system -o 01-asio-timer-with-weak-ptr
//   clang++ -std=c++11 -g -Wall 01-asio-timer-with-weak-ptr.cpp -lboost_system -o 01-asio-timer-with-weak-ptr
//
// run:
//   ./01-asio-timer-with-weak-ptr
//   valgrind --leak-check=full --show-leak-kinds=all ./01-asio-timer-with-weak-ptr
//
// output:
//   my_timer_task::my_timer_task()
//   my_timer_task::arm()
//   running lambda
//   my_timer_task::arm()
//   my_timer_task::timer_expired_callback() 1
//   running lambda
//   my_timer_task::arm()
//   my_timer_task::timer_expired_callback() 2
//   DELETING TIMER OBJECT
//   my_timer_task::~my_timer_task()
//   running lambda
//   timer ec: Operation canceled
//

#include <iostream>
#include <functional>
#include <memory>
#include <termios.h>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include "timer.hpp"



enum KEY_ACTION{
    KEY_NULL = 0,       /* NULL */
    CTRL_C = 3,         /* Ctrl-c */
    CTRL_D = 4,         /* Ctrl-d */
    CTRL_F = 6,         /* Ctrl-f */
    CTRL_H = 8,         /* Ctrl-h */
    TAB = 9,            /* Tab */
    CTRL_L = 12,        /* Ctrl+l */
    ENTER = 13,         /* Enter */
    CTRL_Q = 17,        /* Ctrl-q */
    CTRL_S = 19,        /* Ctrl-s */
    CTRL_U = 21,        /* Ctrl-u */
    ESC = 27,           /* Escape */
    BACKSPACE =  127,   /* Backspace */
    /* The following are just soft codes, not really reported by the
     * terminal directly. */
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};
static struct termios orig_termios;
void disableRawMode(int fd) {
    /* Don't even check the return value as it's too late. */
        tcsetattr(fd,TCSAFLUSH,&orig_termios);
}


/* Raw mode: 1960 magic shit. */
int enableRawMode(int fd) {
    struct termios raw;

//    if (E.rawmode) return 0; /* Already enabled. */
    if (!isatty(STDIN_FILENO)) goto fatal;
//    atexit(editorAtExit);
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;

    raw = orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer. */
    raw.c_cc[VMIN] = 0; /* Return each byte, or zero for timeout. */
    raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
//    E.rawmode = 1;
    return 0;

    fatal:
    errno = ENOTTY;
    return -1;
}
/* Read a key from the terminal put in raw mode, trying to handle
 * escape sequences. */
int editorReadKey(int fd) {
    int nread;
    char c, seq[3];
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);

    while(1) {
        switch(c) {
            case ESC:    /* escape sequence */
                /* If this is just an ESC, we'll timeout here. */
                if (read(fd,seq,1) == 0) return ESC;
                if (read(fd,seq+1,1) == 0) return ESC;

                /* ESC [ sequences. */
                if (seq[0] == '[') {
                    if (seq[1] >= '0' && seq[1] <= '9') {
                        /* Extended escape, read additional byte. */
                        if (read(fd,seq+2,1) == 0) return ESC;
                        if (seq[2] == '~') {
                            switch(seq[1]) {
                                case '3': return DEL_KEY;
                                case '5': return PAGE_UP;
                                case '6': return PAGE_DOWN;
                            }
                        }
                    } else {
                        switch(seq[1]) {
                            case 'A': return ARROW_UP;
                            case 'B': return ARROW_DOWN;
                            case 'C': return ARROW_RIGHT;
                            case 'D': return ARROW_LEFT;
                            case 'H': return HOME_KEY;
                            case 'F': return END_KEY;
                        }
                    }
                }

                    /* ESC O sequences. */
                else if (seq[0] == 'O') {
                    switch(seq[1]) {
                        case 'H': return HOME_KEY;
                        case 'F': return END_KEY;
                    }
                }
                break;
            default:
                return c;
        }
    }
}
struct keyboard_reader
{
    boost::asio::io_context& io_ctx;
    int dup_std_in;
    boost::asio::posix::stream_descriptor input;
    keyboard_reader(boost::asio::io_context& ctx): io_ctx(ctx), input(io_ctx,dup(STDIN_FILENO))
    {
        enableRawMode(dup_std_in);
        enableRawMode(STDIN_FILENO);
    }
    int rdkb2()
    {
        return editorReadKey(dup_std_in);
    }
    void rdkb()
    {
        void* buf = malloc(1000);
        auto b = boost::asio::buffer(buf, 0);
        input.async_wait(boost::asio::posix::stream_descriptor::wait_type::wait_read, [this, buf](const boost::system::error_code& err)
        {
            char ch;
            int nread = read(STDIN_FILENO, &ch, 1);
            std::cout << "aftre read nread: " << nread << " ch: " << ch << std::endl;
            rdkb();
        });
    }
};

int
main(int argc, char *argv[])
{
    boost::asio::io_service io_service;
    MyTimer timer(io_service, 2);
    timer.arm([]()
    {
//        std::cout << "first handler" << std::endl;
    });
    auto dup_std_in = dup(STDIN_FILENO);

#if 0
    enableRawMode(STDIN_FILENO);
    while (1) {
        auto ch = editorReadKey(STDIN_FILENO);
        std::cout << ch << std::endl;
    }
#else
    keyboard_reader k(io_service);
    k.rdkb();

#endif
    io_service.run();
    return 0;

}

