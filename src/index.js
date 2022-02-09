import "./accueil.scss";
import Ws from "./Communicate.js";

var ws = new Ws();
ws.new_connection();


ws.onOpen = function() {
	document.querySelectorAll(".btn").forEach(
		button => {
			button.classList.remove("off");
			button.classList.add("on");
		}
	);
	document.querySelector(".computer .btn").onclick = () => {
		location.href = "computer.html";
	};
	document.querySelector(".humans .btn").onclick = () => {
		location.href = "humans.html";
	};
}

ws.onError = function() {
	setTimeout(function(){
		alert('Hi sweetheart, you are probably reading this alert box and have no clue why the heck you are even reading it, well guess what, the moon is actually just a big rock and we are all going to be sucked up by a black hole at some point. But maybe start the f**king server and refresh the page', function(){
		});
	}, 500);
}

window.alert = function(msg, callback){
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
	currentCallback();
	});





var currentCallback;

// override default browser alert


