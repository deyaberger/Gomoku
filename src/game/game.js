import "./game.scss";
import GameView from "./GameView.js";
import Ws from "../Communicate.js";


let form = JSON.parse(sessionStorage.getItem("form"));
let ws = new Ws();

let gameview = new GameView(document.getElementById("app"), form);

ws.new_connection(gameview);
ws.onOpen = function() {
	ws.co.send(form.data);
}

gameview.onInterClick = function(i, j) {
	gameview.makeMove(i, j);
	ws.send_move(i, j);
}

gameview.onRestartClick = function() {
	gameview.restart();
	ws.restart(gameview);
}

