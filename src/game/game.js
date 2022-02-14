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
	gameview.infos.on_off_suggestions(gameview.suggestions_off, gameview.board);
}


var modal = document.getElementById("myModal");
var span = document.getElementsByClassName("close")[0];
var caption = document.getElementById("caption");


gameview.onAboutClick = function() {
	var modalImg = document.getElementById("img01");
	modal.style.display = "block";
	  modalImg.src = "./img/about_gomoku.png";
}

span.onclick = function() { 
	var modalImg = document.getElementById("img01");
	modal.style.display = "none";
	modalImg.remove();
	modalImg = document.createElement("img");
	modalImg.setAttribute('class', "modal-content");
	modalImg.setAttribute('id', "img01");
	modalImg.setAttribute('height', "100%");
	modal.insertBefore(modalImg, caption);
}


ws.onError = function() {
	setTimeout(function(){
		alert("It seems like the server is not running, so maybe just start it and reload the page. If it still doesn't work, well... too bad for you", function(){
		});
	}, 500);
}


window.alert = function(msg, callback){
	$('.popup').css({
		'visibility' : 'visible',
		'opacity' : '1',
	})
	$('.message').text(msg);
	$('.customAlert').css('animation', 'fadeIn 0.3s linear');
	$('.customAlert').css('display', 'inline');
	setTimeout(function(){
	  $('.customAlert').css('animation', 'none');
	}, 300);
	currentCallback = callback;
  }

$('.confirmButton').click(function(){
	$('.customAlert').css('animation', 'fadeOut 0.3s linear');
	setTimeout(function(){
	$('.customAlert').css('animation', 'none');
		$('.customAlert').css('display', 'none');
	}, 300);
	$('.popup').css({
		'visibility' : 'hiddden',
		'opacity' : '0',
	})
	currentCallback();
	});


var currentCallback;

