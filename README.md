CruzID: noasulli@ucsc.edu



Introduction:
This lab was an interesting one to say the least.  I would be lying if I said it wasn't difficult.  But
a lie of similar magnitude would be to say I didn't enjoy the challenge.  Working as a pair was actually really
enjoyable.  I generally prefer to do the bulk of the programming I do by myself, but upon encountering any kind of
issue it was nice to have someone to bounce ideas off of that was working with me towards a common goal.

BattleBoats (As I understand it):
Obviously battleboats is a battleship clone, albeit with a few twists that would be lost on the user.  For starters 
the coin flip decision to decide who goes first wasn't as simple as using a random number generator.  
Determing the bit parity of two secret numbersxored together, and implementing a hash very similar to how 
RSA encryption is used.  Once that is determined the two machines playing (whether they are both players or AI) can start
sending messages back and forth to each other using NMEA 0183 protocol.  If using the gui provided by the prof. the user
can play as if they were playing a regular game of battleship, however internally a state machine in tandem with various
functions used to decode and encode messages (as well as update the game field) allows the game to be played.  Eventually
after the boats that were placed at the start are all sunk for one player, that player loses.

Implementation:
This lab really tested my debugging skills.  There are so many moving parts to the entire entity once everything comes
together that even after debugging all of the libaries as indivual components when we put the entire thing together 
nothing would happen.  We were able to get a game going if like the .py challenged the board, and then the .py went first.
But not the other way around.  In order to solve this and other various little bugs mason and I spent a lot of time pouring
over the Agent state machine.  Adding print statements in every statechange as well as developing more in depth test 
harnesses for all of the other libraries.  I remember spending over 2.5 hours debugging just to find out that the state
machine was missing one state change statement.

Reflection:
I think the most important thing I learned while doing this lab is how to properly work on a team towards a common goal
with just a small piece of the puzzle.  I've done collaborative work in other classes but its always been a collaboration
from start -> finish.  But having to understand code you never had to write yourself was key in being able to make everything
come together and to properly debug.  I liked that the end result of this was something really interesting that is cool to
show others and is somethinf even non programmers can look at and be intrigued.  I'm always way more invested in projects
like that when compared to ones like the linked lists library.  While that was fun to me since I'm a big programming nerd
nobody really cares if I go "hey check out this super fast insertion sort algorithm I made using a linked list data
structure!" as opposed to "yo I made battleship check this out".
