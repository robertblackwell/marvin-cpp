## So what is this ?
The __marvin-cpp__ project provides a man-in-the-middle proxy that can examine __http__ and __https__ traffic between a http(s) client and an http(s) server. It is a bit like the __Charles__  app that is well known to web developers, but currently only in cli form.

Unlike my project __marvin__ (https://github.com/robertblackwell/marvin.git  which is an earlier effort at a mitm proxy in node-js), this one is more ambitious, written in C++ using Boost/Asio and undertaking all X509 certificate manipulation in code, not using ```system(openssl ....)```.

## Status

The CORVID-19 lockdown of 2020 has given me the opportunity to invest some time in this project and as of May 2020 the basic structure of the Mitm Proxy is now in place and there is a cli app that is able to intercept and view __HTTP__ and __HTTPS__ traffic. See [./apps/simple_proxy/readme.md](./apps/simple_proxy/readme.md) for details.

## Installation

Currently there is no provision within the project for installing a marvin-cpp app into some `bin/` directory for use as a general tool.

However if the goal is to try it out and look at the code; then do the following:

```
git clone git@github.com:robertblackwell/marvin-cpp.git
cd marvin-cpp
./scripts/smpl_install install
cd cmake-build-debug
cmake ..
make -j 8
ctest
```

## Dependencies
The project depends on:

| package | comment |
|--------------|--------------------|
| boost | v1.72 asio, filesystem, optional |
| openssl | 1.1.1f |
| CLI11 | v1.9.0 cli option parsing|
| nlohmann_json | v3.7.3 |
| doctest | v2.3.7 |
| http_parser | - a private fork of github.com:nodejs/http-parser.git|
|url_parser |- clones from somewhere on github - reference required|
|cxx_url |- clones from somewhere on github - reference required|
|trog |- personal logging library on github|
|libcert |- personal library for manipulating openssl certificates|

These dependecies need to be installed within the project in directories named

```
vendor/include
vendor/lib
vendor/src
```
The `./scripts` directory contains a small suite of shell scripts that will pull down, and install the required dependencies. Thats the 3rd line in the install recipe.

There is also a small python cli app near completion that will replace these shell scripts it can be found at [https://github.com/robertblackwell/smpl](https://github.com/robertblackwell/smpl). I hope this app will make maintenance a little easier, I dont like shell script programming.

## simple_proxy cli app

From the project root directory executing
```
./bin/simple_proxy.sh
```
will start a proxy listening on port 9992 (and will __not__ return to a command prompt untill the proxy terminates).

You can test it by running some of the following shell scripts in the project __bin__ directory, from a different/second terminal.

```
./bin/curl_ssl_host.sh https://google.com
./bin/curl_ssl_geeksforgeeks.sh
./bin/curl_ssl_hackernoon.sh
./bin/curl_ssl_data.sh ./data/list_top_500.data
```
The last one will get a page from each of 500 https sites in turn. Be warned it will run for a good while.

simple_proxy can handle a __https__ request either by __tunnel__ or by __mitm__, all http requests are handled by __mitm__.

__Tunnel__ is when simple_proxy connects the client and server by back to back tcp connections and just passes anonymous data. In this mode the content of the exchange is not visible.

__Mitm__ is when simple_proxy intercepts the traffic. To achieve this, in the case of https traffic,  the app must create a X509 certificate for communicating with the client that allows `simple_proxy` to appear, to the client, to be the intended host. This certificate is signed by a local Certificate Authority embedded within the `simple_proxy` app. For this to work the client must use a root certificate bundle that includes simple_proxy's internal CA. You can observe this by looking into the various bash scripts to observe `curl --cacerts=` where curl is give a special set of root certificates. The "magic" that does this is in the __libcert__ library; the new certificate is built on the fly in code (no use of openssl cli command) by the certlib. 

Observing the traffic in __mitm__. By default simple_proxy is configured to intercept all https traffic (that is it is in mitm mode for all https urls,
and all http urls for that matter). 

The intercepted traffic is displayed on stdout, that is the terminal where simple_proxy was originally started. For demo purposes the body of the intercepted messages are not displayed only the headers. So for each exchange you will find a request header followed by a response header in the output from simple_proxy. This is enough to verify (prove) that https traffic is being intercepted as under https headers are part of the encrypted envelope..





