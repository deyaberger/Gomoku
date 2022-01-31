import "./game.scss";
import GameView from "./GameView.js";
import Ws from "../Communicate.js";


var form = JSON.parse(sessionStorage.getItem("form"));

let ws = new Ws();

let gameview = new GameView(form);


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

gameview.onSliderClick = function() {
	gameview.suggestions_off = document.querySelector("input").checked;
}


var modal = document.getElementById("myModal");
var span = document.getElementsByClassName("close")[0];
var caption = document.getElementById("caption");


gameview.onAboutClick = function() {
	var modalImg = document.getElementById("img01");
	modal.style.display = "block";
	  modalImg.src = "img/about_game.pdf#toolbar=0&zoom=120";
}

span.onclick = function() { 
	var modalImg = document.getElementById("img01");
	modal.style.display = "none";
	modalImg.remove();
	modalImg = document.createElement("embed");
	modalImg.setAttribute('class', "modal-content");
	modalImg.setAttribute('id', "img01");
	modalImg.setAttribute('height', "100%");
	modalImg.setAttribute('frameBorder', "0");
	modal.insertBefore(modalImg, caption);
}
