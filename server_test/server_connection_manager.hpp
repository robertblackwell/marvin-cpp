//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include "server_connection.hpp"

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ServerConnectionManager
{
public:
  ServerConnectionManager(const ServerConnectionManager&) = delete;
  ServerConnectionManager& operator=(const ServerConnectionManager&) = delete;

  /// Construct a connection manager.
  ServerConnectionManager();

  /// Add the specified connection to the manager and start it.
  void start(connection_ptr c);

  /// Stop the specified connection.
  void stop(connection_ptr c);

  /// Stop all connections.
  void stop_all();

private:
  /// The managed connections.
  std::set<connection_ptr> connections_;
};

#endif // HTTP_SERVER_CONNECTION_MANAGER_HPP
