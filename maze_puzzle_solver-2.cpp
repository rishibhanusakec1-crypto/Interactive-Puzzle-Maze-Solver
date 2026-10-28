// DSA Microproject - Interactive Puzzle & Maze Solver
// Raj Katariya (22), Parth Katarmal (23), Rishi Bhanu (5)
// Maze solving using DFS + backtracking, and Sudoku using backtracking
//
// compile:  g++ maze_puzzle_solver.cpp -o solver
// run:      ./solver         (menu)
//           ./solver test    (runs all test cases)

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ---------- maze part ----------

vector<string> maze;        // the maze rows as typed
int vis[100][100];          // 0 = not visited, 1 = on path, 2 = dead end
int rows, cols;
int sr, sc, gr, gc;         // start and goal position
int pathR[10000], pathC[10000];
int pathLen;
long long calls, backs;     // counters for analysis
int maxDepth;
bool usePrune = true;       // true = keep dead ends marked
bool showSteps = false;

// move order: down, right, up, left
int dr[4] = {1, 0, -1, 0};
int dc[4] = {0, 1, 0, -1};

bool isSafe(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
    if (maze[r][c] == '#') return false;
    if (vis[r][c] != 0) return false;
    return true;
}

bool dfs(int r, int c) {
    calls++;
    vis[r][c] = 1;
    pathR[pathLen] = r;
    pathC[pathLen] = c;
    pathLen++;
    if (pathLen > maxDepth) maxDepth = pathLen;

    if (showSteps)
        cout << "  Move -> (" << r << "," << c << ")  depth=" << pathLen << endl;

    if (r == gr && c == gc) return true;

    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (isSafe(nr, nc)) {
            if (dfs(nr, nc)) return true;
        }
    }

    // nothing worked from here, so go back
    pathLen--;
    if (usePrune) vis[r][c] = 2;
    else vis[r][c] = 0;
    backs++;

    if (showSteps)
        cout << "  Back <- (" << r << "," << c << ")  dead end" << endl;
    return false;
}

// finds S and G in the maze, returns false if any one is missing
bool findStartGoal() {
    rows = maze.size();
    cols = maze[0].size();
    sr = sc = gr = gc = -1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == 'S') { sr = i; sc = j; }
            if (maze[i][j] == 'G') { gr = i; gc = j; }
        }
    }
    return (sr != -1 && gr != -1);
}

bool solveMaze() {
    rows = maze.size();
    cols = maze[0].size();
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            vis[i][j] = 0;
    pathLen = 0;
    calls = backs = 0;
    maxDepth = 0;
    return dfs(sr, sc);
}

void printMaze(bool solved) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            char ch = maze[i][j];
            if (ch == '.' && solved) {
                for (int k = 0; k < pathLen; k++)
                    if (pathR[k] == i && pathC[k] == j) ch = '*';
            }
            cout << ch << " ";
        }
        cout << endl;
    }
}

// runs one maze and prints everything
void runMaze(string title, vector<string> m) {
    maze = m;
    cout << "\n" << title << endl;
    if (!findStartGoal()) {
        cout << "Error: maze needs both S and G" << endl;
        return;
    }
    bool ok = solveMaze();
    printMaze(ok);
    if (ok) cout << "Result : SOLVED, path length = " << pathLen << " cells" << endl;
    else cout << "Result : NO SOLUTION" << endl;
    cout << "Stats  : calls=" << calls << ", backtracks=" << backs
         << ", max depth=" << maxDepth << endl;
}

// ---------- sudoku part ----------

int board[9][9];
long long placed, undone;

bool canPlace(int r, int c, int num) {
    for (int i = 0; i < 9; i++) {
        if (board[r][i] == num) return false;   // same row
        if (board[i][c] == num) return false;   // same column
    }
    int br = (r / 3) * 3;
    int bc = (c / 3) * 3;
    for (int i = br; i < br + 3; i++)
        for (int j = bc; j < bc + 3; j++)
            if (board[i][j] == num) return false;   // same box
    return true;
}

bool solveSudoku() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board[r][c] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (canPlace(r, c, num)) {
                        board[r][c] = num;
                        placed++;
                        if (solveSudoku()) return true;
                        board[r][c] = 0;     // wrong choice, undo it
                        undone++;
                    }
                }
                return false;    // no number fits here
            }
        }
    }
    return true;    // no empty cell left
}

// checks that the given clues do not already clash
bool cluesOk() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board[r][c] != 0) {
                int v = board[r][c];
                board[r][c] = 0;
                bool ok = canPlace(r, c, v);
                board[r][c] = v;
                if (!ok) return false;
            }
        }
    }
    return true;
}

void loadBoard(string s) {
    for (int i = 0; i < 81; i++) {
        if (s[i] == '.') board[i / 9][i % 9] = 0;
        else board[i / 9][i % 9] = s[i] - '0';
    }
}

void printBoard() {
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0 && i != 0) cout << "------+-------+------" << endl;
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) cout << ".";
            else cout << board[i][j];
            if (j == 2 || j == 5) cout << " | ";
            else if (j != 8) cout << " ";
        }
        cout << endl;
    }
}

// ---------- tests ----------

string easy = "530070000600195000098000060800060003400803001700020006060000280000419005000080079";

void showNames() {
    cout << "Made by: Raj Katariya (22), Parth Katarmal (23), Rishi Bhanu (5)" << endl;
}

void runTests() {
    showNames();
    cout << "============================================================" << endl;
    cout << "MAZE TEST CASES" << endl;
    cout << "============================================================" << endl;

    runMaze("T1: 5x5 maze with solution",
            {"S.#..", ".##.#", "....#", "#.##.", "#...G"});
    runMaze("T2: maze with NO solution",
            {"S.#..", "..#..", "###..", "...#G", "....."});
    runMaze("T3: start boxed in by walls",
            {"#S#..", "###..", "....G"});

    // start and goal on the same cell, cannot type this in the S/G format
    // so it is set by hand
    maze = {"..", ".."};
    rows = 2; cols = 2;
    sr = gr = 0; sc = gc = 0;
    bool same = solveMaze();
    cout << "\nT3b: start equals goal -> " << (same ? "SOLVED" : "FAILED")
         << ", path length = " << pathLen << ", calls=" << calls << endl;

    runMaze("T4: long winding maze (8x8)",
            {"S.#.....", ".##.###.", "....#...", "###.#.##",
             "....#..#", ".####.##", "......#.", "#####..G"});
    runMaze("T5: open 10x10 grid",
            {"S.........", "..........", "..........", "..........", "..........",
             "..........", "..........", "..........", "..........", ".........G"});

    cout << "\n============================================================" << endl;
    cout << "PRUNED vs PURE BACKTRACKING (goal sealed off)" << endl;
    cout << "============================================================" << endl;
    for (int n = 3; n <= 5; n++) {
        // open n x n grid, goal in the corner with walls around it
        vector<string> m;
        m.push_back("S" + string(n - 1, '.'));
        for (int i = 1; i < n - 1; i++) m.push_back(string(n, '.'));
        m[n - 2][n - 1] = '#';
        m.push_back(string(n - 2, '.') + "#G");

        maze = m;
        findStartGoal();
        usePrune = true;
        solveMaze();
        long long c1 = calls;
        usePrune = false;
        solveMaze();
        long long c2 = calls;
        usePrune = true;
        cout << n << "x" << n << ": pruned calls=" << c1 << " | pure calls=" << c2 << endl;
    }

    cout << "\n============================================================" << endl;
    cout << "SUDOKU TEST CASES" << endl;
    cout << "============================================================" << endl;

    loadBoard(easy);
    cout << "\nS1: Standard puzzle - input" << endl;
    printBoard();
    placed = undone = 0;
    if (solveSudoku()) {
        cout << "\nSolved:" << endl;
        printBoard();
    } else {
        cout << "\nNo solution" << endl;
    }
    cout << "Stats: placements=" << placed << ", backtracks=" << undone << endl;

    loadBoard(easy);
    board[0][2] = 5;    // 5 is already in row 1, so this clashes
    cout << "\nS2: Invalid puzzle (duplicate clue 5 in row 1)" << endl;
    cout << "Consistent clues? " << (cluesOk() ? "true" : "false") << endl;

    loadBoard(easy);
    board[0][2] = 1;
    board[1][1] = 2;
    placed = undone = 0;
    bool ok = solveSudoku();
    cout << "\nS3: Consistent but unsolvable puzzle -> "
         << (ok ? "solved" : "NO SOLUTION")
         << " (placements=" << placed << ", backtracks=" << undone << ")" << endl;
}

// ---------- menu ----------

void mazeFromUser(bool trace) {
    cout << "Enter maze rows (S start, G goal, # wall, . open). Blank line to finish:" << endl;
    maze.clear();
    string line;
    while (getline(cin, line)) {
        if (line.empty()) break;
        maze.push_back(line);
    }
    if (maze.empty()) {
        cout << "No maze entered." << endl;
        return;
    }
    // all rows should be the same length
    for (int i = 1; i < (int)maze.size(); i++) {
        if (maze[i].size() != maze[0].size()) {
            cout << "Error: all rows must have the same length." << endl;
            return;
        }
    }
    rows = maze.size();
    cols = maze[0].size();
    if (rows > 100 || cols > 100) {
        cout << "Error: maze too big (max 100 x 100)." << endl;
        return;
    }
    if (!findStartGoal()) {
        cout << "Error: maze needs both S and G." << endl;
        return;
    }

    showSteps = trace;
    bool ok = solveMaze();
    showSteps = false;

    if (ok) {
        cout << "\nSolution path:";
        for (int i = 0; i < pathLen; i++)
            cout << " (" << pathR[i] << "," << pathC[i] << ")";
        cout << endl;
        printMaze(true);
    } else {
        cout << "\nNo solution exists for this maze." << endl;
    }
    cout << "Stats: calls=" << calls << ", backtracks=" << backs
         << ", max depth=" << maxDepth << endl;
}

void sudokuFromUser() {
    cout << "Enter 9 rows of 9 digits (use 0 or . for empty):" << endl;
    string all = "";
    for (int i = 0; i < 9; i++) {
        string row;
        cout << "Row " << i + 1 << ": ";
        getline(cin, row);
        while (row.size() < 9) row += "0";
        all += row.substr(0, 9);
    }
    for (int i = 0; i < 81; i++) {
        if (all[i] != '.' && (all[i] < '0' || all[i] > '9')) {
            cout << "Error: only digits and . are allowed." << endl;
            return;
        }
    }
    loadBoard(all);
    if (!cluesOk()) {
        cout << "Invalid puzzle: clues conflict." << endl;
        return;
    }
    placed = undone = 0;
    if (solveSudoku()) printBoard();
    else cout << "No solution exists." << endl;
    cout << "Stats: placements=" << placed << ", backtracks=" << undone << endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "test") {
        runTests();
        return 0;
    }

    string choice;
    while (true) {
        showNames();
        cout << "\n==== Interactive Puzzle & Maze Solver ====" << endl;
        cout << "1. Solve a maze (enter your own)" << endl;
        cout << "2. Solve a maze (step-by-step trace)" << endl;
        cout << "3. Solve a Sudoku puzzle" << endl;
        cout << "4. Run all built-in test cases" << endl;
        cout << "5. Exit" << endl;
        cout << "Choice: ";
        if (!getline(cin, choice)) break;

        if (choice == "1") mazeFromUser(false);
        else if (choice == "2") mazeFromUser(true);
        else if (choice == "3") sudokuFromUser();
        else if (choice == "4") runTests();
        else if (choice == "5") {
            cout << "Goodbye!" << endl;
            break;
        }
        else cout << "Invalid choice." << endl;
    }
    return 0;
}
