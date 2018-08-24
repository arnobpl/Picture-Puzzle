/*
Picture Puzzle
By Arnob Paul
*/



#include "Picture Puzzle Header.hpp"



#define launcherScreenX 640
#define launcherScreenY 480
#define launcherFPS 60


#define halfOFlauncherScreenX (launcherScreenX >> 1)


#define launcherText "Configuration Settings Wizard"
#define launcherHelpText "Click an item to change value via keyboard and press ENTER to save"
#define launcherButtonText "Verify and Launch Game"
#define EXEfileName "Game.exe"
#define buttonRectX 300
#define buttonRectY 50



char itoaBuffer[20];	//used for "_itoa" function



char menuString[5][30] = { "Screen Width (X) :", "Screen Height (Y) :", "Game Speed (30 to 120) :", "Fullscreen (native only) :", "Vibration Effect :" };
short** menuData;
int lineDividerbottomStart, lineDividerbottom[5], lineDividerTextbottom[5];
int buttonRectLeft = (launcherScreenX - buttonRectX) >> 1;
int menuItemKeyboardState = 0;
char editValueString[6];
int editValueStringLen;



void saveMenuItem() {
	editValueString[editValueStringLen] = 0;
	*menuData[menuItemKeyboardState - 1] = atoi(editValueString);
	menuItemKeyboardState = 0;
}
void showTickSign(int left, int bottom, int value) {
	iSetColor(220, 220, 220);
	iFilledRectangle(left, bottom, 40, 40);
	double xList[4], yList[4];
	if (value) {
		xList[0] = left + 2, xList[1] = left + 7, xList[2] = left + 21, xList[3] = left + 16;
		yList[0] = bottom + 21, yList[1] = bottom + 26, yList[2] = bottom + 12, yList[3] = bottom + 7;
		iSetColor(34, 177, 76);
		iFilledPolygon(xList, yList, 4);
		xList[0] = left + 16, xList[1] = left + 21, xList[2] = left + 37, xList[3] = left + 32;
		yList[0] = bottom + 17, yList[1] = bottom + 12, yList[2] = bottom + 28, yList[3] = bottom + 33;
		iFilledPolygon(xList, yList, 4);
	}
	else {
		xList[0] = left + 2, xList[1] = left + 7, xList[2] = left + 37, xList[3] = left + 32;
		yList[0] = bottom + 32, yList[1] = bottom + 37, yList[2] = bottom + 7, yList[3] = bottom + 2;
		iSetColor(136, 0, 21);
		iFilledPolygon(xList, yList, 4);
		xList[0] = left + 7, xList[1] = left + 2, xList[2] = left + 32, xList[3] = left + 37;
		yList[0] = bottom + 2, yList[1] = bottom + 7, yList[2] = bottom + 37, yList[3] = bottom + 32;
		iFilledPolygon(xList, yList, 4);
	}
}



//Only function names declared here for function-function linkage
void menuDraw();
void menuKeyboard(unsigned char *key);
void menuMouse(int *button, int *state, int *mx, int *my);



int main() {
	//place your own initialization codes here


	//setting up screen resolution, visual effect and hall of fame
	readData();


	//setting up wizard data
	menuData = (short**)malloc(5 * sizeof(short));
	menuData[0] = &screenXtemp;
	menuData[1] = &screenYtemp;
	menuData[2] = &fps;
	menuData[3] = &fullscreen;
	menuData[4] = &vibration;


	//setting up wizard GUI
	int i;
	lineDividerbottomStart = launcherScreenY - 70;
	for (i = 0; i < 5; i++) {
		lineDividerbottom[i] = launcherScreenY - 70 - (1 + i) * 70;
		lineDividerTextbottom[i] = lineDividerbottom[i] + 29;
	}


	//setting up function pointers for first time
	doThisDraw = menuDraw;
	doThisKeyboard = menuKeyboard;
	doThisMouse = menuMouse;


	frameTime = 1000 / launcherFPS;
	iInitialize(launcherScreenX, launcherScreenY, gameName, 1);
	free(menuData);
	return 0;
}



//menu
void menuDraw() {
	iSetColor(0, 110, 183);
	iFilledRectangle(0, 0, launcherScreenX, launcherScreenY);
	static const int titleTextLeft = (launcherScreenX - strlen(gameName) * 9) >> 1;
	static const int launcherTextLeft = (launcherScreenX - strlen(launcherText) * 9) >> 1;
	static const int launcherHelpTextLeft = (launcherScreenX - strlen(launcherHelpText) * 9) >> 1;
	iSetColor(220, 220, 220);
	iText(titleTextLeft, launcherScreenY - 20, gameName, GLUT_BITMAP_9_BY_15);
	iText(launcherTextLeft, launcherScreenY - 40, launcherText, GLUT_BITMAP_9_BY_15);
	iText(launcherHelpTextLeft, launcherScreenY - 60, launcherHelpText, GLUT_BITMAP_9_BY_15);
	iLine(0, lineDividerbottomStart, launcherScreenX, lineDividerbottomStart);

	int i;
	for (i = 0; i < 5; i++) {
		iSetColor(220, 220, 220);
		iText(halfOFlauncherScreenX - strlen(menuString[i]) * 9 - 10, lineDividerTextbottom[i], menuString[i], GLUT_BITMAP_9_BY_15);
		iSetColor(0, 162, 232);
		iLine(0, lineDividerbottom[i], launcherScreenX, lineDividerbottom[i]);
	}
	iSetColor(220, 220, 220);
	iLine(0, lineDividerbottom[4], launcherScreenX, lineDividerbottom[4]);

	iSetColor(0, 127, 0);
	iFilledRectangle(buttonRectLeft, 0, buttonRectX, buttonRectY);
	iSetColor(220, 220, 220);
	iRectangle(buttonRectLeft, 0, buttonRectX, buttonRectY);
	static const int launcherButtonTextLeft = (launcherScreenX - strlen(launcherButtonText) * 9) >> 1;
	iSetColor(220, 220, 220);
	iText(launcherButtonTextLeft, 20, launcherButtonText, GLUT_BITMAP_9_BY_15);
	int menuDataSpace, editValueStringLen;
	for (i = 0; i < 3; i++) {
		if (i != menuItemKeyboardState - 1) {
			_itoa(*menuData[i], itoaBuffer, 10);
			menuDataSpace = (*menuData[i] > 999) ? 0 : (*menuData[i] > 99) ? 1 : (*menuData[i] > 9) ? 2 : 3;
			iText(halfOFlauncherScreenX + 10 + menuDataSpace * 9, lineDividerTextbottom[i], itoaBuffer, GLUT_BITMAP_9_BY_15);
		}
		else {
			editValueStringLen = strlen(editValueString);
			menuDataSpace = (editValueStringLen > 4) ? 0 : (editValueStringLen > 3) ? 1 : (editValueStringLen > 2) ? 2 : 3;
			iText(halfOFlauncherScreenX + 10 + menuDataSpace * 9, lineDividerTextbottom[i], editValueString, GLUT_BITMAP_9_BY_15);
		}
	}
	showTickSign(halfOFlauncherScreenX + 10, lineDividerTextbottom[3] - 12, *menuData[3]);
	showTickSign(halfOFlauncherScreenX + 10, lineDividerTextbottom[4] - 12, *menuData[4] - 1);
}
void menuKeyboard(unsigned char *key) {
	if (menuItemKeyboardState) {
		if (*key >= '0' && *key <= '9') {
			if (editValueStringLen < 4) {
				editValueString[editValueStringLen] = 0;
				char letter[] = { *key, '_', 0 };
				strcat(editValueString, letter);
				editValueStringLen++;
			}
		}
		else if (*key == '\b') {
			if (editValueStringLen) {
				editValueString[editValueStringLen - 1] = 0;
				strcat(editValueString, "_");
				editValueStringLen--;
			}
		}
		else if (*key == '\r') {
			saveMenuItem();
		}
	}
}
void menuMouse(int *button, int *state, int *mx, int *my) {
	int i;
	if (*state == GLUT_DOWN) {
		if (*my < lineDividerbottomStart && *my > lineDividerbottom[2]) {
			for (i = 0; i < 3; i++) {
				if (*my > lineDividerbottom[i]) {
					menuItemKeyboardState = i + 1;
					_itoa(*menuData[i], itoaBuffer, 10);
					strcpy(editValueString, itoaBuffer);
					strcat(editValueString, "_");
					editValueStringLen = strlen(editValueString) - 1;
					break;
				}
			}
		}
		else if (*my < lineDividerbottom[2] && *my > lineDividerbottom[4]) {
			if (*my > lineDividerbottom[3]) {	//fullscreen
				*menuData[3] = !*menuData[3];
			}
			else if (*my > lineDividerbottom[4]) {	//vibration effect
				if (*menuData[4] > 1) *menuData[4] = 1;
				else *menuData[4] = 3;
			}
		}
		else if (*mx > buttonRectLeft && *mx < buttonRectLeft + buttonRectX && *my < buttonRectY) {
			if (menuItemKeyboardState) saveMenuItem();
			saveData();
			FILE *fp;
			if (fp = fopen(EXEfileName, "rb")) {
				fclose(fp);
				ShellExecute(NULL, "open", EXEfileName, NULL, NULL, SW_SHOWNORMAL);	//Windows platform specific code
			}
			exit(0);
		}
		else {
			menuItemKeyboardState = 0;
		}
	}
}