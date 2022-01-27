import {from_2d_to_nb} from "./game/Utils.js";

export default class Ws {
	constructor() { // ADD starting data at some point
		this.url = "ws://192.168.1.79:8080";
		this.onOpen = undefined;
	}

	new_connection(gameview) {
		this.co = new WebSocket(this.url);
		
		this.co.onopen = () => {
			if (this.onOpen){
				this.onOpen();
			}
         };
		this.receive_message(gameview);
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
