const net = require('net');
var http = require('http');

const client = net.createConnection({port: 14564 }, () => {
  console.log('CLIENT: I connected to the server.');
  client.write('CLIENT: Hello this is the client!');
});

client.on('data', (data) => {
  //console.log(data.toString());
  var server = http.createServer(function(req, res){
  res.writeHead(200);
  res.write(data);
  res.end();
  });
  server.listen(8080);
  client.end();
});

client.on('end', () => {
  console.log('CLIENT: I disconnected from the server.');
});
