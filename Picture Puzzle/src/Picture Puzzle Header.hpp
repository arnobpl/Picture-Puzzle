/*
Picture Puzzle
By Arnob Paul
*/



#define _CRT_SECURE_NO_WARNINGS
#include <iGraphics.h>
#include <string.h>



#define gameName "Picture Puzzle by Arnob Paul"
#define dataFolderName "Data\\"
#define savedFileName "saved.bin"
#define maxGameLevels 6
#define maxTimeData 10



unsigned long frameTime, deltaTimeStart = timeGetTime(), deltaTimeEnd, deltaTimeDiff;



//Game screen resolution, FPS, vibration values
short screenX;
short screenY;
short fps;
short fullscreen;
short vibration;
short screenXtemp, screenYtemp;	//used to preserve screenX, screenY value when fullscreen is active



								//Special functions
typedef struct {
	char playerName[16];
	unsigned short timeSecNeeded : 11;
	unsigned short levelPlayed : 5;
} TimeData;
TimeData timeData[maxTimeData];
char fileAddressPath[256];
void setFilePath(const char *filePath) {
	strcpy(fileAddressPath, dataFolderName);
	strcat(fileAddressPath, filePath);
}
void readData() {
	//setting up screen resolution, visual effect and hall of fame

	int i = 0, j;

	setFilePath(savedFileName);
	FILE *fp = fopen(fileAddressPath, "rb");
	if (fp != NULL) {	//if already a file exists
		short *fileData_system;
		fileData_system = (short*)malloc(sizeof(short));
		fread(fileData_system, sizeof(short), 1, fp);
		screenXtemp = *fileData_system;
		fread(fileData_system, sizeof(short), 1, fp);
		screenYtemp = *fileData_system;
		if ((screenYtemp >= 720 && screenYtemp <= 1080) && (screenXtemp * 0.75 >= screenYtemp && screenXtemp <= 1920)) {	//checking whether saved screen resolution is valid
			screenX = screenXtemp;
			screenY = screenYtemp;
		}
		else {	//saved screen resolution is invalid and so default loaded
			fclose(fp);
			remove(fileAddressPath);
			goto setDefaultValues;
		}
		fread(fileData_system, sizeof(short), 1, fp);
		short fpsTemp = *fileData_system;
		if (fpsTemp >= 30 && fpsTemp <= 120) {	//checking whether fps value is valid
			fps = fpsTemp;
		}
		else {	//fps value is invalid and so default loaded
			fclose(fp);
			remove(fileAddressPath);
			goto setDefaultValues;
		}
		fread(fileData_system, sizeof(short), 1, fp);	//checking whether fullscreen
		fullscreen = *fileData_system;
		if (fullscreen) {	//if fullscreen, then ignore custom screenX, screenY
			screenX = glutGet(GLUT_SCREEN_WIDTH);
			screenY = glutGet(GLUT_SCREEN_HEIGHT);
		}
		fread(fileData_system, sizeof(short), 1, fp);
		unsigned short vibrationTemp = *fileData_system;
		if (vibrationTemp == 3 || vibrationTemp == 1) {	//checking whether vibration value is valid
			vibration = vibrationTemp;
		}
		else {	//vibration value is invalid and so default loaded
			fclose(fp);
			remove(fileAddressPath);
			goto setDefaultValues;
		}
		free(fileData_system);

		TimeData *fileData_timeData;	//loading timeData with checking validity
		fileData_timeData = (TimeData*)malloc(sizeof(TimeData));
		fread(fileData_timeData, sizeof(TimeData), 1, fp);
		if (fileData_timeData->timeSecNeeded > 0 && fileData_timeData->timeSecNeeded < 10000) {
			timeData[i].timeSecNeeded = fileData_timeData->timeSecNeeded;
		}
		else {
			fclose(fp);
			remove(fileAddressPath);
			goto setDefaultValues;
		}
		if (fileData_timeData->levelPlayed <= maxGameLevels && fileData_timeData->levelPlayed > 0) {
			timeData[0].levelPlayed = fileData_timeData->levelPlayed;
		}
		else {
			fclose(fp);
			remove(fileAddressPath);
			goto setDefaultValues;
		}
		for (j = 0; j < 16; j++) {
			if (!((fileData_timeData->playerName[j] >= 0 && fileData_timeData->playerName[j] < 26) || fileData_timeData->playerName[j] == -33)) {
				if (fileData_timeData->playerName[j] == -65) { fileData_timeData->playerName[j] = 0; break; }
				fclose(fp);
				remove(fileAddressPath);
				goto setDefaultValues;
			}
			else {
				fileData_timeData->playerName[j] += 65;	//decrypting text
			}
		}
		strcpy(timeData[0].playerName, fileData_timeData->playerName);
		for (i = 1; i < maxTimeData; i++) {
			fread(fileData_timeData, sizeof(TimeData), 1, fp);
			if ((fileData_timeData->timeSecNeeded >= timeData[i - 1].timeSecNeeded || fileData_timeData->levelPlayed <= timeData[i - 1].levelPlayed) && fileData_timeData->timeSecNeeded > 0 && fileData_timeData->timeSecNeeded < 10000) {
				timeData[i].timeSecNeeded = fileData_timeData->timeSecNeeded;
			}
			else {
				fclose(fp);
				remove(fileAddressPath);
				goto setDefaultValues;
			}
			if (fileData_timeData->levelPlayed <= timeData[i - 1].levelPlayed && fileData_timeData->levelPlayed <= maxGameLevels && fileData_timeData->levelPlayed > 0) {
				timeData[i].levelPlayed = fileData_timeData->levelPlayed;
			}
			else {
				fclose(fp);
				remove(fileAddressPath);
				goto setDefaultValues;
			}
			for (j = 0; j < 16; j++) {
				if (!((fileData_timeData->playerName[j] >= 0 && fileData_timeData->playerName[j] < 26) || fileData_timeData->playerName[j] == -33)) {
					if (fileData_timeData->playerName[j] == -65) { fileData_timeData->playerName[j] = 0; break; }
					fclose(fp);
					remove(fileAddressPath);
					goto setDefaultValues;
				}
				else {
					fileData_timeData->playerName[j] += 65;	//decrypting text
				}
			}
			strcpy(timeData[i].playerName, fileData_timeData->playerName);
		}
		free(fileData_timeData);
		fclose(fp);
	}
	else {	//if no file exists, then load default
	setDefaultValues:
		screenX = screenXtemp = 960;
		screenY = screenYtemp = 720;
		fps = 30;
		fullscreen = 0;
		vibration = 3;
		for (i = 0; i < 5; i++) {
			timeData[i].timeSecNeeded = 480 - 20 * (i - 2 * (1 & i));
			timeData[i].levelPlayed = 5 - (i >> 1);
			strcpy(timeData[i].playerName, "ARNOB PAUL");
		}
		for (i = 5; i < maxTimeData; i++) {
			timeData[i].timeSecNeeded = 480 - 20 * (i - 2 * (1 & i));
			timeData[i].levelPlayed = 5 - (i >> 1);
			strcpy(timeData[i].playerName, "ARPON PAUL");
		}
	}
}
void saveData() {
	//Saving data into file
	setFilePath(savedFileName);
	FILE *fp = fopen(fileAddressPath, "wb");


	if (screenYtemp < 720 || screenYtemp > 1080 || screenXtemp * 0.75 < screenYtemp || screenXtemp > 1920) {	//checking whether saved screen resolution is valid (for launcher)
		screenXtemp = 960;
		screenYtemp = 720;
	}
	if (fps < 30 || fps > 120) {	//checking whether fps value is valid (for launcher)
		fps = 30;
	}
	short *fileData_system;
	fileData_system = &screenXtemp;
	fwrite(fileData_system, sizeof(short), 1, fp);
	fileData_system = &screenYtemp;
	fwrite(fileData_system, sizeof(short), 1, fp);
	fileData_system = &fps;
	fwrite(fileData_system, sizeof(short), 1, fp);
	fileData_system = &fullscreen;
	fwrite(fileData_system, sizeof(short), 1, fp);
	fileData_system = &vibration;
	fwrite(fileData_system, sizeof(short), 1, fp);

	TimeData *fileData_timeData;
	fileData_timeData = (TimeData*)malloc(sizeof(TimeData));
	int i, j;
	for (i = 0; i < maxTimeData; i++) {
		fileData_timeData->timeSecNeeded = timeData[i].timeSecNeeded;
		fileData_timeData->levelPlayed = timeData[i].levelPlayed;
		for (j = 0; j < 16; j++) fileData_timeData->playerName[j] = timeData[i].playerName[j] - 65;	//encrypting text
		fwrite(fileData_timeData, sizeof(TimeData), 1, fp);
	}
	free(fileData_timeData);
	fclose(fp);
}



//Function pointers for various purpoeses
void(*doThisDraw)();	//function pointer for iDraw
void(*doThisKeyboard)(unsigned char *key);	//function pointer for iKeyboard, iSpecialKeyboard
void(*doThisMouse)(int *button, int *state, int *mx, int *my);	//function pointer for iMouse



																//Igraphics functions
																/*
																function iDraw() is called again and again by the system.
																*/
void iDraw() {
	//place your drawing codes here

	iClear();

	doThisDraw();

	deltaTimeEnd = timeGetTime();
	deltaTimeDiff = deltaTimeEnd - deltaTimeStart;
	Sleep((frameTime > deltaTimeDiff) ? (frameTime - deltaTimeDiff) : 0);
	deltaTimeStart = deltaTimeEnd;
}
/*
function iMouseMove() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my) {
	//printf("x = %d, y = %d\n", mx, my);
	//place your codes here

}
/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my) {
	doThisMouse(&button, &state, &mx, &my);

	//if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	//	//printf("x = %d, y = %d\n", mx, my);
	//	//place your codes here	

	//}
	//if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	//	//place your codes here	

	//}
}
/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key) {
	//place your codes for other keys here

	doThisKeyboard(&key);

}
/*
function iSpecialKeyboard() is called whenever user hits special keys like-
function keys, home, end, pg up, pg down, arrows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key) {
	//place your codes for other keys here

	doThisKeyboard(&key);

}


