import "./game.css";
import GameView from "./GameView.js";

console.log("game");

var ws = sessionStorage.getItem("websocket");
ws = JSON.parse(ws);
var form = sessionStorage.getItem("form");
form = JSON.parse(form);

var inputs = sessionStorage.getItem("inputs");
inputs = JSON.parse(inputs);

console.log(ws.url);
console.log(form);
console.log(inputs);

// let gameview = new GameView(document.getElementById("app"));

// let ws = new Ws();
// ws.new_connection(gameview);


// gameview.onInterClick = function(i, j) {
// 	console.log("just clicked on intersection", i , "x", j);
// 	gameview.makeMove(i, j);
// 	ws.send_move(i, j);
// }

// gameview.onRestartClick = function() {
// 	console.log("just clicked on restart");
// 	gameview.restart();
// 	ws.restart(gameview);
// }


