import {from_reverse_nb_to_2d, from_nb_to_2d} from "./Utils.js";

export default class GameView {
	constructor(root, form) {
		this.root = root;
		this.turn = "black";
		this.cpu = form.cpu;
		this.player1 = form.player1;
		this.player2 = form.player2;
		this.b_captures = 0;
		this.w_captures = 0;
		this.html_board = document.querySelector(".real_board");
		this.place_all_intersections();
		
		this.onInterClick = undefined;
		this.onRestartClick = undefined;

		this.root.querySelectorAll(".intersection").forEach(
			inter => {
            inter.addEventListener("click", () => {
                if (this.onInterClick) {
                    this.onInterClick(inter.getAttribute("row"), inter.getAttribute("col"));
                }
            });
        });

		this.root.querySelector(".header__restart").addEventListener("click", () => {
            if (this.onRestartClick) {
                this.onRestartClick();
            }
        });
	}

	place_intersection(row, column)
	{
		const intersection = document.createElement("div");

		intersection.style.gridRowStart = (row - 1) ;
		intersection.style.gridRowEnd = (row - 1) + 2;
		intersection.style.gridColumnStart = (column - 1) ;
		intersection.style.gridColumnEnd = (column - 1) + 2;

		intersection.classList.add("intersection");
		intersection.classList.add("void");

		intersection.setAttribute("row", row - 2);
		intersection.setAttribute("col", column - 2);
		this.html_board.appendChild(intersection);
	}

	place_all_intersections()
	{
		for (var j = 2; j <= 20; j++)
		{
			for (var i = 2; i <= 20; i++) {
				this.place_intersection(i, j)
			}
		}
	}

	place_void(row, column)
	{
		const void_intersection = document.querySelector("[row = '" + row + "'][col = '" + column + "']");
		if (void_intersection)
		{
			if (!void_intersection.classList.contains("void"))
			{
				console.log("VOID INTERSECTION FOUND at: ", row, column);
				console.log(void_intersection.classList);
			}
			void_intersection.classList.remove(...void_intersection.classList);
			void_intersection.classList.add("intersection");
			void_intersection.classList.add("void");
		}
	}

	place_illegal(row, column)
	{
		const illegal = document.querySelector("[class = 'intersection void'][row = '" + row + "'][col = '" + column + "']");

		if (illegal)
		{
			illegal.classList.remove("void");
			illegal.classList.add("illegal");
			this.html_board.appendChild(illegal);
		}
	}

	place_stone(row, column, mark)
	{
	  var stone = document.querySelector("[class = 'intersection void'][row = '" + row + "'][col = '" + column + "']");
	
		if (!stone)
		{
			var stone = document.querySelector("[class = 'intersection suggested_stone'][row = '" + row + "'][col = '" + column + "']");
			 if (!stone) {return;}
		}
		stone.classList.remove("void");
		stone.classList.remove("suggested_stone");
		stone.classList.add("stone");
		stone.classList.add(mark);

		this.html_board.appendChild(stone);
	};
	
	place_suggestion(row, column, mark)
	{
		const suggested_stone = document.querySelector("[class = 'intersection void'][row = '" + row + "'][col = '" + column + "']");

		if (suggested_stone)
		{
			suggested_stone.classList.remove("void");
			suggested_stone.classList.add("suggested_stone");
			this.html_board.appendChild(suggested_stone);
		}
	}

	remove_suggestions()
	{
		document.querySelectorAll(".intersection,.suggested_stone").forEach(
			suggestion => {
				suggestion.classList.remove("suggested_stone");
				suggestion.classList.add("void");
			}
		);
	}

	update_winner(winner)
	{
		$('.header__turn').css({
			'display': 'none',
			});

	  document.querySelector("[class = 'status']").textContent = winner + ' wins !!!';
		
	  var color = 'linear-gradient(145deg, #ffffff, #f6cfa6)';
	  if (winner == 'black')
		  {color = 'linear-gradient(145deg, #f6cfa6, #000000)';}

	  $('.header__status .forever_stone').css({
		  background: color,
		  'display' : 'inline',
		});
	  $('.status').css({
		  'margin-left': '10px',
		  });
	};

	remove_clicks()
	{
		document.querySelectorAll(".intersection").forEach(
			inter => {
				inter.classList.add("illegal");
			});
	}


	nextTurn(){
		this.turn = this.turn == "black" ? "white" : "black";
		document.querySelector("[class = 'turn']").textContent = this.turn + "s' turn";
		var color = 'linear-gradient(145deg, #f6cfa6, #000000)';
		if (this.turn == 'white') {
			color = 'linear-gradient(145deg, #ffffff, #f6cfa6)';
		}
		$(".header__turn > .forever_stone").css({
  			background: color,
		})
	}

	makeMove(i, j) {
		this.place_stone(i, j, this.turn);
		console.log("placing stone", this.turn)
	}

	update_captures(captures, mark) {
		if (mark == "white")
		{
			this.w_captures = captures;
		}
		else if (mark == "black")
		{
			this.b_captures = captures;
		}
		var capture_nb = document.querySelector("div." + mark + "_player_captures");
		console.log(capture_nb);
		capture_nb.querySelector("div.num").textContent = Number(captures) * 2;

		var color = '#4CFF33';
		var pixels = 0;
		if (captures >= 5) {
			pixels = 0;
		}
		else if(captures < 5 && captures >= 4){
			pixels = 40;
		}
		else if(captures < 4 && captures >= 3){
			pixels = 80;
		}
		else if (captures < 3 && captures >= 2){
			pixels = 120;
		}
		else if (captures < 2 && captures >= 1){
			pixels = 160;
		}
		else if (captures < 1 && captures >= 0){
			pixels = 185;
		}
	
		console.log("color", color);
		
		$('.' + mark + '_player_captures .column').css({background: color});
		
		$('.' + mark + '_player_captures .column').animate({
			height: (captures * 2) * 10 + '%',
		});
	
		$('.' + mark + '_player_captures .num').css({'padding-top': pixels + 'px'});
	
		$('.' + mark + '_player_captures .num').animate({'padding-top': pixels + 'px'});
	}

	init_captures() {
		$('.column').css({
			height: '0%',
			background: '#90a4ae',
		});
		$('.num').css({
			'padding-top': '185px',
		});
		document.querySelectorAll(".num").forEach( number => {
			number.textContent = '0';
		});
	}


	update(data) {
		console.log(data.type2);
		if (data.type2 == "player_move")
		{
			this.nextTurn()
			if (this.cpu == false)
			{
				this.remove_suggestions();
			}
			for (let index = (19 * 19) - 1; index > 0; index--)
			{
				var {i, j} = from_reverse_nb_to_2d(index);
				if (data.illegal_board[index] == "1")
				{
					this.place_illegal(i, j);
				}
				else if (data.white_board[index] == "0" && data.black_board[index] == "0")
				{
					this.place_void(i, j);
				}
			}
		}
		else if (data.type2 == "AI_move")
		{
			this.nextTurn()
			for (let index = (19 * 19) - 1; index > 0; index--)
			{
				var {i, j} = from_reverse_nb_to_2d(index);
				if (data.black_board[index] == "1" && data.white_board[index] == "0")
				{
					this.place_stone(i, j, 'black');
				}
				else if (data.white_board[index] == "1" && data.black_board[index] == "0")
				{
					this.place_stone(i, j, "white");
				}
				else if (data.white_board[index] == "0" && data.black_board[index] == "0")
				{
					this.place_void(i, j);
				}
			}
			console.log("Type: " + data.type2);
			// console.log(data.black_board)
		}
		if (data.winner == "black" || data.winner == "white")
		{
			this.update_winner(data.winner);
			this.remove_clicks();
		}
		if (data.type2 == "AI_move_suggestion")
		{
			console.log("TURN = " + this.turn);
			var {i, j} = from_nb_to_2d(data.suggested_move);
			this.place_suggestion(i, j, this.turn);

		}
		if (data.b_captures && this.b_captures != data.b_captures)
		{
			this.update_captures(data.b_captures, "black");
		}
		if (data.w_captures && this.w_captures != data.w_captures)
		{
			this.update_captures(data.w_captures, "white");
		}
	}

	restart() {
		
		var non_void = document.querySelectorAll(".intersection,.stone,.black,.white").forEach(
			inter => {
				if (!inter.classList.contains("forever_stone"))
				{
					inter.classList.remove("stone");
					inter.classList.remove("black");
					inter.classList.remove("white");
					inter.classList.remove("illegal");
					inter.classList.remove("suggested_stone");
					inter.classList.add("void");
				}
			});
		this.init_captures();
		$('.header__turn').css({
			'display': 'flex',
			});

		$('.header__status > .forever_stone').css({
			'display': 'none',
			});
		
		$('.status').css({
			'margin-left': '0px',
			});
		
		document.querySelector("[class = 'status']").textContent = '... In Progress ...';

	}
}
