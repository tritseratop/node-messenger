'use strict';

let readline = require('readline');
let readlineSync = require('readline-sync');
const { Client } = require('bindings')('client');
const { WSClient } = require('bindings')('wsclient');

/*let input = function () {
    let res = "";
    res = syncreadline.prompt({ prompt: '>' });
    return res;
};*/

console.log("Enter 0 to run TCP-client");
console.log("Enter 1 to run WS-client");

let command = readlineSync.prompt();
//let command = 0;
if (command == 0) {
    console.log("You entered 0 to run TCP-client");
    const client = new Client();
    const output = (msg) => {
        console.log(msg);
    };

    if (client.Initialize() == false) {
        console.log("cant initialize");
    }
    else if (client.Create() == false) {
        console.log("cant create");
    }
    else if (client.Connect() == false) {
        console.log("cant connect");
    }
    else {
        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });
        rl.prompt();
        rl.on('line', (msg) => {
            //console.log(msg);
            client.Send(msg);
        });
        client.StartChating(output);
    }
}
else {
    console.log("You entered 1 to run WS-client");
    const wsclient = new WSClient();

    const output = (msg) => {
        console.log(msg);
    }
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    rl.prompt();
    rl.on('line', (msg) => {
        //console.log(msg);
        wsclient.Send(msg);
    });

    wsclient.run(output);
}
