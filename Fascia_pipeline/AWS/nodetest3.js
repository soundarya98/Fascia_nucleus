const net = require('net');
var http = require('http');
var globdata = "first message";
var fs = require('fs');
var ejs = require('ejs');
var bodyParser = require('body-parser');
var express = require('express');
var app = express();

var count = 0;
var globinformation = {yaxis1:0, yaxis2:0, yaxis3:0, yaxis4:0};
var datanumber = 0;

app.set('view engine', 'ejs');

app.get('/', (req, res)=>{
  res.render('graph',{globinformation:globinformation});
});

var server = app.listen(8080, function() {
  console.log('listening to port 8080');
});

const client = net.createConnection({port: 14564 }, () => {
  console.log('CLIENT: I connected to the server.');
  client.write('CLIENT: Hello this is the client!');
});


client.on('data', (data) => {
  console.log(data.toString());
  globdata = data;
  if(data == "Wake")
  {
    datanumber = 1;
  }
  if(count==0)
  {
    globinformation.yaxis1 = datanumber;
  }
  if(count==1)
  {
    globinformation.yaxis2 = datanumber;
  }
  count = count + 1;
});

client.on('end', () => {
  console.log('CLIENT: I disconnected from the server.');
});
