import "./accueil.css";
import Ws from "./Communicate.js";

console.log("index");

var ws = new Ws();
ws.new_connection();


ws.onOpen = function() {
	console.log("Opened");
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

sessionStorage.setItem("websocket", JSON.stringify(ws));


// document.querySelector(".computer > button").addEventListener("click", function() {
// 	var form = new Form(true);
// 	console.log("form.gpu = ", form.gpu);
// });

// document.querySelector(".humans > button").addEventListener("click", function() {
// 	var form = new Form(false);
// 	console.log("form.gpu = ", form.gpu);
// });
