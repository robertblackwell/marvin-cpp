	template< typename log_policy >
	logger< log_policy >::logger( const std::string& name )
	{
		log_line_number = 0;
		policy = new log_policy;
		if( !policy )
		{
			throw std::runtime_error("LOGGER: Unable to create the logger instance"); 
		}
		policy->open_ostream( name );
	}

	template< typename log_policy >
	logger< log_policy >::~logger()
	{
		if( policy )
		{
			policy->close_ostream();
			delete policy;
		}
	}
