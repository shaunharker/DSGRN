var net = require('net');

// parse "80" and "localhost:80" or even "42mEANINg-life.com:80"
var addrRegex = /^(([a-zA-Z\-\.0-9]+):)?(\d+)$/;

var addr = {
    from: addrRegex.exec(process.argv[2]),
    to: addrRegex.exec(process.argv[3])
};

if (!addr.from || !addr.to) {
    console.log('Usage: <from> <to>');
    return;
}

net.createServer(function(from) {
	var to = net.createConnection({
		host: addr.to[2],
		port: addr.to[3]
	    });
	from.pipe(to);
	to.pipe(from);
    }).listen(addr.from[3], addr.from[2]);
