	template< typename log_policy >
	void logger< log_policy >::print_impl()
	{
		policy->write( get_logline_header() + log_stream.str() );
		log_stream.str("");
	}

	template< typename log_policy >
		template<typename First, typename...Rest >
	void logger< log_policy >::print_impl(First parm1, Rest...parm)
	{
		log_stream<<parm1;
		print_impl(parm...);	
	}
