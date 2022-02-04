from ctypes import sizeof
import socket
import json
import os
import logging
import os


HOST = "localhost"  # The server's hostname or IP address
# HOST = os.environ["GOHOST"]  # The server's hostname or IP address
PORT = 8889        # The port used by the server
# PORT = int(os.environ["GOPORT"])        # The port used by the server

class  PermanentClient():
	def __init__(self):
		self.sim_port	= None
		self.board		= ["e"] * (19 * 19)
		self.winner		= "no"
		self.illegal	= False
		self.w_captures	= 0
		self.b_captures	= 0
		self.connected	= False

		# self.connect()
		

	def parse_board(self, response):
		for i in range(19 * 19):
			if (response["black_board"][19 * 19 - 1 - i] == "1"):
				self.board[i] = "b"
			if (response["white_board"][19 * 19 - 1 - i] == "1"):
				self.board[i] = "w"
			if (response["white_board"][19 * 19 - 1 - i] == "0") and (response["black_board"][19 * 19 - 1 - i] == "0"):
				self.board[i] = "e"
	

	def connect(self):
		print(f"CONNNNECTING to host: {HOST}, port: {PORT}\n\n\n\n")
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((HOST, PORT))
		self.connected = True
		

	def close(self):
		self.sock.close()


	def start_game(self, cpu = True):
		msg = {
			"type"	: "start",
			"cpu"	: True,
			"depth"	: 7
		}
		data = json.dumps(msg)
		self.sock.sendall(bytes(data,encoding="utf-8"))
		print(f"Sent start game request:\n {msg}")
		received = self.sock.recv(1024)
		received = received.decode("utf-8")
		received = json.loads(received)
		print(f"Recieved response:\n {received}\n")
		self.board		= ["e"] * (19 * 19)
		self.winner		= "no"
		self.illegal	= False
		self.w_captures	= 0
		self.b_captures	= 0


	def send_move(self, move):
		# self.connect()
		msg = {
			"type": "move",
			"move": move
		} # a real dict.
		data = json.dumps(msg)
		print(f"sending {data}")
		try:
			# Connect to server and send data
			self.sock.sendall(bytes(data,encoding="utf-8"))
			received = self.sock.recv(1024)
			received = received.decode("utf-8")
			received = json.loads(received)
			# self.close()
		except:
			pass
		try:
			print('Received', received)
			return received
		except UnboundLocalError:
			print("Nothing recieved, is server started ?")
		except:
			return received


	def handle_response(self, response):
		print("response ", response)
		self.illegal = False
		if (response["illegal"]):
			print("illegal")
			self.illegal = True

		self.parse_board(response)

		self.w_captures = response["w_captures"]
		self.b_captures = response["b_captures"]


		if (response["winner"] != "no"):
			print(f"Winnner: {response['win']}")
			self.winner = response["winner"]
			return False

		return True


	def choose_move(self):
		move = int(input("Choose a row:\n"))
		move = move * 19;
		move += int(input("col:\n"))
		return move


	def play_game(self):
		self.board =  ["e"] * (19 * 19)
		# self.connect()
		move = self.choose_move()
		response = self.send_move(move)
		while (self.handle_response(response)):
			move = self.choose_move()
			response = self.send_move(move)
		self.close()


	def play_move(self, move):
		response = self.send_move(move)
		print("response", response)
		print("illegal", response["illegal"])

		self.handle_response(response)
		return (self.board, self.illegal, self.winner, self.w_captures, self.b_captures)

	
	def get_state(self):
		return (self.board, self.illegal, self.winner, self.w_captures, self.b_captures)

		

# c = Client()
# board = c.play_move(181)


# # c = Client()
# # c.play_game()
