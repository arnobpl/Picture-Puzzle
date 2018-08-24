/*
Picture Puzzle
By Arnob Paul
*/


#include "Picture Puzzle Header.hpp"
#include <stdarg.h>



#define imageBlockSize 120
#define numberOFimages 12
#define maxImageBlockRow 6
#define maxImageBlockCol 6
#define menuButtonX 300
#define menuButtonY 50



//Only function names declared here for function-function linkage
void introDraw();
void introKeyboard(unsigned char *key);
void introMouse(int *button, int *state, int *mx, int *my);
void mainMenuDraw();
void mainMenuKeyboard(unsigned char *key);
void mainMenuMouse(int *button, int *state, int *mx, int *my);
void gameStateDraw();
void gameStateKeyboard(unsigned char *key);
void gameStateMouse(int *button, int *state, int *mx, int *my);
void promptUserLoseCurrentGameDraw();
void promptUserLoseCurrentGameKeyboard(unsigned char *key);
void promptUserLoseCurrentGameMouse(int *button, int *state, int *mx, int *my);
void resultDraw();
void resultKeyboard(unsigned char *key);
void resultMouse(int *button, int *state, int *mx, int *my);
void fameDraw();
void fameKeyboard(unsigned char *key);
void fameMouse(int *button, int *state, int *mx, int *my);
void promptUserNameDraw();
void promptUserNameKeyboard(unsigned char *key);
void promptUserNameMouse(int *button, int *state, int *mx, int *my);
void howToPlayDraw();
void howToPlayKeyboard(unsigned char *key);
void howToPlayMouse(int *button, int *state, int *mx, int *my);
void creditDraw();
void creditKeyboard(unsigned char *key);
void creditMouse(int *button, int *state, int *mx, int *my);
void exitDraw();
void exitKeyboard(unsigned char *key);
void exitMouse(int *button, int *state, int *mx, int *my);



int halfOFscreenX;
int halfOFscreenY;
const int halfOFimageBlockSize = imageBlockSize >> 1;
const int halfOFimages = numberOFimages >> 1;
char itoaBuffer[20];	//used for "_itoa" function
char menuStrings[6][20];	//for storing multiple strings in menu buttons
int timerSec, timerStarted = 0, totalTime;

int introFinished = 0;

int mainMenuButtonsY[6];
int mainMenuButtonsX;
int menuButtonPressed = -1;	//value can be from -1 to 5; -1 means no menu key clicked

int level = 0, menuButtonNumbers = 5;	//level=0 means game has not started
const int gameLevelsElements = maxImageBlockRow * maxImageBlockCol + 3;
char gameLevels[maxGameLevels][gameLevelsElements];	//arrangement of all game levels
char currentGameLevelForRandom[gameLevelsElements - 3];	//a temporary list used for randomly assign current game level
char currentGameLevel[gameLevelsElements];	//arrangement of current game level
int currentGameLevelGraphicsX[maxImageBlockRow][maxImageBlockCol];	//saves every image block's X
int currentGameLevelGraphicsY[maxImageBlockRow][maxImageBlockCol];	//saves every image block's Y
int currentGameRow, currentGameCol, currentGameTotal;
int gameBoardX;
int openImageBlockAnimation, openImageBlockNumber, openImageBlockPictures[2], openImageBlockX[2], openImageBlockY[2], openImageBlockSerial;
int closeImageBlockAnimation, closeImageBlockNumber, closeImageBlockPictures[2], closeImageBlockX[2], closeImageBlockY[2];
int imageBlockMatched, matchedImageBlockPicture, matchedImageBlockX[2], matchedImageBlockY[2], matchedImageBlockXstep[2], matchedImageBlockYstep[2];
int notAnimatingImageBlock;
int pauseButtonBottom;
int pauseButtonPressed = 0;


int gameElementsVerticalX[halfOFimages];
int gameElementsVerticalYconst[4];
int gameElementsHorizontalY[halfOFimages];
int gameElementsHorizontalXconst[4];



//Special initial functions
void gameLevelsBuilder(int row, int col, int levelIndex) {	//last 3 elements are row, col, number of blocks respectively
															/*this function set picture numbers for 2 or more times*/
	int i = 0, j = 2, k, blocksNumber = row * col;	//here j=2 is binary 10; after right shifting j=1 so that 1,1,2,2,... sequence
	for (; i < blocksNumber; i++, j++) {
		k = j >> 1;
		if (k > numberOFimages) {	//if distinct pictures are not available, then again 1,1,2,2,... sequence
			j = 2; k = 1;
		}
		gameLevels[levelIndex][i] = k;
	}
	gameLevels[levelIndex][gameLevelsElements - 3] = row;
	gameLevels[levelIndex][gameLevelsElements - 2] = col;
	gameLevels[levelIndex][gameLevelsElements - 1] = blocksNumber;
}
void timer() {
	if (timerStarted) timerSec++;
}



int main() {
	//place your own initialization codes here
	int i = 0, j;


	//setting up screen resolution, visual effect and hall of fame
	readData();



	//setting up fundamental dependant values
	frameTime = 1000 / fps;
	halfOFscreenX = screenX >> 1;
	halfOFscreenY = screenY >> 1;
	mainMenuButtonsX = halfOFscreenX - (menuButtonX >> 1);
	gameBoardX = screenX * 0.75;
	pauseButtonBottom = screenY - menuButtonY;



	//setting up menuStrings for first time
	strcpy(menuStrings[0], "Exit Game");
	strcpy(menuStrings[1], "Credit");
	strcpy(menuStrings[2], "How to Play");
	strcpy(menuStrings[3], "Hall of Fame");
	strcpy(menuStrings[4], "New Game");
	strcpy(menuStrings[5], "Continue Game");


	//setting up game levels, must be even numbers
	gameLevelsBuilder(3, 4, 0);	//level=1: row=3, col=4
	gameLevelsBuilder(4, 4, 1);	//level=2: row=4, col=4
	gameLevelsBuilder(4, 5, 2);	//level=3: row=4, col=5
	gameLevelsBuilder(4, 6, 3);	//level=4: row=4, col=6
	gameLevelsBuilder(5, 6, 4);	//level=5: row=5, col=6
	gameLevelsBuilder(6, 6, 5);	//level=6: row=6, col=6


								//setting up intro display orientation (game elements Vertical)
	int increment = (screenX - imageBlockSize * halfOFimages) / halfOFimages + imageBlockSize;
	for (i = (increment - imageBlockSize) >> 1, j = 0; j < halfOFimages; i += increment, j++) {
		gameElementsVerticalX[j] = i;
	}
	int amplitude = screenY / 10;
	gameElementsVerticalYconst[0] = amplitude;
	gameElementsVerticalYconst[1] = amplitude << 1;
	gameElementsVerticalYconst[2] = screenY - amplitude - imageBlockSize;
	gameElementsVerticalYconst[3] = screenY - (amplitude << 1) - imageBlockSize;


	//setting up mainMenu display orientation (game elements Horizontal)
	increment = (screenY - imageBlockSize * halfOFimages) / halfOFimages + imageBlockSize;
	for (i = (increment - imageBlockSize) >> 1, j = 0; j < halfOFimages; i += increment, j++) {
		gameElementsHorizontalY[j] = i;
	}
	amplitude = screenX / 10;
	gameElementsHorizontalXconst[0] = amplitude + 10;
	gameElementsHorizontalXconst[1] = (amplitude << 1) + 10;
	gameElementsHorizontalXconst[2] = screenX - amplitude - imageBlockSize - 10;
	gameElementsHorizontalXconst[3] = screenX - (amplitude << 1) - imageBlockSize - 10;

	const int buttonSpaces = screenY / 20;
	increment = menuButtonY + buttonSpaces;
	for (i = screenY * 0.1 - (menuButtonY >> 1), j = 0; j < 6; i += increment, j++) {
		mainMenuButtonsY[j] = i;
	}


	//setting up function pointers for first time
	doThisDraw = introDraw;
	doThisKeyboard = introKeyboard;
	doThisMouse = introMouse;
	//doThisDraw = mainMenuDraw;
	//doThisKeyboard = mainMenuKeyboard;
	//doThisMouse = mainMenuMouse;


	iSetTimer(1000, timer);
	srand(time(NULL));	//needed for "random" function; the pseudo-random number generator is initialized using the argument passed as seed
	iInitialize(screenX, screenY, gameName, 1, fullscreen);
	return 0;
}



//Special functions
int random(int range) {	//returns number from 0 to (range-1)
	return rand() % range;
}
void loadCurrentGameLevel(int levelIndex) {
	levelIndex--;

	//copy from gameLevels to currentGameLevelForRandom
	int i = 0;
	int blocksNumber = gameLevels[levelIndex][gameLevelsElements - 1];
	for (; i < blocksNumber; i++) {
		currentGameLevelForRandom[i] = gameLevels[levelIndex][i];
	}
	currentGameLevel[gameLevelsElements - 3] = gameLevels[levelIndex][gameLevelsElements - 3];
	currentGameLevel[gameLevelsElements - 2] = gameLevels[levelIndex][gameLevelsElements - 2];
	currentGameLevel[gameLevelsElements - 1] = blocksNumber;

	//load currentGameLevel using currentGameLevelForRandom
	int j;
	int lastIndex = blocksNumber - 1;
	for (i = 0; i < lastIndex; i++) {
		j = random(blocksNumber);	//random
		currentGameLevel[i] = currentGameLevelForRandom[j];
		currentGameLevelForRandom[j] = currentGameLevelForRandom[--blocksNumber];
	}
	currentGameLevel[lastIndex] = currentGameLevelForRandom[0];

	//load every image block's X, Y
	int row, col;
	int imageBlockStartLeft = (gameBoardX - imageBlockSize * gameLevels[levelIndex][gameLevelsElements - 2]) >> 1;
	int imageBlockStartBottom = (screenY - imageBlockSize * gameLevels[levelIndex][gameLevelsElements - 3]) >> 1;
	int maxImageBlockLeft = imageBlockStartLeft + imageBlockSize * gameLevels[levelIndex][gameLevelsElements - 2];
	int maxImageBlockBottom = imageBlockStartBottom + imageBlockSize * gameLevels[levelIndex][gameLevelsElements - 3];
	currentGameRow = currentGameLevel[gameLevelsElements - 3];
	currentGameCol = currentGameLevel[gameLevelsElements - 2];
	currentGameTotal = currentGameRow * currentGameCol;
	for (i = imageBlockStartLeft, col = 0; i < maxImageBlockLeft; i += imageBlockSize, col++) {
		for (j = imageBlockStartBottom, row = 0; j < maxImageBlockBottom; j += imageBlockSize, row++) {
			currentGameLevelGraphicsX[row][col] = i;
			currentGameLevelGraphicsY[row][col] = j;
		}
	}

	//set no opened image block
	openImageBlockNumber = 0;

	//set no closing image block
	closeImageBlockNumber = 0;

	//set no image block matched
	imageBlockMatched = 0;

	//set no image block animating
	notAnimatingImageBlock = 1;

	//start timer
	timerSec = 0;
	timerStarted = 1;

	//for the first level
	if (!levelIndex) totalTime = 0;
}
char *timerText(int timerSec) {
	//convert timerSec to "MM : SS"
	static char timeShowText[8];
	strcpy(timeShowText, _itoa(timerSec / 60, itoaBuffer, 10));
	if (!timeShowText[1]) {
		timeShowText[1] = timeShowText[0];
		timeShowText[0] = '0';
		timeShowText[2] = '\0';
		strcat(timeShowText, " : ");
	}
	strcat(timeShowText, _itoa(timerSec % 60, itoaBuffer, 10));
	if (!timeShowText[6]) {
		timeShowText[6] = timeShowText[5];
		timeShowText[5] = '0';
		timeShowText[7] = '\0';
	}
	return timeShowText;
}


//Animation functions
void gameElementsMoveVertical() {
	iSetColor(0, 162, 232);
	iFilledRectangle(0, 0, screenX, screenY);
	static int gameElementsVerticalY[4] = { gameElementsVerticalYconst[0], gameElementsVerticalYconst[1], gameElementsVerticalYconst[2], gameElementsVerticalYconst[3] };
	static int gameElementsVerticalYstep[2] = { 4, -4 };
	int i;
	for (i = 0; i < halfOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsVerticalX[i], gameElementsVerticalY[0], fileAddressPath);
	}
	for (i = 1; i < halfOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsVerticalX[i], gameElementsVerticalY[1], fileAddressPath);
	}
	for (i = halfOFimages; i < numberOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsVerticalX[i - halfOFimages], gameElementsVerticalY[2], fileAddressPath);
	}
	for (i = halfOFimages + 1; i < numberOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsVerticalX[i - halfOFimages], gameElementsVerticalY[3], fileAddressPath);
	}
	gameElementsVerticalY[0] += gameElementsVerticalYstep[0];
	gameElementsVerticalY[3] += gameElementsVerticalYstep[0];
	gameElementsVerticalY[1] += gameElementsVerticalYstep[1];
	gameElementsVerticalY[2] += gameElementsVerticalYstep[1];
	if (gameElementsVerticalY[0]>gameElementsVerticalYconst[1] || gameElementsVerticalY[0] < gameElementsVerticalYconst[0]) {
		gameElementsVerticalYstep[0] *= -1;
		gameElementsVerticalYstep[1] *= -1;
	}
}
void gameElementsMoveHorizontal() {
	iSetColor(0, 162, 232);
	iFilledRectangle(0, 0, screenX, screenY);
	int i;
	static int gameElementsHorizontalX[4] = { gameElementsHorizontalXconst[0], gameElementsHorizontalXconst[1], gameElementsHorizontalXconst[2], gameElementsHorizontalXconst[3] };
	static int gameElementsHorizontalXstep[2] = { 4, -4 };
	for (i = 0; i < halfOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsHorizontalX[0], gameElementsHorizontalY[i], fileAddressPath);
	}
	for (i = 1; i < halfOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsHorizontalX[1], gameElementsHorizontalY[i], fileAddressPath);
	}
	for (i = halfOFimages; i < numberOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsHorizontalX[2], gameElementsHorizontalY[i - halfOFimages], fileAddressPath);
	}
	for (i = halfOFimages + 1; i < numberOFimages; i += 2) {
		setFilePath(_itoa(i + 1, itoaBuffer, 10));
		strcat(fileAddressPath, " 0.bmp");
		iShowBMP(gameElementsHorizontalX[3], gameElementsHorizontalY[i - halfOFimages], fileAddressPath);
	}
	gameElementsHorizontalX[0] += gameElementsHorizontalXstep[0];
	gameElementsHorizontalX[3] += gameElementsHorizontalXstep[0];
	gameElementsHorizontalX[1] += gameElementsHorizontalXstep[1];
	gameElementsHorizontalX[2] += gameElementsHorizontalXstep[1];
	if (gameElementsHorizontalX[0]>gameElementsHorizontalXconst[1] || gameElementsHorizontalX[0] < gameElementsHorizontalXconst[0]) {
		gameElementsHorizontalXstep[0] *= -1;
		gameElementsHorizontalXstep[1] *= -1;
	}
}
void babyEyeMoveVertical(int babyLeft, int babyBottom) {
	static int babyEyeMove = 0;
	static int moveEyeStep = 1;
	setFilePath("babyUpDown.bmp");
	iShowBMP(babyLeft, babyBottom, fileAddressPath);
	iSetColor(30, 30, 30);
	iFilledCircle(babyLeft - 19 + halfOFimageBlockSize, babyBottom + halfOFimageBlockSize + (babyEyeMove >> 2), 4);
	iFilledCircle(babyLeft + 19 + halfOFimageBlockSize, babyBottom + halfOFimageBlockSize + (babyEyeMove >> 2), 4);
	babyEyeMove += moveEyeStep;
	if (babyEyeMove > 39 || babyEyeMove < 0) moveEyeStep *= -1;
}
void babyEyeMoveHorizontal(int babyLeft, int babyBottom) {
	static int babyEyeMove = 0;
	static int moveEyeStep = 1;
	setFilePath("babyLeftRight.bmp");
	iShowBMP(babyLeft, babyBottom, fileAddressPath);
	iSetColor(30, 30, 30);
	iFilledCircle(babyLeft + halfOFimageBlockSize - 24 + (babyEyeMove >> 2), babyBottom + 65, 4);
	iFilledCircle(babyLeft + halfOFimageBlockSize + 15 + (babyEyeMove >> 2), babyBottom + 65, 4);
	babyEyeMove += moveEyeStep;
	if (babyEyeMove > 39 || babyEyeMove < 0) moveEyeStep *= -1;
}


//Window GUI functions
char windowGUItitleText[128], windowGUIcontentText[1024];
int windowGUIbuttonNumber, windowGUIbuttonX[3], windowGUIbuttonY[3], windowGUIbuttonPressed[3] = { 0 };
void windowGUI(char *titleText = "", char *contentText = "", int buttonNumber = 0, ...) {	//maximum 3 buttons and 3 contentText lines are supported
	static const int windowRectAmplitude = screenY / 7;
	static int windowRectBottomStep = 2;
	static const int windowRectYmax = screenY * 0.8;

	static const int windowRectBottomMax = (screenY - windowRectYmax) >> 1;
	static int windowRectYstep = windowRectBottomStep << 1;
	static const int windowRectBottomMin = windowRectBottomMax + windowRectAmplitude;
	static const int windowRectYmin = windowRectYmax - (windowRectAmplitude << 1);
	static int windowRectBottom = windowRectBottomMax;
	static int windowRectY = windowRectYmax;
	iSetColor(127, 127, 127);
	int i;
	for (i = 0; i < screenX; i += 2) {
		iLine(i + random(vibration), windowRectBottom - 20 - random(vibration), i + random(vibration), windowRectBottom + windowRectY + 20 + random(vibration));
	}
	iSetColor(0, 110, 183);
	iFilledRectangle(0, windowRectBottom, screenX, windowRectY);
	windowRectBottom += windowRectBottomStep;
	windowRectY -= windowRectYstep;
	if (windowRectY > windowRectYmax || windowRectY < windowRectYmin) {
		windowRectBottomStep *= -1;
		windowRectYstep *= -1;
	}

	iSetColor(220, 220, 220);
	iText(halfOFscreenX - strlen(titleText) * 6 - random(vibration), windowRectBottom + windowRectYmin - 40 + random(vibration), titleText, GLUT_BITMAP_TIMES_ROMAN_24);	//each character approx 12x20

	char tempContentText[1024];
	strcpy(tempContentText, contentText);
	char *tokenStr;
	tokenStr = strtok(tempContentText, "\n");
	i = 3;
	while (tokenStr != NULL && i--) {
		iText(halfOFscreenX - strlen(tokenStr) * 4 + random((vibration >> 1) + 1), (windowRectBottom >> 1) + windowRectYmin - 150 + i * 40 - random((vibration >> 1) + 1), tokenStr, GLUT_BITMAP_HELVETICA_18);	//each character approx 8x15
		tokenStr = strtok(NULL, "\n");
	}

	babyEyeMoveHorizontal(halfOFimageBlockSize + 10, windowRectBottom / 3 + windowRectYmin - 100);
	babyEyeMoveHorizontal(screenX - imageBlockSize - halfOFimageBlockSize - 10, windowRectBottom / 3 + windowRectYmin - 100);

	if (buttonNumber > 3) buttonNumber = 3;
	va_list buttonTexts;
	va_start(buttonTexts, buttonNumber);
	int buttonDistance = 10;
	int buttonTextLeft = (screenX - menuButtonX * buttonNumber - buttonDistance * (buttonNumber - 1)) >> 1;
	static int increment = buttonDistance + menuButtonX;
	windowGUIbuttonNumber = buttonNumber;
	iSetColor(30, 30, 30);
	for (i = 0; i < buttonNumber; i++) {
		windowGUIbuttonX[i] = buttonTextLeft; windowGUIbuttonY[i] = (windowRectBottom >> 2) + windowRectYmin - 200;
		setFilePath(windowGUIbuttonPressed[i] ? "menuButtonSelected.bmp" : "menuButton.bmp");
		iShowBMP(buttonTextLeft + random(vibration), windowGUIbuttonY[i] - random(vibration), fileAddressPath);
		iText(buttonTextLeft + 85 - random((vibration >> 1) + 1), windowGUIbuttonY[i] + 20 + random((vibration >> 1) + 1), va_arg(buttonTexts, char*), GLUT_BITMAP_HELVETICA_18);
		buttonTextLeft += increment;
	}
	va_end(buttonTexts);
}
void windowGUIlist(char *titleText = "", char *contentText = "", int buttonNumber = 0, ...) {
	static const int windowRectAmplitude = screenX / 20;
	static int windowRectLeftStep = 2;
	static const int windowRectLeftMax = mainMenuButtonsX - screenX / 10;
	static const int windowRectXmax = menuButtonX + screenX / 5;

	static int windowRectXstep = windowRectLeftStep << 1;
	static const int windowRectLeftMin = windowRectLeftMax + windowRectAmplitude;
	static const int windowRectXmin = windowRectXmax - (windowRectAmplitude << 1);
	static int windowRectLeft = windowRectLeftMax;
	static int windowRectX = windowRectXmax;

	iSetColor(127, 127, 127);
	int i;
	for (i = windowRectLeft - 20; i < windowRectLeft; i += 2) {
		iLine(i - random(vibration), 0 - random(vibration), i + random(vibration), screenY + random(vibration));
	}
	for (i = windowRectLeft + windowRectX; i <windowRectLeft + windowRectX + 20; i += 2) {
		iLine(i - random(vibration), 0 - random(vibration), i + random(vibration), screenY + random(vibration));
	}
	iSetColor(0, 110, 183);
	iFilledRectangle(windowRectLeft, 0, windowRectX, screenY);
	windowRectLeft += windowRectLeftStep;
	windowRectX -= windowRectXstep;
	if (windowRectX > windowRectXmax || windowRectX < windowRectXmin) {
		windowRectLeftStep *= -1;
		windowRectXstep *= -1;
	}
	static const int babyLeft = halfOFscreenX - halfOFimageBlockSize;
	static const int babyBottom = screenY - imageBlockSize - 40;
	babyEyeMoveHorizontal(babyLeft, babyBottom);

	iSetColor(220, 220, 220);
	iText(halfOFscreenX - strlen(titleText) * 6 - random(vibration), babyBottom - 40 + random(vibration), titleText, GLUT_BITMAP_TIMES_ROMAN_24);	//each character approx 12x20

	char tempContentText[1024];
	strcpy(tempContentText, contentText);
	char *tokenStr;
	tokenStr = strtok(tempContentText, "\n");
	i = maxTimeData + 2;
	while (tokenStr != NULL && i--) {
		iText(halfOFscreenX - strlen(tokenStr) * 4.5 + random((vibration >> 1) + 1), babyBottom - 420 + i * 30 - random((vibration >> 1) + 1), tokenStr, GLUT_BITMAP_9_BY_15);	//each character extactly 9x15
		tokenStr = strtok(NULL, "\n");
	}

	if (buttonNumber > 3) buttonNumber = 3;
	va_list buttonTexts;
	va_start(buttonTexts, buttonNumber);
	int buttonDistance = 20;
	int buttonTextBottom = halfOFimageBlockSize;
	static int increment = buttonDistance + menuButtonY;
	windowGUIbuttonNumber = buttonNumber;
	iSetColor(30, 30, 30);
	for (i = 0; i < buttonNumber; i++) {
		windowGUIbuttonX[i] = mainMenuButtonsX; windowGUIbuttonY[i] = buttonTextBottom;
		setFilePath(windowGUIbuttonPressed[i] ? "menuButtonSelected.bmp" : "menuButton.bmp");
		iShowBMP(mainMenuButtonsX - random(vibration), windowGUIbuttonY[i] + random(vibration), fileAddressPath);
		iText(mainMenuButtonsX + 85 + random((vibration >> 1) + 1), windowGUIbuttonY[i] + 20 - random((vibration >> 1) + 1), va_arg(buttonTexts, char*), GLUT_BITMAP_HELVETICA_18);
		buttonTextBottom += increment;
	}
	va_end(buttonTexts);
}
void windowGUImouseClickEffect(int *button, int *mx, int *my) {
	int i;
	for (i = 0; i < windowGUIbuttonNumber; i++) {
		if ((windowGUIbuttonX[i] < *mx && *mx < windowGUIbuttonX[i] + menuButtonX) && (windowGUIbuttonY[i] < *my && *my < windowGUIbuttonY[i] + menuButtonY)) {
			windowGUIbuttonPressed[i] = 1;
		}
	}
}


//Hall of fame data
int timeDataIndex, playerNameLen;
void showHallOFfameData() {
	static const char tabSpace[] = "    ";
	int i, j, k;
	strcpy(windowGUItitleText, "Hall of Fame");
	strcpy(windowGUIcontentText, "    NAME                   TIME      LEVEL\n");
	for (i = 0; i < maxTimeData; i++) {
		strcat(windowGUIcontentText, (i >= 9) ? "" : "0");
		strcat(windowGUIcontentText, _itoa(i + 1, itoaBuffer, 10));
		strcat(windowGUIcontentText, ". ");
		strcat(windowGUIcontentText, timeData[i].playerName);
		j = 15 - strlen(timeData[i].playerName);
		for (k = 15 - j; k < 15; k++) strcat(windowGUIcontentText, " ");
		strcat(windowGUIcontentText, tabSpace);
		j = (timeData[i].timeSecNeeded > 999) ? 0 : (timeData[i].timeSecNeeded > 99 ? 1 : (timeData[i].timeSecNeeded > 9 ? 2 : 3));
		for (k = 0; k < j; k++) strcat(windowGUIcontentText, " ");
		strcat(windowGUIcontentText, _itoa(timeData[i].timeSecNeeded, itoaBuffer, 10));
		strcat(windowGUIcontentText, " sec");
		strcat(windowGUIcontentText, tabSpace);
		strcat(windowGUIcontentText, "Level ");
		strcat(windowGUIcontentText, _itoa(timeData[i].levelPlayed, itoaBuffer, 10));
		strcat(windowGUIcontentText, "\n");
	}
}
void checkHallOFfameEligibility() {
	int i;
	for (i = 0; i < maxTimeData; i++) {
		if ((level == timeData[i].levelPlayed && totalTime <= timeData[i].timeSecNeeded) || level > timeData[i].levelPlayed) {
			int j;
			for (j = maxTimeData - 1; j > i; j--) {
				timeData[j] = timeData[j - 1];
			}
			timeData[i].timeSecNeeded = totalTime;
			timeData[i].levelPlayed = level;
			playerNameLen = 0;
			timeDataIndex = i;
			break;
		}
	}
	menuButtonNumbers = 5;
	level = 0;
	if (i == maxTimeData) {
		if (level == maxGameLevels) {
			showHallOFfameData();
			doThisDraw = fameDraw;
			doThisKeyboard = fameKeyboard;
			doThisMouse = fameMouse;
		}
		else {
			doThisDraw = mainMenuDraw;
			doThisKeyboard = mainMenuKeyboard;
			doThisMouse = mainMenuMouse;
		}
	}
	else {
		strcpy(windowGUItitleText, "Congratualations!!! Enter your name:");
		strcpy(windowGUIcontentText, "_");
		doThisDraw = promptUserNameDraw;
		doThisKeyboard = promptUserNameKeyboard;
		doThisMouse = promptUserNameMouse;
	}
}
void enterPlayerData() {
	windowGUIcontentText[playerNameLen] = 0;
	strcpy(timeData[timeDataIndex].playerName, windowGUIcontentText);

	//Saving data into file
	saveData();

	showHallOFfameData();
	doThisDraw = fameDraw;
	doThisKeyboard = fameKeyboard;
	doThisMouse = fameMouse;
}



//Intro
void introDraw() {

	static int r = 0, g = 202, b = 101;	//used for introRect

	static const int introRectXconst = screenX * 0.7;
	static const int introRectYconst = screenY * 0.4;
	static int introRectX = introRectXconst >> 1;
	static int introRectY = introRectYconst >> 1;
	static int introRectLeft = halfOFscreenX - (introRectX >> 1);
	static int introRectBottom = halfOFscreenY - (introRectY >> 1);

	static const int introTextLeftConst = halfOFscreenX - 145;
	static const int introTextBottomConst = halfOFscreenY - introRectY + introRectYconst - 60;
	static int introTextLeft = introRectLeft + introRectXconst - 20;
	static int introTextBottom = introRectBottom - 20;
	static int introTextKeyBlink = 0;

	gameElementsMoveVertical();

	int i;
	if (introRectX < introRectXconst) {	//33 loops by default
		introRectX += 10;
		introRectLeft -= 5;
		if (g > 110) { g -= 2; b++; }
	}
	else if (introRectY < introRectYconst) {	//24 loops by default
		introRectY += 6;
		introRectBottom -= 3;
		if (g > 110) { g--; b += 2; }
		else { g = 110; b = 183; }
	}
	else {	//intro shadow
		iSetColor(127, 127, 127);
		for (i = introRectLeft - 20; i < introRectLeft + introRectX + 20; i += 2) {
			iLine(i - random(vibration), introRectBottom - 20 - random(vibration), i + random(vibration), introRectBottom + introRectY + 20 + random(vibration));
		}
	}
	iSetColor(r, g, b);	//final colour: r = 0; g = 110; b = 183;
	iFilledRectangle(introRectLeft + random(vibration), introRectBottom + random(vibration), introRectX, introRectY);
	if (!(introRectY < introRectYconst)) {	//introRect animation is complete
		if (introTextLeft > introTextLeftConst) {
			introTextLeft -= 20;
		}
		if (introTextBottom < introTextBottomConst) {
			introTextBottom += 5;
		}
		iSetColor(220, 220, 220);
		iText(introTextLeft + random(vibration), introTextBottom + random(vibration), gameName, GLUT_BITMAP_TIMES_ROMAN_24);
		if (!(introTextLeft > introTextLeftConst) && !(introTextBottom < introTextBottomConst)) {	//introText animation is complete
			introTextLeft = introTextLeftConst;
			if (introTextKeyBlink < 25) {
				iSetColor(150, 255, 150);
				iText(introTextLeft + 5 + random((vibration >> 1) + 1), introRectBottom + 50 + random((vibration >> 1) + 1), "Press any key or click to continue", GLUT_BITMAP_HELVETICA_18);
			}
			else if (introTextKeyBlink > 49) introTextKeyBlink = 0;
			introTextKeyBlink++;
			static const int babyLeft = halfOFscreenX - halfOFimageBlockSize;
			static const int babyBottom = halfOFscreenY - halfOFimageBlockSize;
			babyEyeMoveVertical(babyLeft, babyBottom);
			introFinished = 1;
		}
	}
}
void introKeyboard(unsigned char *key) {
	if (introFinished) {
		doThisDraw = mainMenuDraw;
		doThisKeyboard = mainMenuKeyboard;
		doThisMouse = mainMenuMouse;
	}
}
void introMouse(int *button, int *state, int *mx, int *my) {
	if (*state == GLUT_UP && introFinished) {
		doThisDraw = mainMenuDraw;
		doThisKeyboard = mainMenuKeyboard;
		doThisMouse = mainMenuMouse;
	}
}


//MainMenu
void mainMenuDraw() {
	gameElementsMoveHorizontal();
	windowGUIlist();

	int i;
	iSetColor(30, 30, 30);
	for (i = 0; i < menuButtonNumbers; i++) {
		setFilePath((i != menuButtonPressed) ? "menuButton.bmp" : "menuButtonSelected.bmp");
		iShowBMP(mainMenuButtonsX + random(vibration), mainMenuButtonsY[i] + random(vibration), fileAddressPath);
		iText(mainMenuButtonsX + 85 - random((vibration >> 1) + 1), mainMenuButtonsY[i] + 20 + random((vibration >> 1) + 1), menuStrings[i], GLUT_BITMAP_HELVETICA_18);
	}
}
void mainMenuKeyboard(unsigned char *key) {

}
void mainMenuMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		for (i = 0; i < menuButtonNumbers; i++) {
			if ((mainMenuButtonsX < *mx && *mx < mainMenuButtonsX + menuButtonX) && (mainMenuButtonsY[i] < *my && *my < mainMenuButtonsY[i] + menuButtonY)) {
				menuButtonPressed = i;
				break;
			}
		}
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < menuButtonNumbers; i++) {
			if (menuButtonPressed == i && (mainMenuButtonsX < *mx && *mx < mainMenuButtonsX + menuButtonX) && (mainMenuButtonsY[i] < *my && *my < mainMenuButtonsY[i] + menuButtonY)) {
				if (i == 4) {	//click new game
					if (!level) {	//no game is paused
						menuButtonNumbers = 6;
						level = 1;	//first level number
						loadCurrentGameLevel(level);
						doThisDraw = gameStateDraw;
						doThisKeyboard = gameStateKeyboard;
						doThisMouse = gameStateMouse;
					}
					else {	//game is paused, so prompt user to lose the current game
						strcpy(windowGUItitleText, "Current game will be lost!");
						strcpy(windowGUIcontentText, "You are about to start a new game.\nAre you sure to lose the current game?");
						doThisDraw = promptUserLoseCurrentGameDraw;
						doThisKeyboard = promptUserLoseCurrentGameKeyboard;
						doThisMouse = promptUserLoseCurrentGameMouse;
					}
				}
				else if (i == 5) {	//click continue game
					timerStarted = 1;
					doThisDraw = gameStateDraw;
					doThisKeyboard = gameStateKeyboard;
					doThisMouse = gameStateMouse;
				}
				else if (i == 3) {	//click hall of fame
					showHallOFfameData();
					doThisDraw = fameDraw;
					doThisKeyboard = fameKeyboard;
					doThisMouse = fameMouse;
				}
				else if (i == 2) {	//click how to play
					strcpy(windowGUItitleText, "How to Play");
					strcpy(windowGUIcontentText, "It is an easy game!\nSee hidden pictures by pairs and memorize.\nMatch hidden pictures by pairs and win the game!");
					doThisDraw = howToPlayDraw;
					doThisKeyboard = howToPlayKeyboard;
					doThisMouse = howToPlayMouse;
				}
				else if (i == 1) {	//click credit
					strcpy(windowGUItitleText, gameName);
					strcpy(windowGUIcontentText, "Version 1.1\nhttps://arnobpl.github.io\nThank you for playing the game! Hope you will enjoy the game!");
					doThisDraw = creditDraw;
					doThisKeyboard = creditKeyboard;
					doThisMouse = creditMouse;
				}
				else if (!i) {	//click exit
					strcpy(windowGUItitleText, gameName);
					strcpy(windowGUIcontentText, "Thank you for playing the game!\nGoodbye!\nExiting...");
					doThisDraw = exitDraw;
					doThisKeyboard = exitKeyboard;
					doThisMouse = exitMouse;
				}
			}
		}
		menuButtonPressed = -1;
	}
}


//GameState
void gameStateDraw() {
	int i, j, k = 0;
	iSetColor(0, 162, 232);	//blue rectangle
	iFilledRectangle(0, 0, screenX, screenY);
	static int gameElementsVerticalLeftStep[] = { 2, -2 };	//game elements for animation
	static const int gameElementsVerticalLeftMin = gameBoardX - imageBlockSize + gameElementsVerticalLeftStep[0];
	static const int gameElementsVerticalLeftMax = screenX + gameElementsVerticalLeftStep[1];
	static int gameElementsVerticalLeft[] = { gameElementsVerticalLeftMin, gameElementsVerticalLeftMax };
	static const int gameElementBottom[] = { halfOFscreenY + imageBlockSize, halfOFscreenY - (imageBlockSize << 1) };
	static int gameElementsVerticalPicture = 1;
	gameElementsVerticalLeft[0] += gameElementsVerticalLeftStep[0];
	gameElementsVerticalLeft[1] += gameElementsVerticalLeftStep[1];
	if (gameElementsVerticalLeftMin > gameElementsVerticalLeft[0] || gameElementsVerticalLeftMax < gameElementsVerticalLeft[0]) {
		gameElementsVerticalLeftStep[0] *= -1;
		gameElementsVerticalLeftStep[1] *= -1;
		gameElementsVerticalPicture++;
		if (gameElementsVerticalPicture > numberOFimages) gameElementsVerticalPicture = 1;
	}
	setFilePath(_itoa(gameElementsVerticalPicture, itoaBuffer, 10));
	strcat(fileAddressPath, " 0.bmp");
	iShowBMP(gameElementsVerticalLeft[0], gameElementBottom[0], fileAddressPath);
	setFilePath(_itoa((gameElementsVerticalPicture + 1) % numberOFimages + 1, itoaBuffer, 10));
	strcat(fileAddressPath, " 0.bmp");
	iShowBMP(gameElementsVerticalLeft[1], gameElementBottom[1], fileAddressPath);
	static const int gameCircleLeft = gameBoardX + ((screenX - gameBoardX) >> 1);	//show baby and circle
	iSetColor(0, 110, 183);
	static int gameCircleRadiusStep = 1;
	static const int maxGameCircleRadius = gameCircleLeft - gameBoardX - gameCircleRadiusStep;
	static const int minGameCircleRadius = halfOFimageBlockSize * 1.5 + gameCircleRadiusStep;
	static int gameCircleRadius = maxGameCircleRadius;
	if (minGameCircleRadius > gameCircleRadius || maxGameCircleRadius < gameCircleRadius) gameCircleRadiusStep *= -1;
	gameCircleRadius += gameCircleRadiusStep;
	iFilledCircle(gameCircleLeft, halfOFscreenY, gameCircleRadius);
	static const int babyLeft = gameCircleLeft - halfOFimageBlockSize;
	static const int babyBottom = halfOFscreenY - halfOFimageBlockSize;
	babyEyeMoveVertical(babyLeft, babyBottom);
	iSetColor(127, 127, 127);	//grey rectangle and shaodow
	iFilledRectangle(0, 0, gameBoardX, screenY);
	for (i = gameBoardX + 10; i > gameBoardX; i -= 2) {
		iLine(i - random(vibration), random(vibration), i + random(vibration), screenY - random(vibration));
	}
	setFilePath("menuButton.bmp");	//show timer
	iShowBMP(gameBoardX, 0, fileAddressPath);
	static const int gameButtonTextLeft = gameBoardX + 85;
	iSetColor(30, 30, 30);
	iText(gameButtonTextLeft, 20, timerText(timerSec), GLUT_BITMAP_HELVETICA_18);
	setFilePath(pauseButtonPressed ? "menuButtonSelected.bmp" : "menuButton.bmp");	//show Pause Button
	iShowBMP(gameBoardX, pauseButtonBottom, fileAddressPath);
	iText(gameButtonTextLeft, pauseButtonBottom + 20, "Pause", GLUT_BITMAP_HELVETICA_18);
	setFilePath("menuButton.bmp");	//show level number
	iShowBMP(gameBoardX, menuButtonY, fileAddressPath);
	strcpy(fileAddressPath, "Level ");
	strcat(fileAddressPath, _itoa(level, itoaBuffer, 10));
	iText(gameButtonTextLeft, menuButtonY + 20, fileAddressPath, GLUT_BITMAP_HELVETICA_18);
	setFilePath("0 1.bmp");	//show closed image blocks
	for (i = 0; i < currentGameRow; i++) {
		for (j = 0; j < currentGameCol; j++) {
			if (currentGameLevel[k])  iShowBMP(currentGameLevelGraphicsX[i][j] + random(vibration), currentGameLevelGraphicsY[i][j] + random(vibration), fileAddressPath);
			k++;
		}
	}
	for (i = 0; i < openImageBlockNumber; i++) {	//show opened or opening image blocks
		if (!openImageBlockAnimation || i + 1 != openImageBlockNumber) {	//opened
			setFilePath(_itoa(openImageBlockPictures[i], itoaBuffer, 10));
			strcat(fileAddressPath, " 1.bmp");
			if (imageBlockMatched == 1 && i + 1 == openImageBlockNumber) {
				openImageBlockNumber = 0;
				imageBlockMatched = 2;
			}
		}
		else {	//opening
			if (openImageBlockAnimation < 3) {
				setFilePath("0 2.bmp");
			}
			else if (openImageBlockAnimation < 5) {
				setFilePath("0 0.bmp");
			}
			else if (openImageBlockAnimation < 7) {
				setFilePath(_itoa(openImageBlockPictures[i], itoaBuffer, 10));
				strcat(fileAddressPath, " 2.bmp");
			}
			else {
				setFilePath(_itoa(openImageBlockPictures[i], itoaBuffer, 10));
				strcat(fileAddressPath, " 1.bmp");
				openImageBlockAnimation = -1;
				notAnimatingImageBlock = 1;
			}
			openImageBlockAnimation++;
		}
		iShowBMP(openImageBlockX[i], openImageBlockY[i], fileAddressPath);
	}
	if (closeImageBlockNumber) {	//show closing image blocks
		for (i = 0; i < closeImageBlockNumber; i++) {
			if (closeImageBlockAnimation < 3) {
				setFilePath(_itoa(closeImageBlockPictures[i], itoaBuffer, 10));
				strcat(fileAddressPath, " 2.bmp");
			}
			else if (closeImageBlockAnimation < 5) {
				setFilePath("0 0.bmp");
			}
			else if (closeImageBlockAnimation < 7) {
				setFilePath("0 2.bmp");
			}
			else {
				setFilePath("0 1.bmp");
				if (i + 1 == closeImageBlockNumber) {
					closeImageBlockNumber = 0;
					notAnimatingImageBlock = 1;
				}
			}
			iShowBMP(closeImageBlockX[i], closeImageBlockY[i], fileAddressPath);
		}
		closeImageBlockAnimation++;
	}
	if (imageBlockMatched > 1) {	//image blocks match
		setFilePath(_itoa(matchedImageBlockPicture, itoaBuffer, 10));
		strcat(fileAddressPath, " 1.bmp");
		for (i = 0; i < 2; i++) {
			matchedImageBlockX[i] -= matchedImageBlockXstep[i];
			matchedImageBlockY[i] += matchedImageBlockYstep[i];
			iShowBMP(matchedImageBlockX[i], matchedImageBlockY[i], fileAddressPath);
		}
		if (matchedImageBlockX[0] < 0) {
			imageBlockMatched = 0;
		}
	}
	else if (!currentGameTotal && !imageBlockMatched) {
		if (level < maxGameLevels) {	//current level is complete but not the whole game
			timerStarted = 0;
			totalTime += timerSec;
			strcpy(windowGUItitleText, "Level ");
			_itoa(level, itoaBuffer, 10);
			strcat(windowGUItitleText, itoaBuffer);
			strcat(windowGUItitleText, " is complete!");
			strcpy(windowGUIcontentText, "Time needed: ");
			strcat(windowGUIcontentText, timerText(timerSec));
			strcat(windowGUIcontentText, "\nPlay next level  ( Level ");
			_itoa(level + 1, itoaBuffer, 10);
			strcat(windowGUIcontentText, itoaBuffer);
			strcat(windowGUIcontentText, " ) ?\nIf you do not continue, you have to play from the beginning.");
			doThisDraw = resultDraw;
			doThisKeyboard = resultKeyboard;
			doThisMouse = resultMouse;
		}
		else {	//the whole game is complete
			timerStarted = 0;
			totalTime += timerSec;
			strcpy(windowGUItitleText, "Congratulations! All levels are complete!!!");
			strcpy(windowGUIcontentText, "Time needed: ");
			strcat(windowGUIcontentText, timerText(timerSec));
			doThisDraw = resultDraw;
			doThisKeyboard = resultKeyboard;
			doThisMouse = resultMouse;
		}
	}
}
void gameStateKeyboard(unsigned char *key) {

}
void gameStateMouse(int *button, int *state, int *mx, int *my) {
	if (*state == GLUT_DOWN) {
		int i, row, col, k = 0;
		for (row = 0; row < currentGameRow; row++) {
			for (col = 0; col < currentGameCol; col++) {
				if (currentGameLevel[k] && notAnimatingImageBlock && (currentGameLevelGraphicsX[row][col] < *mx && *mx < currentGameLevelGraphicsX[row][col] + imageBlockSize) && (currentGameLevelGraphicsY[row][col] < *my && *my < currentGameLevelGraphicsY[row][col] + imageBlockSize)) {
					if (openImageBlockNumber > 1) {	//while 2 opened image blocks
						for (i = 0; i < openImageBlockNumber; i++) {	//variable 'i' is used to check if same opened image block clicked
							if (currentGameLevelGraphicsX[row][col] == openImageBlockX[i] && currentGameLevelGraphicsY[row][col] == openImageBlockY[i]) {
								break;
							}
						}
						if (i == openImageBlockNumber) {	//same opened image block not clicked
							closeImageBlockNumber = 2;
							for (i = 0; i < closeImageBlockNumber; i++) {
								closeImageBlockPictures[i] = openImageBlockPictures[i];
								closeImageBlockX[i] = openImageBlockX[i];
								closeImageBlockY[i] = openImageBlockY[i];
							}
							openImageBlockAnimation = 1;
						}
						else {	//same opened image block clicked
							closeImageBlockNumber = 1;
							closeImageBlockPictures[0] = openImageBlockPictures[!i];
							closeImageBlockX[0] = openImageBlockX[!i];
							closeImageBlockY[0] = openImageBlockY[!i];
						}
						closeImageBlockAnimation = 1;
						notAnimatingImageBlock = 0;
						openImageBlockNumber = 0;
						openImageBlockX[openImageBlockNumber] = currentGameLevelGraphicsX[row][col];
						openImageBlockY[openImageBlockNumber] = currentGameLevelGraphicsY[row][col];
						openImageBlockPictures[openImageBlockNumber] = currentGameLevel[k];
						openImageBlockSerial = k;
					}
					else if (openImageBlockNumber) {	//while 1 opened image block
						if (openImageBlockSerial != k) {	//opened image block not clicked
							openImageBlockX[openImageBlockNumber] = currentGameLevelGraphicsX[row][col];
							openImageBlockY[openImageBlockNumber] = currentGameLevelGraphicsY[row][col];
							openImageBlockPictures[openImageBlockNumber] = currentGameLevel[k];
							openImageBlockAnimation = 1;
							notAnimatingImageBlock = 0;
							if (openImageBlockPictures[0] == openImageBlockPictures[1]) {	//if 2 opened image blocks match
								for (i = 0; i < 2; i++) {
									matchedImageBlockX[i] = openImageBlockX[i];
									matchedImageBlockY[i] = openImageBlockY[i];
									matchedImageBlockXstep[i] = openImageBlockX[i] / 40;
									matchedImageBlockYstep[i] = (screenY + imageBlockSize - openImageBlockY[i]) / 40 + 1;
								}
								matchedImageBlockPicture = currentGameLevel[k];
								currentGameLevel[openImageBlockSerial] = 0;
								currentGameLevel[k] = 0;
								currentGameTotal -= 2;
								imageBlockMatched = 1;
							}
						}
						else {	//opened image block clicked
							openImageBlockNumber--;
						}
					}
					else {	//while no opened image block
						openImageBlockX[openImageBlockNumber] = currentGameLevelGraphicsX[row][col];
						openImageBlockY[openImageBlockNumber] = currentGameLevelGraphicsY[row][col];
						openImageBlockPictures[openImageBlockNumber] = currentGameLevel[k];
						openImageBlockSerial = k;
						openImageBlockAnimation = 1;
						notAnimatingImageBlock = 0;
					}
					openImageBlockNumber++;
				}
				k++;
			}
		}
		if (gameBoardX < *mx && *mx < gameBoardX + menuButtonX && pauseButtonBottom < *my) {
			pauseButtonPressed = 1;
		}
	}
	else if (*state == GLUT_UP) {
		if (gameBoardX < *mx && *mx < gameBoardX + menuButtonX && pauseButtonBottom < *my) {
			timerStarted = 0;
			doThisDraw = mainMenuDraw;
			doThisKeyboard = mainMenuKeyboard;
			doThisMouse = mainMenuMouse;
		}
		pauseButtonPressed = 0;
	}
}


//promptUserLoseCurrentGame
void promptUserLoseCurrentGameDraw() {
	gameElementsMoveVertical();
	windowGUI(windowGUItitleText, windowGUIcontentText, 2, "Yes", "No");
}
void promptUserLoseCurrentGameKeyboard(unsigned char *key) {

}
void promptUserLoseCurrentGameMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		windowGUImouseClickEffect(button, mx, my);
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < windowGUIbuttonNumber; i++) {
			if (windowGUIbuttonPressed[i] && (windowGUIbuttonX[i] < *mx && *mx < windowGUIbuttonX[i] + menuButtonX) && (windowGUIbuttonY[i] < *my && *my < windowGUIbuttonY[i] + menuButtonY)) {
				if (i == 0) {	//yes button clicked
					level = 1;
					loadCurrentGameLevel(level);
					doThisDraw = gameStateDraw;
					doThisKeyboard = gameStateKeyboard;
					doThisMouse = gameStateMouse;
				}
				else if (i == 1) {	//no button clicked
					doThisDraw = mainMenuDraw;
					doThisKeyboard = mainMenuKeyboard;
					doThisMouse = mainMenuMouse;
				}
			}
			windowGUIbuttonPressed[i] = 0;
		}
	}
}


//Result
void resultDraw() {
	gameElementsMoveVertical();
	if (level < maxGameLevels) windowGUI(windowGUItitleText, windowGUIcontentText, 3, "Hall of Fame", "Yes", "No");
	else windowGUI(windowGUItitleText, windowGUIcontentText, 1, "Hall of Fame");
}
void resultKeyboard(unsigned char *key) {

}
void resultMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		windowGUImouseClickEffect(button, mx, my);
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < windowGUIbuttonNumber; i++) {
			if (windowGUIbuttonPressed[i] && (windowGUIbuttonX[i] < *mx && *mx < windowGUIbuttonX[i] + menuButtonX) && (windowGUIbuttonY[i] < *my && *my < windowGUIbuttonY[i] + menuButtonY)) {
				if (i == 1) {	//Yes button clicked
					level++;
					loadCurrentGameLevel(level);
					doThisDraw = gameStateDraw;
					doThisKeyboard = gameStateKeyboard;
					doThisMouse = gameStateMouse;
				}
				else if (i == 2) {	//No button clicked
					checkHallOFfameEligibility();
				}
				else if (i == 0) {	//Hall of Fames button clicked
					if (level == maxGameLevels) {
						checkHallOFfameEligibility();
						windowGUIbuttonPressed[i] = 0;
						return;
					}
					showHallOFfameData();
					doThisDraw = fameDraw;
					doThisKeyboard = fameKeyboard;
					doThisMouse = fameMouse;
				}
			}
			windowGUIbuttonPressed[i] = 0;
		}
	}
}


//Hall of Fame
void fameDraw() {
	gameElementsMoveVertical();
	windowGUIlist(windowGUItitleText, windowGUIcontentText, 1, "Back");
}
void fameKeyboard(unsigned char *key) {

}
void fameMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		windowGUImouseClickEffect(button, mx, my);
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < windowGUIbuttonNumber; i++) {
			if (windowGUIbuttonPressed[i] && (windowGUIbuttonX[i] < *mx && *mx < windowGUIbuttonX[i] + menuButtonX) && (windowGUIbuttonY[i] < *my && *my < windowGUIbuttonY[i] + menuButtonY)) {
				if (i == 0) {	//back button clicked
					doThisDraw = mainMenuDraw;
					doThisKeyboard = mainMenuKeyboard;
					doThisMouse = mainMenuMouse;
				}
			}
			windowGUIbuttonPressed[i] = 0;
		}
	}
}


//promptUserName
void promptUserNameDraw() {
	gameElementsMoveHorizontal();
	windowGUI(windowGUItitleText, windowGUIcontentText, 3, "Enter Name", "Delete", "Clear");
}
void promptUserNameKeyboard(unsigned char *key) {
	if (('a' <= *key && *key <= 'z') || ('A' <= *key && *key <= 'Z') || *key == ' ') {
		if (playerNameLen < 15) {
			if ('a' <= *key && *key <= 'z') *key -= 32;
			windowGUIcontentText[playerNameLen] = 0;
			char letter[] = { *key, '_', 0 };
			strcat(windowGUIcontentText, letter);
			playerNameLen++;
		}
	}
	else if (*key == '\b') {
		if (playerNameLen) {
			windowGUIcontentText[playerNameLen - 1] = 0;
			strcat(windowGUIcontentText, "_");
			playerNameLen--;
		}
	}
	else if (*key == '\r') {
		enterPlayerData();
	}
}
void promptUserNameMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		windowGUImouseClickEffect(button, mx, my);
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < windowGUIbuttonNumber; i++) {
			if (windowGUIbuttonPressed[i] && (windowGUIbuttonX[i] < *mx && *mx < windowGUIbuttonX[i] + menuButtonX) && (windowGUIbuttonY[i] < *my && *my < windowGUIbuttonY[i] + menuButtonY)) {
				if (i == 0) {	//Enter Name button clicked
					enterPlayerData();
				}
				else if (i == 1) {	//Delete button clicked
					if (playerNameLen) {
						windowGUIcontentText[playerNameLen - 1] = 0;
						strcat(windowGUIcontentText, "_");
						playerNameLen--;
					}
				}
				else if (i == 2) {	//Clear button clicked
					strcpy(windowGUIcontentText, "_");
					playerNameLen = 0;
				}
			}
			windowGUIbuttonPressed[i] = 0;
		}
	}
}


//How to Play
void howToPlayDraw() {
	gameElementsMoveVertical();
	windowGUI(windowGUItitleText, windowGUIcontentText, 1, "Back");
}
void howToPlayKeyboard(unsigned char *key) {

}
void howToPlayMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		windowGUImouseClickEffect(button, mx, my);
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < windowGUIbuttonNumber; i++) {
			if (windowGUIbuttonPressed[i] && (windowGUIbuttonX[0] < *mx && *mx < windowGUIbuttonX[0] + menuButtonX) && (windowGUIbuttonY[0] < *my && *my < windowGUIbuttonY[0] + menuButtonY)) {
				if (i == 0) {	//back button clicked
					doThisDraw = mainMenuDraw;
					doThisKeyboard = mainMenuKeyboard;
					doThisMouse = mainMenuMouse;
				}
			}
			windowGUIbuttonPressed[i] = 0;
		}
	}
}


//Credit
void creditDraw() {
	gameElementsMoveHorizontal();
	windowGUI(windowGUItitleText, windowGUIcontentText, 1, "Back");
}
void creditKeyboard(unsigned char *key) {

}
void creditMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		windowGUImouseClickEffect(button, mx, my);
	}
	else if (*state == GLUT_UP) {
		for (i = 0; i < windowGUIbuttonNumber; i++) {
			if (windowGUIbuttonPressed[i] && (windowGUIbuttonX[i] < *mx && *mx < windowGUIbuttonX[i] + menuButtonX) && (windowGUIbuttonY[i] < *my && *my < windowGUIbuttonY[i] + menuButtonY)) {
				doThisDraw = mainMenuDraw;
				doThisKeyboard = mainMenuKeyboard;
				doThisMouse = mainMenuMouse;
			}
			windowGUIbuttonPressed[i] = 0;
		}
	}
}


//Exit
void exitDraw() {
	static const int i = timeGetTime();
	gameElementsMoveVertical();
	windowGUI(windowGUItitleText, windowGUIcontentText);
	if (timeGetTime() - i >= 2000) exit(0);	//2000 msec show
}
void exitKeyboard(unsigned char *key) {

}
void exitMouse(int *button, int *state, int *mx, int *my) {

}


