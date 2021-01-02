# Dionysus
A UCI-compatible chess engine, capable of beating (most) human players.

## How to run
First, compile the source code in the `Dionysus` folder. For example, using `g++`, run the following command:
```
g++ *.cpp -o dionysus.exe
```
Now, you need to download a GUI to run the engine with. I recommend [Arena](http://www.playwitharena.de/), but any UCI-compatible GUI should work.
In Arena, click `Engines -> Install new engine` and select the `.exe` generated above. Dionysus should then be loaded into the GUI. You can now either click the `demo` button to watch Dionysus play against itself, or start making moves as white to play against it.

To use an opening book, you will need to download one. Currently, only the [Formula17](https://rybkaforum.net/cgi-bin/rybkaforum/topic_show.pl?tid=33232) opening book is supported, but plans are for custom books to be supported in the future.
Download and unzip the file, leaving the `Book_Formula17` folder next to the generated `.exe`.

## How it works
### Talking to the GUI
Dionysus keeps track of the current board state internally, including the position of each pieces, the number of moves since the last pawn move or capture (relevant for the [50 move rule](https://www.chessprogramming.org/Fifty-move_Rule)), the castling rights of each side and more. It then communicates with the GUI using the [UCI protocol](http://wbec-ridderkerk.nl/html/UCIProtocol.html) (Universal Chess Interface), which tells the engine what moves have been played and when to start and stop calculating.

### Search Overview
If an opening book is enabled, and the position is in the book, then a random move from the book is selected and played. If an opening book is not present, or if the position is not in the book, then a move is searched for normally. The engine uses an iteratively deepening search for each move. It begins by searching to a depth of 1 ply (or half-move), then searches to a depth of 2, then 3 and so on until its time for that move has been fully used. At that point, the best move found in the most recently fully completed search is played. 
The search to each depth is done using the [negamax](https://en.wikipedia.org/wiki/Negamax) algorithm (a structural variant on the more well known minimax algorithm). 

### Search Optimisations
[Alpha-beta pruning](https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning) is used to speed up the search, by skipping over game tree nodes which we know will be irrelevant to the final outcome of the search. This allows us to search far fewer nodes, and still produce the same answer.
In addition, a [transposition table](https://en.wikipedia.org/wiki/Negamax#Negamax_with_alpha_beta_pruning_and_transposition_tables) is used memoise the results of previous nodes in the search. Then, if we encounter the same game position again at a lower depth, we do not need to recompute the score for that position, and can instead use the score stored in the transposition table.
A [zobrist hash](https://www.chessprogramming.org/Zobrist_Hashing) is generated incrementally every time a move is made or trialled by the search. This allows us to efficiently create and store a (mostly) unique, 64-bit hash value for each board position. This is useful for the transposition table, as it means no additional hash function is required, we can simple store the zobrist hash as the key, and still have O(1) access.

### Position Evaluation
At the leaves of each search tree (where the depth has reached the max for that search) a [quiescence search](https://en.wikipedia.org/wiki/Quiescence_search) is used to stabilise the position. The quiescence search continues the normal search, only considering moves which are captures until there are none that remain, at which point the position is evaluated and the score returned. Extending the search in this way can help to mitigate the [horizon effect](https://en.wikipedia.org/wiki/Horizon_effect). For example, if the normal negamax search reaches its max depth halfway through a queen trade, when only one queen has been captured, stopping here would lead the evaluation function to believe that one side is a queen up, when in fact it will just be taken on the next move. The quiescence search extends the search past the end of the queen trade, preventing this.
The evaluation function is currently very basic, and is based on only two factors. Firstly, how many pieces are left on the board (weighted by the value of each piece e.g. pawn=1, knight=3 and so on). Secondly, the 'mobility' (the number of available moves) of each side is taken into account. This aims to encourage the engine to develop its pieces onto key squares, and to try and control the centre of the board.
