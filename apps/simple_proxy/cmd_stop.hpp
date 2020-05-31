#ifndef marvin_simple_proxy_cmd_stop_hpp
#define marvin_simple_proxy_cmd_stop_hpp

using ControlRequest = std::string;
using ParsedControlRequest = std::vector<std::string>;
struct ControlResponse {
    int                         return_code;
    std::vector<std::string>    output;
// using ControlResponse = std::vector<std::string>;
};

class CmdInterface
{
    virtual ControlRequest make_request() = 0;
    virtual void exec(std::string method, ParsedControlRequest req) = 0;
};

class CmdStop
{
    CmdStop(CLI::App* subcmd, bool stop_immediate)
    {

    }
    ControlRequest make_request()
    {
        return "/stop";
    }
    void operator()()
    {

    }
    void exec(MitmThread& mitm_thread, std::string method, std::vector<std::string>& bits)
    {
        std::string body = "stop not yet implemented";
        MessageBaseSPtr response_msg = make_200_response(body);
        auto s = response_msg->to_string();
        m_wrtr->async_write(response_msg, body, [this](ErrorType& err)
        {
            if (err) {
                p_on_write_error(err);
            } else {
                m_mitm_thread_ref.terminate();
                m_ctl_thread_ptr->terminate();
                p_req_resp_cycle_complete();
            }
        });
    }


};
