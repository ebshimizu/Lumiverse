var io = require('socket.io').listen(80);

io.sockets.on('connection', function (socket) {
  socket.on('DMX', function (data) {
  	console.log("DMX Packet received for universe " + data.universe);
    io.sockets.emit('rcvDMX', data);
  });
});