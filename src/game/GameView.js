import {from_reverse_nb_to_2d, from_nb_to_2d} from "./Utils.js";

class Infos {
	constructor(player1, player2) {

		this.turn = "black";
		this.player1 = player1;
		this.player2 = player2;
		this.update_turn_text(this.turn);
		this.b_captures = 0;
		this.w_captures = 0;
	}

	update_turn_text(turn){
		this.turn_text = this.turn + "s' turn";
		if (turn == null)
		{
			this.turn_text = "Waiting for suggestion...";
		}
		else if (turn == "black" && this.player1 != turn)
		{
			this.turn_text = this.player1 + "'s turn";
		}
		else if (turn == "white" && this.player2 != turn)
		{
			this.turn_text = this.player2 + "'s turn";
		};
		document.querySelector("[class = 'turn']").textContent = this.turn_text;
	}

	
	update_turn_stone(turn){
		var color = turn == 'white' ? 'linear-gradient(145deg, #ffffff, #f6cfa6)' : 'linear-gradient(145deg, #f6cfa6, #000000)';
		$(".header__turn > .forever_stone").css({
  			background: color,
		})
	}


	nextTurn(){
		this.turn = this.turn == "black" ? "white" : "black";
		this.update_turn_text(this.turn);
		this.update_turn_stone(this.turn);
	}
	

	update_captures_appearance(mark, captures)
	{
		let capture_nb = document.querySelector("div." + mark + "_player_captures");
		capture_nb.querySelector("div.num").textContent = Number(captures) * 2;
	
		const color = '#4CFF33';
		var pixels = 0;

		if(captures < 5 && captures >= 4){
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

		$('.' + mark + '_player_captures .column').css({background: color});
		
		$('.' + mark + '_player_captures .column').animate({
			height: (captures * 2) * 10 + '%',
		});
	
		$('.' + mark + '_player_captures .num').css({'padding-top': pixels + 'px'});
	
		$('.' + mark + '_player_captures .num').animate({'padding-top': pixels + 'px'});
	}


	update_captures(data)
	{
		if (data.b_captures != this.b_captures)
		{
			this.b_captures = data.b_captures;
			this.update_captures_appearance('black', this.b_captures);
		}
		else if (data.w_captures != this.w_captures)
		{
			this.w_captures = data.w_captures;
			this.update_captures_appearance('white', this.w_captures);
		}
		else
		{
			return;
		}
	}

	update_time(time) {
		var real_time = Number(time) / 1e6;
		document.querySelector(".time_text").textContent = (Math.round(real_time * 100) / 100) + ' s';
	}

	update_winner(winner)
	{
		var winner_name = winner == "black" ? this.player1 : this.player2;

		$('.header__turn').css({
			'display': 'none',
			});

	  document.querySelector("[class = 'status']").textContent = winner_name + ' wins !!!';
		
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

	reset_captures() {
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

	reset_turn()
	{
		this.turn = 'black';
		this.update_turn_text(this.turn);
		this.update_turn_stone(this.turn);
	}

	reset_time()
	{
		document.querySelector(".time_text").textContent = 'None yet';
	}

	reset_status()
	{
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


class Board {
	constructor() {

		this.html_board = document.querySelector(".real_board");
		this.place_all_intersections();

	}

	place_intersection(row, column)
	{
		const intersection = document.createElement("div");

		intersection.style.gridRowStart = (row - 1) ;
		intersection.style.gridRowEnd = (row - 1) + 2;
		intersection.style.gridColumnStart = (column - 1) ;
		intersection.style.gridColumnEnd = (column - 1) + 2;

		intersection.classList.add("intersection", "void");

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


	place_stone(row, column, mark)
	{
	  const stone = document.querySelector("[row = '" + row + "'][col = '" + column + "']");
		if (stone)
		{
			stone.classList.add("stone", mark);
			stone.classList.remove("void", "off", "suggested_stone");
		}
	};

	temporary_illegals(on)
	{
		if (on == true)
		{
			const voids = document.querySelectorAll("[class = 'intersection void']");
			if (voids)
			{
				voids.forEach(void_on => {
					void_on.classList.add("off");
				});
			}
			
		}
		else if (on == false)
		{
			const voids_off = document.querySelectorAll("[class = 'intersection void off']");
			if (voids_off)
			{
				voids_off.forEach(void_off => {
					void_off.classList.remove("off");
				});
			}
			
		}
	}

	place_illegal(row, column)
	{
		const illegal = document.querySelector("[class = 'intersection void'][row = '" + row + "'][col = '" + column + "']");

		if (illegal)
		{
			illegal.classList.remove("void");
			illegal.classList.add("illegal");
		}
	}


	place_void(row, column)
	{
		const void_intersection = document.querySelector("[row = '" + row + "'][col = '" + column + "']");
		let replace_off = false;
		if (void_intersection && !void_intersection.classList.contains("void"))
		{
			if (void_intersection.classList.contains("suggested_stone"))
			{replace_off = true};
			void_intersection.classList.remove(...void_intersection.classList);
			void_intersection.classList.add("intersection", "void");
			if (replace_off == true)
			{void_intersection.classList.add("off")};
		}
	}
	

	update_illegals_and_captures(data, turn)
	{
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

	update_all(data, turn)
	{
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
	}

	place_suggestion(row, column, mark)
	{
		const suggested_stone = document.querySelector("[class = 'intersection void'][row = '" + row + "'][col = '" + column + "']");

		if (suggested_stone)
		{
			suggested_stone.classList.remove("void");
			suggested_stone.classList.add("suggested_stone");
		}
	}

	remove_suggestions()
	{
		document.querySelectorAll(".intersection .suggested_stone").forEach(
			suggestion => {
				suggestion.classList.remove("suggested_stone");
				suggestion.classList.add("void");
				suggestion.classList.add("off");
			}
		);
	}

	remove_clicks()
	{
		document.querySelectorAll(".intersection").forEach(
			inter => {
				inter.classList.add("illegal");
			});
	}

}

export default class GameView {
	constructor(form) {
		this.vs_ai = JSON.parse(form.data).cpu;
		this.player1 = form.player1;
		this.player2 = form.player2;
		this.onInterClick = undefined;
		this.onRestartClick = undefined;
		this.onAboutClick = undefined;

		this.board = new Board();
		this.infos = new Infos(this.player1, this.player2);

		document.querySelectorAll(".intersection").forEach(
			inter => {
            inter.addEventListener("click", () => {
                if (this.onInterClick) {
                    this.onInterClick(inter.getAttribute("row"), inter.getAttribute("col"));
                }
            });
        });

		document.querySelector(".header__restart").addEventListener("click", () => {
            if (this.onRestartClick) {
                this.onRestartClick();
            }
        });

		document.querySelector(".header__about").addEventListener("click", () => {
            if (this.onAboutClick) {
                this.onAboutClick();
            }
        });

		
	}


	makeMove(i, j) {
		this.board.place_stone(i, j, this.infos.turn);
		this.board.temporary_illegals(true);
	}


	update(data) {
		if (data.type2 == "player_move")
		{
			if (this.vs_ai == false)
			{
				this.board.remove_suggestions();
			}
			this.board.update_illegals_and_captures(data, this.infos.turn);
			this.infos.update_captures(data);
			if(this.vs_ai == true)
			{
				this.infos.nextTurn();
			}
			else if (this.vs_ai == false)
			{
				this.infos.update_turn_text(null);
			}
		}
		else if (data.type2 == "AI_move" || data.type2 == "AI_move_suggestion")
		{
			if (data.type2 == "AI_move")
			{
				this.board.update_all(data, this.infos.turn);
				this.infos.update_captures(data);
				this.infos.nextTurn();
				this.board.temporary_illegals(false);
			}
			else if (data.type2 == "AI_move_suggestion")
			{
				this.board.temporary_illegals(false);
				var {i, j} = from_nb_to_2d(data.suggested_move);
				this.board.place_suggestion(i, j, this.infos.turn);
				this.infos.nextTurn();
			}
			this.infos.update_time(data.thinking_time);
		}
		if (data.winner == "black" || data.winner == "white")
		{
			this.infos.update_winner(data.winner);
			this.board.remove_clicks();
		}

	}



	restart() {
		
		var non_void = document.querySelectorAll(".intersection,.stone,.black,.white").forEach(
			inter => {
				if (!inter.classList.contains("forever_stone"))
				{
					inter.classList.remove("stone", "black", "white", "illegal", "suggested_stone");
					inter.classList.add("void");
				}
			});
		this.infos.reset_captures();
		this.infos.reset_turn();
		this.infos.reset_time();
		this.infos.reset_status();
		

	}
}
