def print_board(board):
	for i in range(19):
		print(" {i:2d} ", end="")
	print("")
	for i in range(19):
		print("____", end="")
	print("")
	for i in range(19):
		for j in range(19):
			if (board[i * 19 + j] == "w"):
				print(f"| X ", end="")
			if (board[i * 19 + j] == "b"):
				print(f"| O ", end="")
			if (board[i * 19 + j] == "e"):
				print(f"|   ", end="")

		print(f"| {i:2d}")

		for i in range(19):
			print("____", end="")
		print("")


from Client import PermanentClient
if __name__ == "__main__":
	c = PermanentClient()
	c.connect()
	c.start_game()
	c.play_move(181)
	print_board(c.board)
	c.play_move(200)
	print_board(c.board)
 
			

