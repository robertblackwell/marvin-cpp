	template< typename log_policy >
	class logger
	{
		unsigned log_line_number;
		std::string get_time();
		std::string get_logline_header();
		std::stringstream log_stream;
		log_policy* policy;
		std::mutex write_mutex;
		
		//Core printing functionality
		void print_impl();
		template<typename First, typename...Rest>
		void print_impl(First parm1, Rest...parm);
	public:
		logger( const std::string& name );

		template< severity_type severity , typename...Args >
		void print( Args...args );

		~logger();
	};
