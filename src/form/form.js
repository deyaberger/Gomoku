import "./form.css";

// Class form that will help to comunicate with the server
export default class Form {
	constructor() {
		this.cpu = true;
		this.player1 = "black";
		this.player2 = "white";
	}

	update_starting_data() {
		this.data = JSON.stringify({
			"type"	: "start",
			"cpu"	: this.cpu,
			"depth"	: 7
		});
}
}

let form = new Form();

// Make sur we can send the form also by clicking on the save button
var click_start = document.getElementsByTagName("a")[0];

if (click_start){
	click_start.addEventListener("click", function () {start()});
}

// Make sur we can send the form also by pressing Enter
var player1 = document.getElementById("player1");
if (player1) {
player1.addEventListener("keypress", function(event) {
  if (event.key === 'Enter') {
	event.preventDefault();
	click_start.click();
  }
});
}

var player2 = document.getElementById("player2");
if (player2) {
player2.addEventListener("keypress", function(event) {
  if (event.key === 'Enter') {
	event.preventDefault();
	click_start.click();
  }
});
}

// When button or Enter has been pressed, we keep in session storage all the inputs
function start() {
	var input_list = document.querySelectorAll(".user-box > input");
	if (input_list && input_list[0].value != "")
	{
		form.player1 = input_list[0].value;
	}
	if (input_list && input_list[1])
	{
		if (input_list[1].value != "")
			form.player2 = input_list[1].value;
		form.cpu = false;
	}
	else if (input_list && input_list[0].value != "" && !input_list[1])
	{
		form.player2 = "computer";
	}
	form.update_starting_data();
	sessionStorage.setItem("form", JSON.stringify(form));
}
