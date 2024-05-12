#include <bits/stdc++.h>
using namespace std;

typedef array<uint_fast32_t, 6> FaceArray;
// UDFBLR
enum Color{WHITE, YELLOW, BLUE, GREEN, RED, ORANGE};

// two non affecting moves are separated by 2 ie U + 2 -> D
// all '2' moves are odd. ie U2 is 1, D2 is 3
// all single moves are even
// NONE for passing to DFS start
enum Move{U = 0, U2 = 1, D = 2, D2 = 3, F = 10, F2 = 11, B = 12, B2 = 13, L = 20, L2 = 21, R = 22, R2 = 23, NONE = 50};

/* rubiks cube should be read as 

   U    :       W       Y       B       G       R       O      
L  F    :    R  B    R  G    R  Y    O  Y    G  Y    B  Y

Type faces UDFBLR
U(W) -->  GRRRWOGOB
D(Y) -->  GGWBYWOGG
F(B) -->  OWRYBBORO
B(G) -->  WWRBGOYYY
L(R) -->  WBWYRRBYY
R(O) -->  YGBWOOBGR

                        G R R  <--U
                        R W O
                        G O B
                        _____
                W B W | O W R | Y G B | W W R
         L-->   Y R R | Y B B | W O O | B G O   <--B
                B Y Y | O R O | B G R | Y Y Y
                        __F__   __R__
                        G G W
                  D-->  B Y W
                        O G G


rubiks cube is stored as 

            0 1 2  
            7 U 3
            6 5 4
            _____
    0 1 2 | 0 1 2 | 0 1 2 | 0 1 2
    7 L 3 | 7 F 3 | 7 R 3 | 7 B 3
    6 5 4 | 6 5 4 | 6 5 4 | 6 5 4
            _____
            0 1 2
            7 D 3
            6 5 4

*/

char getColorCharacterx(int c);
void printCubex(const FaceArray &faces, const Color centres[6]);

void doMove(const Move &move, FaceArray &faces);
void doMoveList(FaceArray &faces, const vector<Move> &moveList, const Color centres[6]);

void initialiseSolver(const Color centres[6]);

vector<Move> getStage1Moves(const FaceArray &faces, const Color centres[6]);
vector<Move> getStage2Moves(const FaceArray &faces, const Color centres[6]);
vector<Move> getStage3Moves(const FaceArray &faces, const Color centres[6]);
vector<Move> getStage4Moves(const FaceArray &faces, const Color centres[6]);

bool isAllEdgesGood(const FaceArray &faces, const Color centres[6]);
bool isStage2Goal(const FaceArray &faces, const Color centres[6]);
bool isStage3Goal(const FaceArray &faces, const Color centres[6]);
bool isSolved(const FaceArray &faces, const Color centres[6]);