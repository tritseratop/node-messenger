'use strict';

let readline = require('readline');
const { Client } = require('bindings')('client');

const client = new Client();

const output = (msg) => {
    console.log(msg);
};

client.Initialize();
client.Create();
client.Connect();

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});
rl.prompt();
rl.on('line', (msg) => {
    //console.log(msg);
    client.Send(msg);
});


if (client.StartChating(output) == false) {
    console.log("The end");
}