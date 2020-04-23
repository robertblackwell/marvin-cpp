# start the nodejs server prior to running the
# tests in this folder
echo This is a wrapper for the test_messsage_roundtrip test
echo it starts a nodejs server before running the test
this_test_dir=$(realpath ./)
project_dir=/home/robert/Projects/marvin++
node_server=${project_dir}/tests/tools/js_server/server.js
# start the server in the background
node ${node_server} > /dev/null 2 >&1  &
echo Node server should be running
# give it time to get started
sleep 2
# now run the test cases
${this_test_dir}/test_message_roundtrip

echo now shutdown the node server
#  now stop the node server
curl -s http://localhost:3000/stop
