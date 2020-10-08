const net = require('net');
var http = require('http');
var globdata;

var server = http.createServer(function(req, res){
  res.writeHead(200);
  res.write(globdata);
  res.end();
  });
server.listen(8080);

const client = net.createConnection({port: 14564 }, () => {
  console.log('CLIENT: I connected to the server.');
  client.write('CLIENT: Hello this is the client!');
});


client.on('data', (data) => {
  console.log(data.toString());
  globdata = data;
});

client.on('end', () => {
  console.log('CLIENT: I disconnected from the server.');
});
