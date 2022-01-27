import "./game.css";
import GameView from "./GameView.js";
import Ws from "../Communicate.js";


console.log("game");

let form = JSON.parse(sessionStorage.getItem("form"));
let ws = new Ws();

let gameview = new GameView(document.getElementById("app"), form);

ws.new_connection(gameview);
ws.onOpen = function() {
	ws.co.send(form.data);
}

console.log(gameview.cpu);


gameview.onInterClick = function(i, j) {
	console.log("just clicked on intersection", i , "x", j);
	gameview.makeMove(i, j);
	ws.send_move(i, j);
}

gameview.onRestartClick = function() {
	console.log("just clicked on restart");
	gameview.restart();
	ws.restart(gameview);
}


