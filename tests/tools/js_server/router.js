var handlerFactory = require('./handler');
var fs = require('fs');
var parser = require('url');
var handlers = {};

exports.clear = function() {
  handlers = {};
}

exports.register = function(url, method) {
  console.log("router.register url " + url + " method " + method.name)
  if(!method) {
    console.log("Method not provided")
  }
  handlers[url] = handlerFactory.createHandler(method);
}

exports.route = function(req) {
  console.log("route: url = " + req.url + " url.path = " + req.url.path)
  url = parser.parse(req.url, true);
  var handler = handlers[url.pathname];
  if (!handler) {
    handler = this.missing(req)
  }
  return handler;
}

exports.missing = function(req) {
  // Try to read the file locally, this is a security hole, yo /../../etc/passwd
  var url = parser.parse(req.url, true);
  return handlerFactory.createHandler(function(req, res) {
    res.writeHead(404, {'Content-Type': 'text/plain'});
    res.write("No route registered for " + url.pathname);
    res.end();
  });      
}


