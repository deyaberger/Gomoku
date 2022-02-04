# Gomoku-AI

The program once run listens for a TCP connection on port 1234.   
In the following file **"server"** shall refer to the Gomook executable and **"client"** to the user interface connecting to it.

<br></br>
## **API specifications**
The server excpects to recieve a json message.   
The json **MUST** contain a **"type"** field with a value in **["start", "move"]**.   specifying the type of message :
- "start" requests a new game
- "move"  plays a move in an ongoing game


<br></br>
### **Start message**
An example start message.
```json
{
	"type"	: "start",
	"cpu"	: true,					# play vs AI
	"player": "white"				# player who plays first
	"depth"	: 7 					# depth of minmax tree
}
```
Accepted values:

	- cpu 		= true / false
	- player	= "white" / "black
	- depth		= [1 . . . 7] 


The server will respond with
```json
{
	"type"	: "game_start",
	"cpu"	: true / false,
	"player": "white" / "black",
	"depth"	: 7 				
}
```
After this **"move"** type messages can be sent.

<br></br>
###	**Move message**
The server awaits a message of the following shape:
```json
{
	"type"	: "move",
	"move"	:  123
}
```
The move number corresponds to `row * 19 + col`.

The server will respond with two successive messages:    
The first message contains the board state after the player's move is computed (can be reconized by: `type2 == "player_move"`).
```json
{
	"type"			: "game_state",
    "type2"			: "player_move",
	"illegal"		: true / false,				# If true the move sent was illegal and was ignored
	"w_captures"	: int						# Number of white captures [1-5]
	"b_captures"	: int						# Number of black captures [1-5]
	"winner"		: "black" / "white" / "no"	# Winner, "no" if no winner
	"player":		: "black" / "white"			# Player who's turn it is
	"cpu"			: true / false				# true if cpu game, false if hotseat game
	"white_board"	: str						# explained below
	"black_board"	: str						# explained below
	"illegal_board"	: str						# explained below
}
```
The second message can be one of two types (identified by the `type2` field): `AI_move` or `AI_suggestion`. This depends on the type of game (cpu or hotseat). If `AI_move` the message will contain the board state after the AI made it's move. (With the `thinking_time` field). If `AI_suggestion` it will contain the same game state as the previous message but with a suggested move and thinking time field.    
**NB:** when playing vs cpu and the player plays a winning move this second message will not be sent.    
Here is an example:
```json
{
   "b_captures": 0,
   "black_board": "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\n",
   "cpu": true,
   "illegal": false,
   "illegal_board": "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\n",
   "player": "white",
   "thinking_time": 97150,
   "type": "game_state",
   "type2": "AI_move",
   "w_captures": 0,
   "white_board": "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\n",
   "winner": "no"
}	
```
As you can see in the above message the `type2` is `AI_move` and there is no `suggested_move field`.

The **"white_board"** field contains a string of `'0'` and `'1'` of length `19 * 19` representing the white pieces on the board.   
1's are the white pieces, 0's the empty squares. The string is in reverse reading order (i.e. the bottom right square is represented by the first char of the string, the second char is the square to the left of the first.).   
Same for **"black_board"**.   
Same for **"illegal_board"** which indicates the positions where it is illegal to play.
