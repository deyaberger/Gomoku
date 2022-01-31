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

gameview.onAboutClick = function() {

	var modal = document.getElementById("myModal");

	// Get the image and insert it inside the modal - use its "alt" text as a caption
	var modalImg = document.getElementById("img01");
	var captionText = document.getElementById("caption");
	  modal.style.display = "block";
	  modalImg.src = "img/test2.pdf#toolbar=0&zoom=120";

	
	// Get the <span> element that closes the modal
	var span = document.getElementsByClassName("close")[0];
	
	// When the user clicks on <span> (x), close the modal
	span.onclick = function() { 
	  modal.style.display = "none";
	}
}

