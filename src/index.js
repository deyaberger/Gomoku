import "./accueil.css";
// import "./username.css";
import "./computer.html";
import "./humans.html";


document.querySelector(".computer > button").addEventListener("click", function() {
	location.replace("./computer.html");
  });

document.querySelector(".humans > button").addEventListener("click", function() {
	location.replace("./humans.html");
	});
