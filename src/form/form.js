import "./form.css";

console.log("form");

var ws = sessionStorage.getItem("websocket");
console.log(ws);
ws = JSON.parse(ws);
console.log(ws.url);

// document.getElementsByTagName("a").addEventListener("click", function() {
// 	console.log("form.gpu = ", form.gpu);
// });



// var input_list = document.querySelectorAll(".user-box > input");
// console.log(input_list);

// var gpu = false;

// if (input_list)
// {
// 	input_list[0].addEventListener("click", function() {
// 		// 	var form = new Form(false);
// 		// 	console.log("form.gpu = ", form.gpu);
// 		});
// }

if (document.title == 'Computer')
{
	
	console.log("You want to play against the computer");
}
else if (document.title == 'Humans')
{
	console.log("You want to play against a human");
}

