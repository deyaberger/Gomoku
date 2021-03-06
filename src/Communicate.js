import {from_2d_to_nb} from "./game/Utils.js";

export default class Ws {
	constructor() { // ADD starting data at some point
		this.url = "wss://gomoku.deyaberger.fr:16784";
		this.onOpen = undefined;
		this.onError = undefined;
		this.onClose = undefined;
	}

	new_connection(gameview) {
		this.co = new WebSocket(this.url);
		
		this.co.onopen = () => {
			if (this.onOpen){
				this.onOpen();
			}
         };

		 this.co.onerror = () => {
			if (this.onError){
				this.onError();
			}
         };

		 this.co.onclose = () => {
			if (this.onClose){
				this.onClose();
			}
         };
		this.receive_message(gameview);
      }

   	receive_message(gameview) {
		this.co.onmessage = (e) => {
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

