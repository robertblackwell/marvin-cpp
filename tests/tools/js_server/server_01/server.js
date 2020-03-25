var http = require('http');
var router = require('./router');

// Handle your routes here, put static pages in ./public and they will server
router.register('/', function(req, res) {
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Hello World');
	res.end()
});
router.register('/echo/smart', function(req, resp) {
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Hello World');
	res.end()

})
router.register('/echo/plain', function(req, resp) {
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Hello World');
	res.end()
	
})
router.register('/ping', function(req, resp) {
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Hello World this is a ping');
	res.end()
})

// We need a server which relies on our router
var server = http.createServer(function (req, res) {
	console.log("Request received")
	handler = router.route(req);
	handler.process(req, res);
});

// Start it up
server.listen(3000);
console.log('Server running');
