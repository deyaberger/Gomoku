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