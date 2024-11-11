#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <GL/glut.h>
#include <SOIL/SOIL.h>

using namespace std;

const int rows = 5;
const int cols = 5;
const int num_mines = 6;
const int num_points = 6;
const int cellSize = 100;
int rowTotal[5] = {0,0,0,0,0};
int colTotal[5] = {0,0,0,0,0};
int rowBomb[5] = {0,0,0,0,0};
int colBomb[5] = {0,0,0,0,0};
bool tryAgain = false;
bool gameWon = false;
int totalScore = 1;

GLuint textures[rows][cols], texAlien, againTry, wonGame;

vector<vector<char>> mines_grid(rows, vector<char>(cols, ' '));
vector<vector<int>> picross_grid(rows, vector<int>(cols, 1));
vector<vector<bool>> revealed(rows, vector<bool>(cols, false));

void placeMines() {
    srand(time(NULL));
    int count = 0;
    while (count < num_mines) {
        int x = rand() % rows;
        int y = rand() % cols;
        if (mines_grid[y][x] != '*') {
            mines_grid[y][x] = '*';
            count++;
            rowBomb[x]++;
            colBomb[y]++;
        }
    }
}

void generatePicrossNumbers(){

    int count = 0;
    while(count < num_points){
        int x = rand() % rows;
        int y = rand() % cols;
        if (mines_grid[y][x] != '*' ) {
            picross_grid[y][x] = rand()%2 + 2;
            count++;
        }
    }
    for (int i = 0; i < cols; ++i){
        for (int j = 0; j < rows; ++j){
            if (mines_grid[i][j] == '*'){
                    picross_grid[i][j] = 0;
                }
                // add to row and col total
                rowTotal[j] += picross_grid[i][j];
                colTotal[i] += picross_grid[i][j];
            }
        }
}

//For Debug Purposes displays answer to game
void printPicrossGrid() {

    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            if (mines_grid[i][j] == '*') {
                cout << "* ";
            }
            else {
                cout << picross_grid[i][j] << " ";
            }
        }
        cout <<"| " << colTotal[i] << " ";
        cout << colBomb[i] << " ";
        cout << endl;
    }
    cout <<"----------"<< endl;
    for(int j = 0; j < cols; ++j){
        cout  << rowTotal[j] << " ";
    }
    cout << endl;
    for(int j = 0; j < cols; ++j){
        cout  << rowBomb[j] << " ";
    }
}

void loadTextures(){
     for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            string filename = "pngTextures/" + to_string(i * rows + j + 1) + ".png";
            textures[i][j] = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        }
    }
            texAlien = SOIL_load_OGL_texture("pngTextures/GuiAlien.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
            againTry = SOIL_load_OGL_texture("pngTextures/Tryagainn.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
            wonGame = SOIL_load_OGL_texture("pngTextures/AWinnerIsYou.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
}


void drawTexture(GLuint texture, float x, float y, float width, float height) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + height);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void drawGrid() {
// GRID
    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            float x = j * cellSize;
            float y = i * cellSize;
            if (revealed[i][j]) {
                //PRINTS ALIEN TEXTURE ON BOMBS
                if (mines_grid[i][j] == '*') {
                    drawTexture(texAlien, x, y, cellSize, cellSize);
                }
                //PRINTS THE NUMBERS UNDER
                else if (picross_grid[i][j] > 0){
                    glRasterPos2f(x + cellSize * 0.4, y + cellSize * 0.5);
                    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + picross_grid[i][j]);
                }
            }
                //PRINTS TEXTURE MAP ARRAY
                else {
                drawTexture(textures[i][j], x, y, cellSize, cellSize);
            }
        }

    }
                // GAMEOVER, TRY AGAIN, GAME WON

            if (tryAgain) {
                if (gameWon) {
                    drawTexture(wonGame, 150, 150, 250, 250);
        }
                else {
                    drawTexture(againTry, 150, 150, 250, 250);
        }
                //string scoreText = "TOTAL SCORE: " + to_string(totalScore);
               // glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
               // glRasterPos2f(120, 170); // Position the text
               // for (char c : scoreText) {
                //glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c); // Draw each character of the score text
           // }
        }
    }


void revealCell(int x, int y) {
    bool bombHit = false;
    if (!revealed[y][x]) {
        revealed[y][x] = true;
        if (mines_grid[y][x] == '*') {
            tryAgain = true;
        } else {
            totalScore *= picross_grid[y][x]; // Increment total score by the value of the revealed cell
            if (totalScore >= 30) {
                bool bombHit = false;

            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    if (revealed[i][j] && mines_grid[i][j] == '*') {
                        bombHit = true;
                        break;
                        }
                    }
                    if (bombHit) {
                        tryAgain = true;
                        break;
                    }

                    if (!bombHit && totalScore >= 30) {
                        tryAgain = true;
                        gameWon = true;
                        break;
                    }
                }
            }
        }
    }
}

void restartGame() {
    placeMines();
    generatePicrossNumbers();
    revealed = vector<vector<bool>>(rows, vector<bool>(cols, false));
    tryAgain = false;
    totalScore = 0;
}

void mouseClick(int button, int state, int x, int y) {
     if (!tryAgain && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int gridX = x / cellSize;
        int gridY = y / cellSize;
        revealCell(gridX, gridY);
    } else if (tryAgain && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float buttonSize = 200;
        float buttonX = (cols * cellSize - buttonSize) / 2;
        float buttonY = (rows * cellSize - buttonSize) / 2;
        if (x >= buttonX && x <= buttonX + buttonSize && y >= buttonY && y <= buttonY + buttonSize) {
            restartGame();
        }
    }
    glutPostRedisplay();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    glFlush();
}

void init() {
    loadTextures();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, cols * cellSize, rows * cellSize, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


int main(int argc, char** argv) {

    placeMines();
    generatePicrossNumbers();
    printPicrossGrid();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(cols * cellSize, rows * cellSize);
    glutCreateWindow("Space Minesweeper x Picross");
    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    init();
    glutMainLoop();

    return 0;

}
