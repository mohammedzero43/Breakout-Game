# Breakout-Game
Breakout game made with C++ and OpenGL &amp; GLUT libraries</br>

# Dependencies :
GLUT Library</br>
OPENGL Library (installed by default)</br>
GCC-C++ (or any C++ compiler)</br>

# Linker/Build Command:
g++ -Wall -o "%e" "%f" -lGL -lGLU -lglut </br>

# Game Controls
A-D : player movement</br>
R   : Restart Game after winning/losing</br>
Esc : Exit Game</br>

# PowerUps:
4 Combo : Expand player pad</br>
5 Combo : FireBall</br>

# Errors
Stack Dumb Error in VC++ - Collision checking loops (Solution : increase loops from 8 to 52. <or find another solution>)</br>
