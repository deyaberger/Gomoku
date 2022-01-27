import "./form.css";

// Avoid Implicit submission when pressing Enter
$(document).on("keydown", "form", function(event) { 
    return event.key != "Enter";
});

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

var click_start = document.getElementsByTagName("a")[0];

if (click_start){
	click_start.addEventListener("click", function () {start()});
}

// When button start has been clicked, we keep in session storage all the inputs
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
