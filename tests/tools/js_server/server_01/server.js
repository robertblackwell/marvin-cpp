var http = require('http');
var router = require('./router');

router.register('/', function(req, res) {
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Hello World');
	res.end()
});

router.register('/chunked_error', function(req, resp) {
	console.log("handler: /chunked")
	resp.setHeader('Content-Type', 'text/html; charset=UTF-8');
    resp.setHeader('Transfer-Encoding', 'chunked');

    let body = [
    	"abcedf",
    	"ghijklm",
    	"nopqrstuvw",
    	"xyz012",
    	"34567890ABC",
    	"DEF",
    	"GH",
    	"IJKLMNOPQRST",
    	"UVWXYZ"
    ]
    let count = 0;
    let timer = setInterval(() => {
    	if (count == body.length) {
    		resp.end()
    		clearInterval(timer)
    	} else if (count == 3) {

    		req.socket.destroy()

    	} else {
    		resp.write(body[count])
    		count += 1
    	}
    }, 200)

})

router.register('/chunked', function(req, resp) {
	console.log("handler: /chunked")
	resp.setHeader('Content-Type', 'text/html; charset=UTF-8');
    resp.setHeader('Transfer-Encoding', 'chunked');

    let body = [
    	"abcedf",
    	"ghijklm",
    	"nopqrstuvw",
    	"xyz012",
    	"34567890ABC",
    	"DEF",
    	"GH",
    	"IJKLMNOPQRST",
    	"UVWXYZ"
    ]
    let count = 0;
    let timer = setInterval(() => {
    	if (count == body.length) {
    		resp.end()
    		clearInterval(timer)
    	} else {
    		resp.write(body[count])
    		count += 1
    	}
    }, 200)

})

router.register('/echo/smart', function(req, resp) {
	console.log("Handler: /echo/smart")
	let body = ""
	req.on('data', (chunk) => {
		console.log("echoplain: on data")
		body = body + chunk;
	})
	req.on("end", () => {
		// test for trailers
		console.log(["echoplain: on end", "headers: ", req.rawHeaders, "body : ", body,"trailers: ", req.rawTrailers.join()])
		let rh = req.rawHeaders.join();
		let obj = {
			"req" : {
				junk: "some junk",
				method: req.method,
				uri: req.url,
				headers: req.headers,
				headers_raw: rh,
				body : body,
				trailers: req.trailers
			}
		}
		let outBody = JSON.stringify(obj);;
		resp.writeHead(200, {'Content-Type': 'application/json'});
		resp.write(outBody);
		resp.end()
	
	})
	req.on("error", () => {

	})
})

router.register('/echo/plain', function(req, resp) {
	console.log("Handler: /echo/plain")
	let body = ""
	req.on('data', (chunk) => {
		console.log("echoplain: on data")
		body = body + chunk;
	})
	req.on("end", () => {
		// test for trailers
		console.log(["echoplain: on end", "headers: ", req.rawHeaders, "body : ", body,"trailers: ", req.rawTrailers.join()])
		let rh = req.rawHeaders.join();
		let outBody = rh + body;
		resp.writeHead(200, {'Content-Type': 'text/plain'});
		resp.write(outBody);
		resp.end()
	
	})
	req.on("error", () => {

	})
	
})
router.register('/ping', function(req, resp) {
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Hello World this is a ping');
	res.end()
})

// We need a server which relies on our router
var server = http.createServer(function (req, res) {
	console.log("Server::Request received")
	handler = router.route(req);
	handler.process(req, res);
});

// Start it up
server.listen(3000);
console.log('Server running');
