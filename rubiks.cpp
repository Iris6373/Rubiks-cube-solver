#include "rubiks.h"
#include <bits/stdc++.h>
using namespace std;

#define F_UP 0
#define F_DOWN 1
#define F_FRONT 2
#define F_BACK 3
#define F_LEFT 4
#define F_RIGHT 5

set<uint_fast64_t> permutations;

bool doDFS(const int searchDepth, vector<Move> &moveList, 
        const array<Move, 6> &availableMoves, const FaceArray &faces, const Color centres[6], 
        bool (*isStageGoal)(const FaceArray &faces, const Color centres[6]), const Move &lastMove) { // isStageGoal goes to the corressponding group stageGoal

    if(searchDepth == 0) {
        return false;
    }

    for(auto &m: availableMoves) {

        // repeating U2 takes back to base state
        if(lastMove%2 && m == lastMove)    // if its double move (since all double moves are odd in Move) and equal to current move (the current double move is equal to last move)
            continue;
        // no need to do after B after F. since F after B will be the same state
        if(m - lastMove == 2)       // if non affecting moves (F after B or B after F, U after D or D after U, L after R or R after L)
            continue;
        
        auto facesCopy = faces;

        doMove(m, facesCopy);
        if(isStageGoal(facesCopy, centres)) {           // the correct sequence is added to moveList only when it satisfies this condition
            moveList.insert(moveList.begin(), m);       // It is added in the front of moveList since we are doing a DFS call and the current moves should be done in the reverse order
            return true;
        }

        if(doDFS(searchDepth - 1, moveList, availableMoves, facesCopy, centres, isStageGoal, m)) {
            moveList.insert(moveList.begin(), m);       // the correct sequence is added to moveList only when it satisfies this condition
            return true;
        }
        
    }

    return false;
}

vector<Move> doIDDFS(const array<Move, 6> &availableMoves, 
        const FaceArray &faces, const Color centres[6], 
        bool (*isStageGoal)(const FaceArray &faces, const Color centres[6])) {

    vector<Move> moveList;
    cout<< "\n" <<"IDDFS Depth: 1";
    int searchDepth = 1;
    while(!doDFS(searchDepth, moveList, availableMoves, faces, centres, isStageGoal, NONE)) {
        ++searchDepth;
        cout << ". " << searchDepth;
    }
    cout << ". Complete" << "\n";

    return moveList;
}


Color getSquareColor(const FaceArray &faces, int faceIndex, int cubieIndex) {
    int shiftBits = 4 * (7 - cubieIndex);
    int cubieColor = (faces[faceIndex]>>shiftBits) & 0xF;
    return static_cast<Color>(cubieColor);
}

// returns the face index for a particular color
int getFace(const Color centres[6], Color faceColor) {
    for(int i = 0; i < 6; ++i) {
        if(faceColor == centres[i])
            return i;
    }
    return -1;
}

// returns the index of an edge cubie of given face and color
int getEdgeCubieIndex(const Color centres[6], int cubieFaceIndex, const Color &edgeColor) {
    switch(cubieFaceIndex) {
        case F_FRONT: {
            if(edgeColor == centres[F_UP])
                return 1;
            if(edgeColor == centres[F_RIGHT])
                return 3;
            if(edgeColor == centres[F_DOWN])
                return 5;
            if(edgeColor == centres[F_LEFT])
                return 7;
            break;
        }        
        case F_BACK: {
            if(edgeColor == centres[F_UP])
                return 1;
            if(edgeColor == centres[F_RIGHT])
                return 7;
            if(edgeColor == centres[F_DOWN])
                return 5;
            if(edgeColor == centres[F_LEFT])
                return 3;
            break;
        }        
        case F_UP: {
            if(edgeColor == centres[F_FRONT])
                return 5;
            if(edgeColor == centres[F_RIGHT])
                return 3;
            if(edgeColor == centres[F_BACK])
                return 1;
            if(edgeColor == centres[F_LEFT])
                return 7;
            break;
        }        
        case F_DOWN: {
            if(edgeColor == centres[F_FRONT])
                return 1;
            if(edgeColor == centres[F_RIGHT])
                return 3;
            if(edgeColor == centres[F_BACK])
                return 5;
            if(edgeColor == centres[F_LEFT])
                return 7;
            break;
        }
        case F_LEFT: {
            if(edgeColor == centres[F_UP])
                return 1;
            if(edgeColor == centres[F_BACK])
                return 7;
            if(edgeColor == centres[F_DOWN])
                return 5;
            if(edgeColor == centres[F_FRONT])
                return 3;
            break;
        }
        case F_RIGHT: {
            if(edgeColor == centres[F_UP])
                return 1;
            if(edgeColor == centres[F_BACK])
                return 3;
            if(edgeColor == centres[F_DOWN])
                return 5;
            if(edgeColor == centres[F_FRONT])
                return 7;
            break;
        }
    }
    return -1;
}

uint_fast64_t makePermutation(const FaceArray &faces) {      // This is to form all possible permuations of 4 corners
    uint_fast64_t perm = 0;
    for(auto &f: {F_LEFT, F_FRONT, F_RIGHT, F_BACK}) {
        for(int i = 0; i <=6; i+=2) {
            // cout << getSquareColor(faces, f, i) << " ";
            perm = (perm << 4) | getSquareColor(faces, f, i);
        }
        // cout << "\t";
    }
    // cout << "\n";
    return perm;
}

void printPermutations(set<uint_fast64_t> &permutations) {
    int i = 1;
    for (auto f : permutations) {
        int b = 0;
        cout << i << "\t";
        while (f) {
            int a = (f & 0xF);
            cout << static_cast<Color>(a) << " ";
            f >>= 4;
            b++;
            if (b%4 == 0) cout << "\t";
        }
        i++;
        cout << "\n";
    }
}

bool isInitialiseStageGoal(const FaceArray &faces, const Color centres[6]) {       // 4.4! = 96 
    auto perm = makePermutation(faces);
    if(permutations.count(perm) == 0) {
        permutations.insert(perm);

        if(permutations.size() == 96)   // there are 96 possible permutations
            return true;
    }

    return false;
}

// to generate the 96 permutations obtained by 180 degree turns from solved state. (for g3 condition)  
void initialiseSolver(const Color centres[6]) {       
    FaceArray solvedState;
    for(int i = 0; i < 6; ++i) {
        uint_fast32_t face = 0;
        for(int j = 0; j < 8; ++j) {
            face = (face<<4) | centres[i];
        }
        solvedState[i] = face;
    }
    array<Move, 6> availableMoves{L2, R2, F2, B2, U2, D2};
    doIDDFS(availableMoves, solvedState, centres, isInitialiseStageGoal);
}

void doMove(const Move &move, FaceArray &faces) {
    switch(move) {
        case U2: doMove(U, faces);                                                          // First runs the function doMove(U, faces); and comes back to the next line and executes case U:, since there is no break
        case U: {
            faces[F_UP] = (faces[F_UP]>>8) | ((faces[F_UP]&0xFF)<<24);                      // rotate F_UP by 90 deg Clockwise twist by rolling the last 8 bits to front

            uint_fast32_t F_value = faces[F_FRONT]&0xFFF00000;
            faces[F_FRONT] = (faces[F_FRONT]&0x000FFFFF) | (faces[F_RIGHT]&0xFFF00000);     // replace 0-1-2 bits of F_FRONT with 0-1-2 of F_RIGHT
            faces[F_RIGHT] = (faces[F_RIGHT]&0x000FFFFF) | (faces[F_BACK]&0xFFF00000);      // replace 0-1-2 bits of F_RIGHT with 0-1-2 of F_BACK
            faces[F_BACK] = (faces[F_BACK]&0x000FFFFF) | (faces[F_LEFT]&0xFFF00000);        // replace 0-1-2 bits of F_BACK with 0-1-2 of F_LEFT
            faces[F_LEFT] = (faces[F_LEFT]&0x000FFFFF) | F_value;                           // replace 0-1-2 bits of F_LEFT with 0-1-2 of F_value
            break;
        }

        case D2: doMove(D, faces);
        case D: {
            faces[F_DOWN] = (faces[F_DOWN]>>8) | ((faces[F_DOWN]&0xFF)<<24);

            uint_fast32_t F_value = faces[F_FRONT]&0x0000FFF0;
            faces[F_FRONT] = (faces[F_FRONT]&0xFFFF000F) | (faces[F_LEFT]&0x0000FFF0);
            faces[F_LEFT] = (faces[F_LEFT]&0xFFFF000F) | (faces[F_BACK]&0x0000FFF0);
            faces[F_BACK] = (faces[F_BACK]&0xFFFF000F) | (faces[F_RIGHT]&0x0000FFF0);
            faces[F_RIGHT] = (faces[F_RIGHT]&0xFFFF000F) | F_value;
            break;
        }

        case F2: doMove(F, faces);
        case F: {
            faces[F_FRONT] = (faces[F_FRONT]>>8) | ((faces[F_FRONT]&0xFF)<<24);             // rotate F_FRONT by 90 degree Clockwise twist by rolling the last 8 bits to front

            uint_fast32_t F_value = faces[F_UP]&0x0000FFF0, val;

            faces[F_UP] = (faces[F_UP]&0xFFFF000F) | ((faces[F_LEFT]&0x00FFF000)>>8);       // replace 4-5-6 bits of F_UP with 2-3-4 of F_LEFT
            faces[F_LEFT] = (faces[F_LEFT]&0xFF000FFF) | ((faces[F_DOWN]&0xFFF00000)>>8);   // replace 2-3-4 bits of F_LEFT with 0-1-2 of F_DOWN
            val = ((faces[F_RIGHT]&0x000000FF)<<24) | ((faces[F_RIGHT]&0xF0000000)>>8);     // roll bits [6-7-0 to 0-1-2] and stores only 6-7-0 bits
            faces[F_DOWN] = (faces[F_DOWN]&0x000FFFFF) | val;                               // replace 0-1-2 bits from F_RIGHT
            val = (F_value>>8) | (F_value<<24);                                             // stores 4-5 bits in 6-7 and 6 in 0 bit
            faces[F_RIGHT] = (faces[F_RIGHT]&0x0FFFFF00) | val;                             // replace 0-6-7 bits from val
            break;
        }
        
        case B2: doMove(B, faces);
        case B: {
            faces[F_BACK] = (faces[F_BACK]>>8) | ((faces[F_BACK]&0xFF)<<24);

            uint_fast32_t F_value = (faces[F_UP]&0xFFF00000), val;

            faces[F_UP] = (faces[F_UP]&0x000FFFFF) | ((faces[F_RIGHT]&0x00FFF000)<<8);
            faces[F_RIGHT] = (faces[F_RIGHT]&0xFF000FFF) | ((faces[F_DOWN]&0x0000FFF0)<<8);
            val = ((faces[F_LEFT]&0x000000FF)<<8) | ((faces[F_LEFT]&0xF0000000)>>24);
            faces[F_DOWN] = (faces[F_DOWN]&0xFFFF000F) | val;
            val = (F_value>>24) | (F_value<<8);
            faces[F_LEFT] = (faces[F_LEFT]&0x0FFFFF00) | val;
            break;
        }

        case L2: doMove(L, faces);
        case L: {
            faces[F_LEFT] = (faces[F_LEFT]>>8) | ((faces[F_LEFT]&0xFF)<<24);

            uint_fast32_t F_value = (faces[F_UP]&0xF00000FF), val;

            val = ((faces[F_BACK]&0x00FF0000)>>16) | ((faces[F_BACK]&0x0000F000)<<16);
            faces[F_UP] = (faces[F_UP]&0x0FFFFF00) | val;
            val =  ((faces[F_DOWN]&0x000000FF)<<16) | ((faces[F_DOWN]&0xF0000000)>>16);
            faces[F_BACK] = (faces[F_BACK]&0xFF000FFF) | val;
            faces[F_DOWN] = (faces[F_DOWN]&0x0FFFFF00) | (faces[F_FRONT]&0xF00000FF);
            faces[F_FRONT] = (faces[F_FRONT]&0x0FFFFF00) | F_value;
            break;
        }

        case R2: doMove(R, faces);
        case R: {
            faces[F_RIGHT] = (faces[F_RIGHT]>>8) | ((faces[F_RIGHT]&0xFF)<<24);

            uint_fast32_t F_value = (faces[F_UP]&0x00FFF000), val;

            faces[F_UP] = (faces[F_UP]&0xFF000FFF) | (faces[F_FRONT]&0x00FFF000);
            faces[F_FRONT] = (faces[F_FRONT]&0xFF000FFF) | (faces[F_DOWN]&0x00FFF000);
            val = ((faces[F_BACK]&0x000000FF)<<16) | ((faces[F_BACK]&0xF0000000)>>16);
            faces[F_DOWN] = (faces[F_DOWN]&0xFF000FFF) | val;
            val = (F_value>>16) | (F_value<<16);
            faces[F_BACK] = (faces[F_BACK]&0x0FFFFF00) | val;
            break;
        }
    }
}

void doMoveList(FaceArray &faces, const vector<Move> &moveList, const Color centres[6]) {
    for(auto &m: moveList) {
        doMove(m, faces);
        // printCubex(faces, centres);
    }
}

// an edge piece is "GOOD if it can be moved to its solved position without using quarter turns of the up or down faces
bool isGoodEdge(int fromFace, int fromCubieInd, int toFace, int toCubieInd) {

    /* 
            0 A 0  <--U
            B   B
            0 A 0
            _____
    0 A 0 | 0 B 0 | 0 A 0 | 0 B 0
    A   A | B   B | A   A | B   B
    0 A 0 | 0 B 0 | 0 A 0 | 0 B 0
            _____
            0 A 0
            B   B
            0 A 0

    if an 'A' edge actually belongs in a 'B' position, then a quarter up/down turn is needed and hence the edge is bad
    and vice versa

    if 'A' edge belongs to 'A' position then edge is good

    */

    static bool A_edges[6][8] = {
        {0, 1, 0, 0, 0, 1, 0, 0}, {0, 1, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0, 1, 0, 1}, {0, 1, 0, 1, 0, 1, 0, 1}
    };

    return A_edges[fromFace][fromCubieInd] == A_edges[toFace][toCubieInd]; 
}

bool isAllEdgesGood(const FaceArray &faces, const Color centres[6]) {
    // 12 edges

    // F edges -> 4      [(1, 5), (3, 7), (5, 1), (7, 3)]   [(F_FRONT, F_UP), (F_FRONT, F_RIGHT), (F_FRONT, F_DOWN), (F_FRONT, F_LEFT)]
    // B edges -> 4      [(1, 1), (3, 7), (5, 5), (7, 3)]   [(F_BACK, F_UP), (F_BACK, F_RIGHT), (F_BACK, F_DOWN), (F_BACK, F_LEFT)]
    // edges in between -> 4     [(1, 7), (3, 1), (5, 3), (7, 5)] [(F_LEFT, F_UP), (F_UP, F_RIGHT), (F_RIGHT, F_DOWN), (F_DOWN, F_LEFT)]

    int i = 1, j = i, f = F_LEFT;
    int toFace, toCubieInd;

    for (auto face : {F_UP, F_RIGHT, F_DOWN, F_LEFT}) {

        // F edges
        toFace = getFace(centres, getSquareColor(faces, F_FRONT, i));
        toCubieInd = getEdgeCubieIndex(centres, toFace, getSquareColor(faces, face, (i+4)%8));
        if(!isGoodEdge(F_FRONT, i, toFace, toCubieInd))
            return false;

        // B edges
        toFace = getFace(centres, getSquareColor(faces, F_BACK, i));  
        toCubieInd = getEdgeCubieIndex(centres, toFace, getSquareColor(faces, face, j)); 
        if(!isGoodEdge(F_BACK, i, toFace, toCubieInd))
            return false;
        j = (j == i) ? (i+6)%8 : i+2;

        // between edges
        toFace = getFace(centres, getSquareColor(faces, f, i));
        toCubieInd = getEdgeCubieIndex(centres, toFace, getSquareColor(faces, face, (i+6)%8));
        if(!isGoodEdge(f, i, toFace, toCubieInd))
            return false;
        f = face;
        
        i += 2;
    }

    return true;
}

vector<Move> getStage1Moves(const FaceArray &faces, const Color centres[6]) {       //should be done in 7 moves

    if(isAllEdgesGood(faces, centres))
        return vector<Move>();

    array<Move, 6> availableMoves{L, R, F, B, U, D};
    return doIDDFS(availableMoves, faces, centres, isAllEdgesGood);
}

// the cube is moved to a state such that all corners are correctly oriented. 
// i.e, if red is the L face, then all 8 corner cubies have red or orange on the L and R faces
// Also, four of the edges are moved to the correct slice: The FU, FD, BU, BD edges are placed in the slice b/w L and R
// If this is not possible, get them all in the U-face in at most 4 moves.
bool isStage2Goal(const FaceArray &faces, const Color centres[6]) {
    const Color &color_l = centres[F_LEFT], &color_r = centres[F_RIGHT];

    // left and right face check if all 8 edges contain either R or O
    for(int i = 0; i <= 6; i+=2) { 
        Color c1 = getSquareColor(faces, F_LEFT, i), c2 = getSquareColor(faces, F_RIGHT, i);
        if(!((c1 == color_l || c1 == color_r) && (c2 == color_l || c2 == color_r)))
            return false;
    }

    const Color &color_f = centres[F_FRONT], &color_b = centres[F_BACK], &color_u = centres[F_UP], &color_d = centres[F_DOWN];

    // check for edges between L(R) and R(O) faces
    for(auto &i: {1, 5}) {
        Color c1 = getSquareColor(faces, F_UP, i), c2 = getSquareColor(faces, F_DOWN, i);
        if(!((c1 == color_u || c1 == color_d) && (c2 == color_u || c2 == color_d)))
            return false; 

        c1 = getSquareColor(faces, F_FRONT, i); c2 = getSquareColor(faces, F_BACK, i);
        if(!((c1 == color_f || c1 == color_b) && (c2 == color_f || c2 == color_b)))
            return false; 
    }

    return true;

}

vector<Move> getStage2Moves(const FaceArray &faces, const Color centres[6]) {
    
    if(isStage2Goal(faces, centres))
        return vector<Move>();

    array<Move, 6> availableMoves{L, R, F, B, U2, D2};
    return doIDDFS(availableMoves, faces, centres, isStage2Goal);
}

// all corners are moved to the correct orbit, meaning that each corner can be moved to its home position with only 180-degree twists. 
// Also, each edge is at its home slice. Every face should only contain edge from it and opposite face
// one of 96 possible corner permutations
bool isStage3Goal(const FaceArray &faces, const Color centres[6]) {    // Check if U and D, F and B, L and R faces have either of its colours

    const Color &color_u = centres[F_UP], &color_d = centres[F_DOWN], &color_f = centres[F_FRONT], 
                &color_b = centres[F_BACK], &color_l = centres[F_LEFT], &color_r = centres[F_RIGHT];

    for (auto &i: {1, 3, 5, 7}) {
        Color c1 = getSquareColor(faces, F_UP, i), c2 = getSquareColor(faces, F_DOWN, i);
        if (!((c1 == color_u || c1 == color_d) && (c2 == color_u || c2 == color_d)))
            return false; 

        c1 = getSquareColor(faces, F_FRONT, i); c2 = getSquareColor(faces, F_BACK, i);
        if (!((c1 == color_f || c1 == color_b) && (c2 == color_f || c2 == color_b)))
            return false; 
        
        c1 = getSquareColor(faces, F_LEFT, i); c2 = getSquareColor(faces, F_RIGHT, i);
        if (!((c1 == color_l || c1 == color_r) && (c2 == color_l || c2 == color_r)))
            return false; 
    }

    return permutations.count(makePermutation(faces)) == 1;
}

vector<Move> getStage3Moves(const FaceArray &faces, const Color centres[6]) {

    if(isStage3Goal(faces, centres))
        return vector<Move>();

    array<Move, 6> availableMoves{L, R, F2, B2, U2, D2};
    return doIDDFS(availableMoves, faces, centres, isStage3Goal);
}

bool isSolved(const FaceArray &faces, const Color centres[6]) {
    for(int f = 0; f < 6; ++f) {
        for(int i = 0; i < 8; ++i) {
            if(getSquareColor(faces, f, i) != centres[f])
                return false;
        }
    }
    return true;
}

vector<Move> getStage4Moves(const FaceArray &faces, const Color centres[6]) {

    if(isSolved(faces, centres))
        return vector<Move>();
    // printPermutations(permutations);
    array<Move, 6> availableMoves{L2, R2, F2, B2, U2, D2};
    return doIDDFS(availableMoves, faces, centres, isSolved);
}