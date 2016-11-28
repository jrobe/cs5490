// thrift -gen js:node ../thrift/cs5490.thrift
// npm install
// node index.js

var thrift = require('thrift');
var AuthServe = require('./gen-nodejs/AuthServe');
var ttypes = require('./gen-nodejs/cs5490_types');

var fs = require("fs");


var connection = thrift.createHttpConnection("127.0.0.1", 9090, {
    transport: thrift.TBufferedTransport(),
    protocol: thrift.TBinaryProtocol(),
    https: true,
    headers: { "Connection": "close" },
    nodeOptions: {
        ca: fs.readFileSync("../ssl/server.crt"),
        checkServerIdentity: function (host, cert) {
            // Override hostname check cause we don't have a hostname
            return undefined;
        }
    }
});


connection.on('error', function (err) {
    console.log("Connection Error:" + err);
});


var client = thrift.createHttpClient(AuthServe, connection);

client.createAccount("testing", {email: "testing@test.com"}, function(error, result) {
    console.log("callback");
});