#include<iostream>
#include<Windows.h>
#include<string>
#include<chrono>

int nScreenWidth = 120;
int nScreenHeight = 40;

//Player Locaton & View Angle
float fPlayerX = 1.0f;
float fPlayerY = 2.0f;
float fPlayerA = 0.0f;
float fFOV = 3.14159f / 3.0f;
int nMaxV = 16;
float fVelocity = 5.0f; //5.0f;
float fRotateVelocity = 1.5f; //0.8f

//Map Settings
int nMapHeight = 16;
int nMapWidth = 32;
// wchar_t* map = new wchar_t[nMapWidth * nMapHeight];

bool Hit(float fPlayerX, float fPlayerY, std::wstring map) {
	bool ret = false;
	if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') ret = true;
	return ret;
}


int main() {
	std::wstring map;
	//Set Map
	map += L"################################";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"#...........#......#...........#";
	map += L"#...........#......#...........#";
	map += L"#...........#......#...........#";
	map += L"#...........#......#...........#";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"#...........#......#...........#";
	map += L"#............######............#";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"#..............................#";
	map += L"################################";


	//Create screen
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	DWORD dwBytesWritten = 0;


	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	while (1) {
		//Timing frame
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedtime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedtime = elapsedtime.count();

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerX += cosf(fPlayerA) * fVelocity * fElapsedtime; 
			fPlayerY += sinf(fPlayerA) * fVelocity * fElapsedtime;
			if (Hit(fPlayerX, fPlayerY, map)) {
				fPlayerX -= cosf(fPlayerA) * fVelocity * fElapsedtime;
				fPlayerY -= sinf(fPlayerA) * fVelocity * fElapsedtime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX -= cosf(fPlayerA) * fVelocity * fElapsedtime; 
			fPlayerY -= sinf(fPlayerA) * fVelocity * fElapsedtime;
			if (Hit(fPlayerX, fPlayerY, map)) {
				fPlayerX += cosf(fPlayerA) * fVelocity * fElapsedtime;
				fPlayerY += sinf(fPlayerA) * fVelocity * fElapsedtime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= fRotateVelocity * fElapsedtime;
		if (fPlayerA < 0) fPlayerA = 6.28318 + fPlayerA;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += fRotateVelocity * fElapsedtime;
		if (fPlayerA > 6.28318) fPlayerA = 0;
		//RayDetect
		for (int x = 0; x < nScreenWidth; x++) {
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + (float)x / (float)nScreenWidth * fFOV;
			float fdistance2wall = 0;
			bool HitWall = false;
			while (!HitWall) {
				fdistance2wall += 0.1f;

				float fEyeX = cosf(fRayAngle);
				float fEyeY = sinf(fRayAngle);
				int nTestX = (int)(fPlayerX + fEyeX * fdistance2wall);
				int nTestY = (int)(fPlayerY + fEyeY * fdistance2wall);


				if (nTestX < 0 || nTestX > nScreenWidth || nTestY < 0 || nTestY > nScreenHeight) HitWall == true;
				else if (map[nTestY * nMapWidth + nTestX] == '#')
				{
					HitWall = true;
				}
			}
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / (float)fdistance2wall;
			// int nCeiling = (int)((float)fdistance2wall / (float) nMaxV * (nScreenHeight / 2.0));
			int nFloor = nScreenHeight - nCeiling;
			
			short nWallShade = ' ';
			if (fdistance2wall < nMaxV / 4.0)		nWallShade = 0x2588;
			else if (fdistance2wall < nMaxV / 3.0)	nWallShade = 0x2593;
			else if (fdistance2wall < nMaxV / 2.0)	nWallShade = 0x2592;
			else if (fdistance2wall < nMaxV)		nWallShade = 0x2591;
			else									nWallShade = ' ';

			for (int y = 0; y < nScreenHeight; y++) {
				int loc = y * nScreenWidth + x;
				if (y < nCeiling) {
					screen[loc] = ' ';
				}
				else if (y >= nCeiling && y < nFloor) {
					screen[loc] = nWallShade;
				}
				else {
					short nFloorShade = ' ';
					float fFloorDistance = 1.0f - ((float)y - (float)nScreenHeight / 2.0) / ((float)nScreenHeight / 2.0);
					if (fFloorDistance < 0.25)			nFloorShade = '*';
					else if (fFloorDistance < 0.5)		nFloorShade = '=';
					else if (fFloorDistance < 0.75)		nFloorShade = '-';
					else								nFloorShade = '.';

					screen[loc] = nFloorShade;
				}
			}
		}

		//Display status
		swprintf_s(screen, 43, L"X=%3.2f, Y=%3.2f, Angle=%3.2f, FPS=%3.2f", fPlayerX, fPlayerY, fPlayerA, 1.0/fElapsedtime);
		//Display map
		for (int x = 0; x < nMapWidth; x++) {
			for (int y = 0; y < nMapHeight; y++) {
				screen[(y + 1)* nScreenWidth + x] = map[y * nMapWidth + x];
			}
		}
		screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';


		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(Console, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}


	return 0;
}