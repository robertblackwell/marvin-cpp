	template< typename log_policy >
		template< severity_type severity , typename...Args >
	void logger< log_policy >::print( Args...args )
	{
		write_mutex.lock();
		switch( severity )
		{
			case severity_type::debug:
				log_stream<<"<DEBUG> :";
				break;
			case severity_type::warning:
				log_stream<<"<WARNING> :";
				break;
			case severity_type::error:
				log_stream<<"<ERROR> :";
				break;
		};
		print_impl( args... );
		write_mutex.unlock();
	}

