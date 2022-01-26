import {from_2d_to_nb} from "./Utils.js";

export default class Ws {
	constructor() { // ADD starting data at some point
		this.url = "ws://192.168.1.79:8080";
		this.starting_data = JSON.stringify({
			"type"	: "start",
			"cpu"	: true,
			"depth"	: 7
		});
	}

	new_connection(gameview) {
		this.co = new WebSocket(this.url);
		this.co.onopen = () => {
			console.log("YEAAHH you are connected to the Gomoku Server")
			this.co.send(this.starting_data);
		}
		this.receive_message(gameview)
	}

	receive_message(gameview) {
		this.co.onmessage = (e) => {
		// console.log(e.data);
		gameview.update(JSON.parse(e.data));
		}
	}

	send_move(i, j)
	{
		this.co.send(JSON.stringify({
			"type"	: "move",
			"move"	:  from_2d_to_nb(i, j)
		}));
	}

	restart(gameview) {
		this.co.close();
		this.new_connection();
		this.receive_message(gameview);
	}
	
}
