import "./form.css";

console.log("form");

// var ws = sessionStorage.getItem("websocket");
// ws = JSON.parse(ws);
// console.log(ws.url);


export default class Form {
	constructor() {
		this.gpu = true;
		this.player1 = "black";
		this.player2 = "white";
	}
}

let form = new Form();

var click_start = document.getElementsByTagName("a")[0];
var html_form = document.getElementById('my_form');
console.log(html_form);

if (click_start){
	click_start.addEventListener("click", function () {start()});
}

if (html_form){
	function handleForm(event) {
		event.preventDefault();
		start();
		location.href = "./game.html";
	} 
	html_form.addEventListener('submit', handleForm);
}


function start() {
	var input_list = document.querySelectorAll(".user-box > input");
	if (input_list && input_list[0].value != "")
	{
		form.player1 = input_list[0].value;
	}
	if (input_list && input_list[1] && input_list[1].value != "")
	{
		form.player2 = input_list[1].value;
		form.gpu = false;
	}
	else if (input_list && input_list[0].value != "" && !input_list[1])
	{
		form.player2 = "computer";
	}
	sessionStorage.setItem("inputs", JSON.stringify(input_list));
	sessionStorage.setItem("form", JSON.stringify(form));
}
