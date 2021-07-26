# Solving-Pac-Man

## Purpose
The purpose of this assignment is for you to:
 Increase your proficiency in C programming, your dexterity with dynamic memory allocation and
your understanding of data structures, through programming a search algorithm over Graphs.
 Gain experience with applications of graphs and graph algorithms to solving games, one form of
artificial intelligence.

## Description
In this programming assignment you'll be expected to build an AI algorithm to solve Pac-Man. The
game invented in 1980 is one of the classics among arcade games. You can play the game compiling
the code given to you using the keyboard, or using this web implementation.
The code in this assignment was adapted from the open-source terminal version made available by
Mike Billars1 and the original version can be installed as a standard package in Ubuntu2.

### The Pac-Man game
As explained in the wikipedia entry, The player navigates Pac-Man through a maze with no dead
ends. The maze is lled with Pac-Dots, and includes four roving multi-colored ghosts: Blinky, Pinky,
Inky, and Clyde.
The objective of the game is to accumulate as many points as possible by eating dots, fruits, and
ghosts. When all of the dots in a stage are eaten, that stage is completed, and the player will advance
to the next. The four ghosts roam the maze and chase Pac-Man. If any of the ghosts touches Pac-Man,
a life is lost. When all lives have been lost, the game is over.
Pac-Man can eat a fruit rst and then eat the ghosts for a xed period of time to earn bonus points.
The enemies turn deep blue, reverse direction and move away from Pac-Man, and usually move more
slowly. When an enemy is eaten, its eyes return to the center ghost box where the ghost is regenerated
in its normal color. The bonus score earned for eating a blue ghost increases exponentially for each
consecutive ghost eaten while a single energizer is active: a score of 200 points is scored for eating one
ghost, 400 for eating a second ghost, 800 for a third, and 1600 for the fourth.
The level id and a scoreboard can be found on the lower part. The information in the last three lines
of the screen reveals information about the algorithm execution.
The game is won when all dots have been eaten. An AI agent or human player can change the direction
of Pac-Man movements.

## The Algorithm
Each possible conguration of the Pac-Man game 29x28 grid and other relevant information such as
the direction of pacman movements, number of lives left, etc. is called a state. The Pac-Man Graph
G = hV;Ei is implicitly dened. The vertex set V is dened as all the possible congurations (states),
and the edges E connecting two vertexes are dened by the legal movements (right, left, up, down).
Your task is to nd the path leading to the highest score, i.e. leading to the most rewarding vertex
(state). A path is a sequence of movements. You are going to use a variant of Dijkstra to explore
the most rewarding path rst, up to a maximum budget B of expanded/explored nodes (nodes for
which you've already generated its children).
Every time the game asks you for a movement (action), you should explore all possible paths until
consuming the budget B if possible. Once you nished generating all the paths, you should return the
rst action only of the path leading to the highest score vertex. This action will then be executed
by the game engine.
You might have multiple paths with the same maximum score. If more than one action (left,right,up
or down) begins paths with the same maximum score, you'll have to break ties randomly.
Make sure you manage the memory well. Everytime you nish running the algorithm, you have to
free all the nodes from the memory, otherwise you are going to run out of memory fairly fast or cause
memory leaks.