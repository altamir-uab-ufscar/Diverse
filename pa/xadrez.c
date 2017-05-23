#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define NUMPIECES 32
#define NUMRANKS 6

const char ranks[NUMRANKS] = {'P', 'N', 'B', 'R', 'Q', 'K'};

struct xpiece {
	unsigned char isActive, hasMoved;
	int xp, yp;
	unsigned char (*moveFun)(char **, char *, char *, int, int, int, int, int);
};

/*
unsigned char moveToHere(char **board, char* list, int x2, int y2, int x1, int y1) {
  int idx;
  char type;
  
  if (isEmpty(board[x1][y1])) {
    return 0;
  }
  type = toupper(board[x1][y1]);
  switch (type) {
    case 'K':
      break;
    case 'Q':
      break;
    case 'B':
      idx = find(list, x1, y1);
//      return piece.moveFun(x1, y1, x2, y2, board);
      break;
    case 'N':
      break;
    case 'R':
      break;
    case 'P':
		break;
  }
}
*/

char encodeRow(int x) {
	return '8' - x;
}

char encodeColumn(int y) {
	return y + 'a';
}

int decodeRow(char c) {
	return '8' - c;
}

int decodeColumn(char c) {
	return c - 'a';
}


int straightDistance(int x1, int y1, int x2, int y2) {
	return abs(x1 - x2) + abs(y1 - y2);
}

int diagonalDistance(int x1, int x2) {
	return abs(x1 - x2);
}

int move(int p1, int p2) {
  if (p1 == p2) {
    return 0;
  }
  return p1 < p2 ? 1 : -1;
}

int abs(int a) {
  return a < 0 ? -a : a; 
}

int min(int a, int b) {
  return a < b ? a : b;
}

int max(int a, int b) {
  return a > b ? a : b;
}

int yCoordinate(char *pos) {
	return pos[0] - 'a';
}

int xCoordinate(char *pos) {
	return 7 - (pos[1] - '1');
}

unsigned char isPawn(char piece) {
	return toupper(piece) == 'P' ? 1 : 0;
}

unsigned char isBishop(char piece) {
	return toupper(piece) == 'B' ? 1 : 0;
}

unsigned char isRook(char piece) {
	return toupper(piece) == 'R' ? 1 : 0;
}

unsigned char isKnight(char piece) {
	return toupper(piece) == 'N' ? 1 : 0;
}

unsigned char isKing(char piece) {
	return toupper(piece) == 'K' ? 1 : 0;
}

unsigned char isQueen(char piece) {
	return toupper(piece) == 'Q' ? 1 : 0;
}

unsigned char areOfSameType(char piece1, char piece2) {
	return toupper(piece1) == toupper(piece2) ? 1 : 0;
}

char isPlayer(char piece) {
	if (piece >= 'A' && piece <= 'Z') {
		return 'w';
	}
	if (piece >= 'a' && piece <= 'z') {
		return 'b';
	}
	return -1;
}

char *append(char *str, char *word) {
	int i = 0, j = 0;
	
	while (str[i] != 0) {
		++i;
	}
	while (word[j] != 0) {
		str[i++] = word[j++];
	}
	str[i] = 0;
	return str;
}

unsigned char isEmpty(char piece) {
	return piece == '.' ? 1 : 0;
}

unsigned char areEnemies(char piece1, char piece2) {
	if ((piece1 >= 'A' && piece1 <= 'Z' && piece2 >= 'a' && piece2 <= 'z') || (piece2 >= 'A' && piece2 <= 'Z' && piece1 >= 'a' && piece1 <= 'z')) {
		return 1;
	}
	return 0;
}

unsigned char areFriends(char piece1, char piece2) {
	if ((piece1 >= 'A' && piece1 <= 'Z' && piece2 >= 'A' && piece2 <= 'Z') || (piece2 >= 'a' && piece2 <= 'z' && piece1 >= 'a' && piece1 <= 'z')) {
		return 1;
	}
	return 0;
}

unsigned char isWhite(char piece) {
	return piece >= 'A' && piece <= 'Z';
}

unsigned char isBlack(char piece) {
	return piece >= 'a' && piece <= 'z';
}

unsigned char isAhead(char piece, int x1, int x2) {
	if ((isWhite(piece) && x2 < x1) || (isBlack(piece) && x1 < x2)) {
		return 1;
	}
	return 0;
}

unsigned char isFirstRow(int x) {
	return x == 7 ? 1 : 0;
}

unsigned char isLastRow(int x) {
	return x == 0 ? 1 : 0;
}

unsigned char isSecondRow(int x) {
	return x == 6 ? 1 : 0;
}

unsigned char isPenultimateRow(int x) {
	return x == 1 ? 1 : 0;
}

unsigned char areVerticallyAligned(int y1, int y2) {
	return y1 == y2 ? 1 : 0;
}

unsigned char areHorizontallyAligned(int x1, int x2) {
	return x1 == x2 ? 1 : 0;
}

unsigned char areDiagonallyAligned(int x1, int y1, int x2, int y2) {
	return abs(x1 - x2) == abs(y1 - y2) ? 1 : 0;
}

int find(struct xpiece *list, int x, int y) {
  int i;
  
  for (i = 0; i < NUMPIECES; ++i) {
    if (list[i].xp == x && list[i].yp == y) {
      return i;
    }
  }
  return -1;
}

unsigned char hasCastlingAvailable(char *castling, char king) {
	int i = 0;
	
	while (castling[i] != 0) {
		if ((isWhite(king) && isWhite(castling[i])) || (isBlack(king) && isBlack(castling[i]))) {
			return 1;
		}
		++i;
	}
	return 0;
}

unsigned char canCastle(char *castling, int x, int y) {
	int i = 0;
	
	while (castling[i] != 0) {
		switch (castling[i]) {
			case 'K':
				if (x == 7 && y == 6) {
					return 1;
				}
			case 'Q':
				if (x == 7 && y == 2) {
					return 1;
				}
			case 'k':
				if (x == 0 && y == 6) {
					return 1;
				}
			case 'q':
				if (x == 0 && y == 2) {
					return 1;
				}
		}
		++i;
	}
	return 0;
}

unsigned char canVerticalMove(char **board, int x1, int y1, int x2, int y2) {
	if ((x1 == x2 && y1 == y2) || (x1 != x2 && y1 != y2)) {
		return 0;
	}
	while (x1 != x2 || y1 != y2) {
		x1 += move(x1, x2);
		y1 += move(y1, y2);
		if (x1 == x2 && y1 == y2) {
			if (isEmpty(board[x1][y1]) || isEnemy(board[x1][y1], board[x2][y2])) {
				return 1;
			}
		}
		if (!isEmpty(board[x1][y1])) {
			return 0;
		}
	}
	return 0;
}

unsigned char canDiagonalMove(char **board, int x1, int y1, int x2, int y2) {
	if ((x1 == x2 && y1 == y2) || abs(x1 - x2) != abs(y1 - y2)) {
		return 0;
	}
	while (x1 != x2 || y1 != y2) {
		x1 += move(x1, x2);
		y1 += move(y1, y2);
		if (x1 == x2 && y1 == y2) {
			if (isEmpty(board[x1][y1]) || isEnemy(board[x1][y1], board[x2][y2])) {
				return 1;
			}
		}
		if (!isEmpty(board[x1][y1])) {
			return 0;
		}
	}
	return 0;
}

unsigned char canLMove(char **board, int x1, int y1, int x2, int y2) {
	return ((!isFriend(board[x1][y1], board[x2][y2]) && (abs(x1 - x2) == 2 && abs(y1 - y2) == 1)) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 2)) ? 1 : 0; 
}

unsigned char isToPromote(char **board, int x1, int y1, int x2, int y2) {
	return (isPawn(board[x1][y1]) && !isFriend(board[x1][y1], board[x2][y2]) && abs(y1 - y2) <= 1 && isLastRow(x1 - 1) && isWhite(board[x1][y1])) || (isFirstRow(x1 + 1) && isBlack(board[x1][y1])) ? 1 : 0; 
}

unsigned char isExiting(char piece, int x) {
	return (isPawn(piece) && isSecondRow(x) && isWhite(piece)) || (isPenultimateRow(x) && isBlack(piece)) ? 1 : 0;
}

unsigned char canBishopMove(char **board, char *castling, char *enpassant, int x1, int y1, int x2, int y2) {
	if ((areVerticallyAligned(y1, y2) && areHorizontallyAligned(x1, x2)) || !isWithinBounds(x1, y1)) || !isWithinBounds(x2, y2)) {
		return 0;
	}
	return canDiagonalMove(board, x1, y1, x2, y2);
}

// ok?
unsigned char canRookMove(char **board, char *castling, char *enpassant, int moveType, int x1, int y1, int x2, int y2) {
	if ((areVerticallyAligned(y1, y2) && areHorizontallyAligned(x1, x2)) || !isWithinBounds(x1, y1)) || !isWithinBounds(x2, y2)) {
		return 0;
	}
	char thisOne = board[x1][y1];
	char thatOne = board[x2][y2];
	switch (moveType) {
		case 0:
			return canVerticalMove(board, x1, y1, x2, y2) & isEmpty(thisOne);
			break;
		case 1:
			return canVerticalMove(board, x1, y1, x2, y2) & areEnemies(thisOne, thatOne);
	}
	return 0;
}

// ok?
unsigned char canQueenMove(char **board, char *castling, char *enpassant, int x1, int y1, int x2, int y2) {
	if ((areVerticallyAligned(y1, y2) && areHorizontallyAligned(x1, x2)) || !isWithinBounds(x1, y1)) || !isWithinBounds(x2, y2)) {
		return 0;
	}
	char thisOne = board[x1][y1];
	char thatOne = board[x2][y2];
	switch (moveType) {
		case 0:
			return (canDiagonalMove(board, x1, y1, x2, y2) | canVerticalMove(board, x1, y1, x2, y2)) & isEmpty(thisOne);
			break;
		case 1:
			return (canDiagonalMove(board, x1, y1, x2, y2) | canVerticalMove(board, x1, y1, x2, y2)) & areEnemies(thisOne, thatOne);
	}
	return 0;
}

unsigned char canKingMove(char **board, char *castling, char *enpassant, int moveType, int x1, int y1, int x2, int y2) {
	if ((areVerticallyAligned(y1, y2) && areHorizontallyAligned(x1, x2)) || !isWithinBounds(x1, y1)) || !isWithinBounds(x2, y2)) {
		return 0;
	}
	char thisOne = board[x1][y1];
	char thatOne = board[x2][y2];
	switch (moveType) {
		case 0:
			if (isAhead(thisOne, x1, x2) && areVerticallyAligned(y1, y2)) {
				
				switch (straightDistance(x1, y1, x2, y2)) {
					case 1:
						if (isEmpty(thatOne)) {
							return 1;
						}
						break;
					case 2:
						if (isExiting(content, x1) && isEmpty(possibleMidway) && isEmpty(thatOne)) {
							return 1;
						}
				}
			} 
		case 1:
			if (areEnemies(thisOne, thatOne) && isAhead(thisOne, x1, x2) && areDiagonallyAligned(x1, y1, x2, y2) && diagonalDistance(x1, y1, x2, y2) == 1) {
				return 1;
			}
		
	}

	return canDiagonalMove(board, x1, y1, x2, y2) | canVerticalMove(board, x1, y1, x2, y2) | canCastle(castling, x2, y2);
}

unsigned char isWithinBounds(int x, int y) {
	return x >= 0 && x < 8 && y >= 0 && y < 8 ? 1 : 0;
}

// moveType: 0 = ocupação, 1 = ataque, ok?
unsigned char canPawnMove(char **board, char *castling, char *enpassant, int moveType, int x1, int y1, int x2, int y2) {	
	if ((areVerticallyAligned(y1, y2) && areHorizontallyAligned(x1, x2)) || !isWithinBounds(x1, y1)) || !isWithinBounds(x2, y2)) {
		return 0;
	}
	char thisOne = board[x1][y1];
	char possibleMidway = board[(x1 + x2) / 2][y1];
	char thatOne = board[x2][y2];
	switch (moveType) {
		case 0:
			if (isAhead(thisOne, x1, x2) && areVerticallyAligned(y1, y2)) {
				switch (straightDistance(x1, y1, x2, y2)) {
					case 1:
						if (isEmpty(thatOne)) {
							return 1;
						}
						break;
					case 2:
						if (isExiting(content, x1) && isEmpty(possibleMidway) && isEmpty(thatOne)) {
							return 1;
						}
				}
			} 
		case 1:
			if (areEnemies(thisOne, thatOne) && isAhead(thisOne, x1, x2) && areDiagonallyAligned(x1, y1, x2, y2) && diagonalDistance(x1, y1, x2, y2) == 1) {
				return 1;
			}
		
	}
	return 0;
}

// ok?
unsigned char canKnightMove(char **board, char *castling, char *enpassant, int moveType, int x1, int y1, int x2, int y2) {
	if ((areVerticallyAligned(y1, y2) && areHorizontallyAligned(x1, x2)) || !isWithinBounds(x1, y1)) || !isWithinBounds(x2, y2)) {
		return 0;
	}
	char thisOne = board[x1][y1];
	char thatOne = board[x2][y2];
	switch (moveType) {
		case 0:
			return canLMove(board, x1, y1, x2, y2) | isEmpty(thatOne) ? 1 : 0;
		case 1:
			return canLMove(board, x1, y1, x2, y2) | areEnemies(thisOne, thatOne) ? 1 : 0;		
	}
}

/*
unsigned char isAttack(char **board, int x1, int y1, int x2, int y2) {
	if (isEnemy(board[x1][y1], board[x2][y2])) {
		return 1;
	}
	return 0;
}
*/

/*
char isCastlingOkay(...) {
	int i;
	
	if (!isKing(board[x1][y1]) || !isRook(board[x2][y2]) || isEnemy(board[x1][y1]) {
		return 0;
	}
	for (i = min(y1, y2) + 1; i < max(y1, y2); ++i) {
		if (!isEmpty(x1, i)) {
			return 0;
		}
	}
	return 1;
}
*/


/*
unsigned char hasWhiteKingsideCastle(...) {
	return !hasWhiteKingMoved(...) | !hasWhiteKingRookMoved (...);
}

unsigned char hasWhiteQueensideCastle(...) {
	return !hasWhiteKingMoved(...) | !hasWhiteQueenRookMoved (...);
}

unsigned char hasBlackKingsideCastle(...) {
	return !hasBlackKingMoved(...) | !hasBlackKingRookMoved (...);
}

unsigned char hasBlackQueensideCastle(...) {
	return !hasBlackKingMoved(...) | !hasBlackQueenRookMoved (...);
}

unsigned char hasWhiteKingMoved(...) {
    int idx = find(list, 7, 4);
	if (list[idx]->hasMoved || isKing(board, 7, 4) && isWhite(board, 7, 4)) {
		return 0;
	}
	return 1;
}

unsigned char hasBlackKingMoved(...) {
    int idx = find(list, 0, 4);
	if (list[idx]->hasMoved || isKing(board, 0, 4) && isBlack(board, 0, 4)) {
		return 0;
	}
	return 1;
}

unsigned char hasWhiteKingRookMoved(...) {
    int idx = find(list, 7, 7);
	if (list[idx]->hasMoved || isRook(board, 7, 7) && isWhite(board, 7, 7)) {
		return 0;
	}
	return 1;
}

unsigned char hasWhiteQueenRookMoved(...) {
    int idx = find(list, 7, 0);
	if (list[idx]->hasMoved || isRook(board, 7, 0) && isWhite(board, 7, 0)) {
		return 0;
	}
	return 1;
}

unsigned char hasBlackKingRookMoved(...) {
    int idx = find(list, 0, 7);
	if (list[idx]->hasMoved || isRook(board, 0, 7) && isBlack(board, 0, 7)) {
		return 0;
	}
	return 1;
}

unsigned char hasBlackQueenRookMoved(...) {
    int idx = find(list, 0, 0);
	if (list[idx]->hasMoved || isRook(board, 0, 0) && isBlack(board, 0, 0)) {
		return 0;
	}
	return 1;
}

unsigned char hasPawnMoved(...) {
	if (isWhite(...) && x != 7 || isBlack(...) && x != 0) {
	}
}
*/



/*
void format(int ply, char piece, int x1, int y1, int x2, int y2, int cap, int enpass) {
	char *str = malloc(80 * sizeof(char));
	if (ply) {
		piece += 'a' - 'A';
	}
	if (piece == 'P') {
	} else {
	}
	str[0]
	printf("%s\n", );

	free(str);
}
*/

unsigned char isDrowned() {
	return 1;
}

unsigned char isWhiteUnderAttack(char **board, struct xpiece *list, char *castling, char *enpassant, int x, int y) {
	int i;

	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && isWhite(board[x][y]) && isBlack(board[list[i].xp][list[i].yp]) && list[i].moveFun(board, castling, enpassant, list[i].xp, list[i].yp, x, y)) {
			return 1;
		}
	}
	return 0;
}

unsigned char isBlackUnderAttack(char **board, struct xpiece *list, char *castling, char *enpassant, int x, int y) {
	int i;

	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && isBlack(board[x][y]) && isWhite(board[list[i].xp][list[i].yp]) && list[i].moveFun(board, castling, enpassant, list[i].xp, list[i].yp, x, y)) {
			return 1;
		}
	}
	return 0;
}

unsigned char canWhiteUnderAttack(char **board, struct xpiece *list, char *castling, char *enpassant, int x, int y) {
	int i, j;

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			if (!isWhite(board[i][j]) && list[i].moveFun(board, castling, enpassant, x, y, i, j) && 
				!isWhiteUnderAttack(board, list, castling, enpassant, i, j)) {
				return 1;
			}
		}
	}
	return 0;
}

unsigned char canBlackUnderAttack(char **board, struct xpiece *list, char *castling, char *enpassant, int x, int y) {
	int i, j;

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			if (!isBlack(board[i][j]) && list[i].moveFun(board, castling, enpassant, x, y, i, j) && 
				!isBlackUnderAttack(board, list, castling, enpassant, i, j)) {
				return 1;
			}
		}
	}
	return 0;
}

unsigned char canWhiteKingReact(char **board, struct xpiece *list, char *castling, char *enpassant) {
	int i, j, k;

	int ans = 0;
	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && 
		    isKing(board[list[i].xp][list[i].yp]) && 
		    isWhite(board[list[i].xp][list[i].yp])) {
			for (j = 0; j < 8; ++j) {
				for (k = 0; k < 8; ++k) {
					if (!isFriend(board[list[i].xp][list[i].yp], board[j][k]) && list[i].moveFun(board, castling, enpassant, list[i].xp, list[i].yp, j, k)) {
						ans |= !canWhiteUnderAttack(board, list, castling, enpassant, j, k);
					}
				}
			}
		}
	}
	return ans;
}

unsigned char canBlackKingReact(char **board, struct xpiece *list, char *castling, char *enpassant) {
	int i, j, k;

	int ans = 0;
	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && 
		    isKing(board[list[i].xp][list[i].yp]) && 
		    isBlack(board[list[i].xp][list[i].yp])) {
			for (j = 0; j < 8; ++j) {
				for (k = 0; k < 8; ++k) {
					if (!isFriend(board[list[i].xp][list[i].yp], board[j][k]) && list[i].moveFun(board, castling, enpassant, list[i].xp, list[i].yp, j, k)) {
						ans |= !canBlackUnderAttack(board, list, castling, enpassant, j, k);
					}
				}
			}
		}
	}
	return ans;
}

unsigned char isWhiteKingInCheck(char **board, struct xpiece *list, char *castling, char *enpassant) {
	int i;

	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && 
		    isKing(board[list[i].xp][list[i].yp]) && 
		    isWhite(board[list[i].xp][list[i].yp])) {
			return isWhiteUnderAttack(board, list, castling, enpassant, list[i].xp, list[i].yp);
		}
	}
	return 0;
}

unsigned char isBlackKingInCheck(char **board, struct xpiece *list, char *castling, char *enpassant) {
	int i;

	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && isKing(board[list[i].xp][list[i].yp]) && isBlack(board[list[i].xp][list[i].yp])) {
			return isBlackUnderAttack(board, list, castling, enpassant, list[i].xp, list[i].yp);
		}
	}
	return 0;
}

struct xpiece *makeList(char **board) {
  int i, j, p = 0;
  char type;
  struct xpiece *list;
  
  list = (struct xpiece *) malloc(NUMPIECES * sizeof(struct xpiece));
  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 8; ++j) {
      if (isEmpty(board[i][j])) {
        continue;
      }
      list[p].isActive = 1;
      list[p].hasMoved = 0;
      list[p].xp = i;
      list[p].yp = j;
		type = toupper(board[i][j]);
      switch (type) {
        case 'K':
          list[p].moveFun = &canKingMove;
          break;
        case 'Q':
          list[p].moveFun = &canQueenMove;
          break;
        case 'B':
          list[p].moveFun = &canBishopMove;
          break;
        case 'N':
          list[p].moveFun = &canKnightMove;
          break;
        case 'R':
          list[p].moveFun = &canRookMove;
          break;
        case 'P':
          list[p].moveFun = &canPawnMove;
      }
      ++p;
    }
  }
  
  return list;
}

unsigned char isEnpassant(char *enpassant, int x, int y) {
	if (strcmp(enpassant, "-") == 0) {
		return 0;
	}
	int tx = xCoordinate(enpassant);
	int ty = yCoordinate(enpassant);
	if (tx == x && ty == y) {
		return 1;
	}
	return 0;
}

void printIt(char **board, struct xpiece *list, char *castling, char *enpassant, int x1, int y1, int x2, int y2) {
	int i;
	
	if (isEnpassant(enpassant, x2, y2)) {
		printf("%c", encodeColumn(y1));
		printf("x");
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;
	}
	if (canCastle(castling, x1, y1)) {
		printf("K");
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;
	}
	if (isToPromote(board, x1, y1, x2, y2)) {
		for (i = 4; i >= 1; --i) {
			printf("%c", encodeColumn(y1));
			if (isAttack(board, x1, y1, x2, y2)) {
				printf("x");
			}
			printf("%c%c", encodeColumn(y2), encodeRow(x2));
			printf("%c", ranks[i]);
			printf("\n");
		}		
		return;
	}
	if (isKing(board[x1][y1])) {
		printf("K");
		if (isAttack(board, x1, y1, x2, y2)) {
			printf("x");
		}
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;
	}
	if (isQueen(board[x1][y1])) {
		printf("Q");
		if (isAttack(board, x1, y1, x2, y2)) {
			printf("x");
		}
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;
	}
	if (isRook(board[x1][y1])) {
		printf("R");
		if (isAttack(board, x1, y1, x2, y2)) {
			printf("x");
		}
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;
	}
	if (isKnight(board[x1][y1])) {
		printf("N");
		if (isAttack(board, x1, y1, x2, y2)) {
			printf("x");
		}
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;
	}
	if (isBishop(board[x1][y1])) {
		printf("B");
		if (isAttack(board, x1, y1, x2, y2)) {
			printf("x");
		}
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;		
	}
	if (isPawn(board[x1][y1])) {
		printf("%c", encodeColumn(y1));
		if (isAttack(board, x1, y1, x2, y2)) {
			printf("x");
		}
		printf("%c%c", encodeColumn(y2), encodeRow(x2));
		printf("\n");
		return;		
	}
}

// imprime os movimentos
void printMoveTo(char **board, struct xpiece *list, char *castling, char *enpassant, char type, char player, int x, int y) {
	int i;
  
	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && isPlayer(board[list[i].xp][list[i].yp]) == player && areOfSameType(board[list[i].xp][list[i].yp], type) && list[i].moveFun(board, castling, enpassant, list[i].xp, list[i].yp, x, y)) {
			if (player == 'w' && !isKing(board[list[i].xp][list[i].yp]) && isWhiteKingInCheck(board, list, castling, enpassant)) {
				continue;
			}
			if (player == 'b' && !isKing(board[list[i].xp][list[i].yp]) && isBlackKingInCheck(board, list, castling, enpassant)) {
				continue;
			}
			printIt(board, list, castling, enpassant, list[i].xp, list[i].yp, x, y);
		}
	}
}

// lista movimentos
void listMovesTo(char **board, struct xpiece *list, char *castling, char *enpassant, char player) {
	int i, j, k, m, n;
  
	for (k = 0; k < NUMRANKS; ++k) {
		for (n = 0; n < 8; ++n) {
			for (m = 7; m >= 0; --m) {
				for (j = 0; j < 8; ++j) {
					for (i = 7; i >= 0; --i) {
//						printMoveTo(board, list, castling, enpassant, ranks[k], player, i, j);
						if (player == 'w' && !isKing(board[i][j]) && isWhiteKingInCheck(board, list, castling, enpassant)) {
//							continue;
						}
						if (player == 'b' && !isKing(board[i][j]) && isBlackKingInCheck(board, list, castling, enpassant)) {
//							continue;
						}
						if (isEmpty(board[i][j])) {
							continue;
						}
//						printf("ok\n");
//						printf(">> %c %c\n", board[i][j], board[m][n]);
						int idx1 = find(list, i, j);
//						int idx2 = find(list, m, n);
						if (list[idx1].isActive && isPlayer(board[i][j]) == player
						    && areOfSameType(board[i][j], ranks[k])
						    && !isFriend(board[i][j], board[m][n])
						    && list[idx1].moveFun(board, castling, enpassant, i, j, m, n)) {
//							printf("ok\n");
							printIt(board, list, castling, enpassant, i, j, m, n);
						}
//		if (list[i].isActive && isPlayer(board[list[i].xp][list[i].yp]) == player && areOfSameType(board[list[i].xp][list[i].yp], type) && list[i].moveFun(board, castling, enpassant, list[i].xp, list[i].yp, x, y)) {
					}
				}
			}
		}
	}
} 

/*
unsigned char checkMoveTo(char **board, struct xpiece *list, char type, int x1, int y1, int x2, int y2) {
	int i;
  
	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i]->isActive && areOfSameType(board[list[i]->xp][list[i]->yp], type) && list[i]->xp == x1 && list[i]->yp == y1 && list[i]->moveFun(..., x1, y1, x2, y2)) {
			return 1;
		}
	}
	return 0;
}
*/

void takePiece(char **board, struct xpiece *list, int x, int y) {
	int i;
	
	for (i = 0; i < NUMPIECES; ++i) {
		if (list[i].isActive && list[i].xp == x && list[i].yp == y) {
			list[i].isActive = 0;
			board[x][y] = '.';
		}
	}
}

void movePieceTo(char **board, struct xpiece *list, int x1, int y1, int x2, int y2) {
	int idx = find(list, x1, y1);
	
	if (list[idx].isActive) {
		list[idx].xp = x2;
		list[idx].yp = y2;
		board[x2][y2] = board[x1][y1];
		board[x1][y1] = '.';
		list[idx].hasMoved = 1;
	}
	
}

void readIntoBoard(char **board, char *config) {
	int i, j, p = 0, idx = 0;
	char ch;
	
	for (i = 0; i < 8; ++i) {
		while (config[idx] != 0 && (ch = config[idx++]) != '/') {
			if (ch >= '1' && ch <= '8') {
				for (j = 0; j < ch - '0'; ++j) {
					board[p / 8][p % 8] = '.';
					++p;
				}
			} else {
				if (ch >= 'A' && ch <= 'Z') {
					board[p / 8][p % 8] = ch;
				} else {
					if (ch >= 'a' && ch <= 'z') {
						board[p / 8][p % 8] = ch;
					}
				}
				++p;
			}
		}
	}
}

void releaseBoard(char **board) {
	free(board);
}

void encodeBoard(char *boardConfig, char **board) {
	int i, j, p = 0, acc = 0;
	
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			if (!isEmpty(board[i][j])) {
				if (acc > 0) {
					boardConfig[p++] = acc + '0';
					acc = 0;
				}
				boardConfig[p++] = board[i][j];
			} else {
				++acc;
			}
		}
		if (acc > 0) {
			boardConfig[p++] = acc + '0';
		}
		acc = 0;
		if (i < 7) {
			boardConfig[p++] = '/';
		}
	}
	boardConfig[p] = 0;
}

unsigned char lacksMaterial(char **board) {
	int i, j, k, total = 0;
	int *count = (int *) malloc(2 * NUMRANKS * sizeof(int));

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j) {
			for (k = 0; k < NUMRANKS; ++k) {
				if (isWhite(board[i][j])) {
					if (board[i][j] == ranks[k]) {
						++count[k];
						++total;
					}
				}
				if (isBlack(board[i][j])) {
					if (toupper(board[i][j]) == ranks[k]) {
						++count[k + NUMRANKS];
						++total;
					}
				}
			}
		}
	}
	if (total == 2) {
		if (count[5] == 1 && count[11] == 1) {
			return 1;
		}
	}
	if (total == 3 && ((count[5] == 1 && count[11] == 1 && count[1] == 1) || (count[5] == 1 && count[11] == 1 && count[7] == 1) || (count[5] == 1 && count[11] == 1 && count[2] == 1) || (count[5] == 1 && count[11] == 1 && count[8] == 1))) {
		return 1;
	}
	return 0;
}


int main() {
	int i, half, full;
	char player, *command, *boardConfig, *castling, *enpassant, **board;
	struct xpiece *list;
	
	boardConfig = (char *) malloc(80 * sizeof(char));
	castling = (char *) malloc(80 * sizeof(char));
	enpassant = (char *) malloc(80 * sizeof(char));
	board = (char **) malloc(8 * sizeof(char *));
	for (i = 0; i < 8; ++i) {
		board[i] = (char *) malloc(8 * sizeof(char));
	}
	command = (char *) malloc(80 * sizeof(char));
	
	scanf("%s %c %s %s %d %d\n", boardConfig, &player, castling, enpassant, &half, &full);
	
	readIntoBoard(board, boardConfig);

	/*
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 8; ++j)
			printf("%c", board[i][j]);
		printf("\n");
	}
	printf("\n");
	*/
	
	list = makeList(board);


	free(boardConfig);
	free(castling);
	free(enpassant);
	free(*board);
	free(board);
	free(list);
	free(command);

	return 0;
}
