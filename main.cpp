//-----------------------------------------------------------------------------
// File: Competition.cpp
//
// Desc: This is the first tutorial for using Direct3D. In this tutorial, all
//       we are doing is creating a Direct3D device and using it to clear the
//       window.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <strsafe.h>
#pragma warning( default : 4996 )




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device
LPD3DXSPRITE g_pSprite;
ID3DXFont* g_pFont = NULL;

struct Image {
	BOOL visible;
	LPDIRECT3DTEXTURE9 texture;
	RECT rect;
	D3DXVECTOR3 center;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 speedpos;
};
struct Property {
	FLOAT speed;
	FLOAT hp;
	FLOAT attackDamage;
};
struct Font
{
	RECT fontRect;
	BOOL Visible;
	TCHAR string[100];
};
struct Character {
	Image _Image;
	Property _Property;
	Font _Font;
};

enum GAME_STATE {
	INIT, INTRO1, INTRO2, INTRO3, INTRO4, INTRO5, READY, RUNNING, RUNNING2, SUCCESS, FAILED
};

GAME_STATE g_GameState = INTRO1;

Character init;
Character intro1;
Character intro2;
Character intro3;
Character intro4;
Character intro5;
Character ready;
Character success;
Character failed;

Character bg2;

Character bg; // 배경
Character player; // 플레이어
Character pb[100]; // 플레이어 총알
Character mob1[20]; // 몹1
Character mob2[10]; // 몹2
Character mob3[10]; // 몹3
Character mob4[10]; // 몹4
Character mboss; // 중간보스
Character boss;  // 보스
Character boss1; // 보스1
Character boss2; // 보스2
Character mb[100]; // 몹 총알

Character hpitem[20];

Character mob1spawn; // 몹1스폰매니저
Character mob2spawn; // 몹2스폰매니저
Character mob3spawn; // 몹3스폰매니저
Character mob4spawn; // 몹4스폰매니저

Character hpitemspawn;

INT bulletcount = 0; // 총알카운트
INT bulletdelay = 0; // 총알딜레이

INT mob1count = 0; // 몹1카운트
INT mob1delay = 0; // 몹1딜레이

INT mob2count = 0; // 몹2카운트
INT mob2delay = 0; // 몹2딜레이
INT movecheck;
INT randdelay = 0;

INT mbossdelay = 0;
INT mrandcheck = 0;

INT stagedelay = 0;

INT mob3count = 0; // 몹3카운트
INT mob3delay = 0; // 몹3딜레이

INT mob4count = 0; // 몹3카운트
INT mob4delay = 0; // 몹3딜레이

INT hpitemcount = 0;
INT hpitemdelay = 0;

INT mbossmove = 0; // 중간보스움직임제어
INT mbossmoveud = 0; // 중간보스움직임제어

INT bossmove = 0; // 보스움직임제어
INT bossmoveud = 0; // 보스움직임제어

INT bossmove1 = 0; // 보스1움직임제어
INT bossmove1ud = 0; // 보스1움직임제어

INT bossmove2 = 0; // 보스2움직임제어
INT bossmove2ud = 0; // 보스2움직임제어

BOOL mbosstime = FALSE; //중간보스시간
BOOL mbossLRmove = TRUE; // 왼오른
BOOL mbossUDmove = FALSE; // 위아래

BOOL Collision(D3DXVECTOR3 a, float r1, D3DXVECTOR3 b, float r2);

VOID playercontrol(); // 플레이어 컨트롤

VOID mob1control(); // 몹1 컨트롤
VOID mob2control(); // 몹2 컨트롤
VOID mob3control(); // 몹3 컨트롤
VOID mob4control(); // 몹4 컨트롤

VOID mbosscontrol(); // 중간보스컨트롤

VOID bosscontrol();	 // 최종보스컨트롤
VOID boss1control(); // 최종보스1컨트롤
VOID boss2control(); // 최종보스2컨트롤

VOID hpitemcontrol(); // hp컨트롤

VOID InitAimingBullet(float mx, float my, float ex, float ey, float speed, float& x, float& y, float& vx, float& vy);
VOID MoveAimingBullet(float& x, float& y, float& vx, float& vy);

DOUBLE fx(double mx, double my, double ex, double ey);
Font score;
Font hp;
Font damage;
Font speed;

clock_t g_OldTime;

VOID Init();
VOID Update();
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And 
    // we request a back buffer format that matches the current desktop display 
    // format.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE; // 전체화면모드로 생성
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 가장 효율적인 SWAP효과
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; // 현재 바탕화면 모드에 맞춰 백버퍼를 생성
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferHeight = 600;

    // Create the Direct3D device. Here we are using the default adapter (most
    // systems only have one, unless they have multiple graphics hardware cards
    // installed) and requesting the HAL (which is saying we want the hardware
    // device rather than a software one). Software vertex processing is 
    // specified since we know it will work on all cards. On cards that support 
    // hardware vertex processing, though, we would see a big performance gain 
    // by specifying hardware vertex processing.
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}

BOOL Collision(D3DXVECTOR3 a, float r1, D3DXVECTOR3 b, float r2) {
	float dist = sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y) *(a.y - b.y));
	if (dist <= r1 + r2) {
		return true;
	}
	else {
		return false;
	}
}

//거리구하기
DOUBLE fx(double mx, double my, double ex, double ey) {
	double d = sqrt((mx - ex) * (mx - ex) + (my - ey)*(my - ey));
	return d;
}

VOID InitAimingBullet(float mx, float my, float ex, float ey, float speed, float& x, float& y, float& vx, float& vy) {
	//탄환의 좌표 설정하기
	x = ex; y = ey;

	//목표까지의 거리 d를 구하기
	float d = sqrt((mx - ex) * (mx - ex) + (my - ey)*(my - ey));

	//속도가 일정한 값(speed)이 되도록 속도 벡터(vx, vy)를 구하기
	//목표까지의 거리 d가 0일 때는 아래쪽으로 발사함
	if (d) {
		vx = (mx - ex) / d*speed;
		vy = (my - ey) / d*speed;
	}
	else {
		vx = 0;
		vy = speed;
	}
}

VOID MoveAimingBullet(float& x, float& y, float& vx, float& vy) {
	//탄환의 좌표(x, y)에 속도를 더해 줌
	x += vx;
	y += vy;
}



VOID Init() {
	D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"1.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &init._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"2.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &intro1._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"3.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &intro2._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"4.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &intro3._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"5.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &intro4._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"6.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &intro5._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"tur.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &ready._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"end.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &success._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"die.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &failed._Image.texture);

	init._Image.rect = { 0,0,800,600 };
	intro1._Image.rect = { 0,0,800,600 };
	intro2._Image.rect = { 0,0,800,600 };
	intro3._Image.rect = { 0,0,800,600 };
	intro4._Image.rect = { 0,0,800,600 };
	intro5._Image.rect = { 0,0,800,600 };
	ready._Image.rect = { 0,0,800,600 };
	success._Image.rect = { 0,0,800,600 };
	failed._Image.rect = { 0,0,800,600 };

	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bossroom.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &bg2._Image.texture);
	
	bg2._Image.rect = { 0,0,800,600 };

	D3DXCreateFont(g_pd3dDevice, 28, 0, FW_BOLD,
		1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"돋움", &g_pFont);

	score.fontRect = { 0, 0, 100, 100 };
	score.Visible = TRUE;
	hp.fontRect = { 0, 0, 0 };
	hp.Visible = TRUE;
	damage.fontRect = { 0, 0, 0 };
	damage.Visible = TRUE;
	speed.fontRect = { 0, 0, 0 };
	speed.Visible = TRUE;

	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"hp.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &hpitem[0]._Image.texture);
	hpitem[0]._Image.rect = { 0,0,20,20 };
	hpitem[0]._Image.center = { 0,0,0 };
	hpitem[0]._Image.pos = { 0,0,0 };
	hpitem[0]._Image.visible = FALSE;
	hpitem[0]._Property.hp = 1;
	for (int i = 1; i < 20; i++) {
		hpitem[i]._Image = hpitem[0]._Image;
	}



	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bg.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &bg._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"char.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &player._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bullet.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &pb[0]._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"mob1.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &mob1[0]._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"mob2.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &mob2[0]._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"mob3.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &mob3[0]._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"mob4.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &mob4[0]._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"mmboss.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &mboss._Image.texture);

	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"boss.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &boss._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bosss.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &boss1._Image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bosss.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &boss2._Image.texture);

	boss._Image.rect = { 0,0,168,256 };
	//boss._Image.center = { 0,0,0 };
	boss._Image.pos = { 400,0,0 };
	boss._Image.visible = TRUE;
	boss._Property.hp = 50;
	//movecheck = 0;

	boss1._Image.rect = { 0,0,62,72 };
	//boss._Image.center = { 0,0,0 };
	boss1._Image.pos = { 0,0,0 };
	boss1._Image.visible = TRUE;

	boss2._Image.rect = { 0,0,62,72 };
	//boss._Image.center = { 0,0,0 };
	boss2._Image.pos = { 738,0,0 };
	boss2._Image.visible = TRUE;

	bg._Image.rect = { 0,0,800,11945 };
	bg._Image.pos = { 0,-11345,0 };

	player._Image.rect = { 30,2,41,50 };
	player._Image.center = { 0,0,0 };
	player._Image.pos = { 400,500,0 };
	player._Property.hp = 10;
	player._Image.visible = TRUE;

	pb[0]._Image.rect = { 0,0,6,8 };
	pb[0]._Image.center = { 0,0,0 };
	pb[0]._Image.pos = { 0,300,0 };
	pb[0]._Image.visible = FALSE;
	for (int i = 1; i < 100; i++) {
		pb[i]._Image = pb[0]._Image;
	}

	mob1[0]._Image.rect = { 0,0,26,42 };
	mob1[0]._Image.center = { 0,0,0 };
	mob1[0]._Image.pos = { 0,0,0 };
	mob1[0]._Image.visible = FALSE;
	mob1[0]._Property.hp = 1;
	for (int i = 1; i < 20; i++) {
		mob1[i]._Image = mob1[0]._Image;
	}

	mob2[0]._Image.rect = { 0,0,26,32 };
	mob2[0]._Image.center = { 0,0,0 };
	mob2[0]._Image.pos = { 0,0,0 };
	mob2[0]._Image.visible = FALSE;
	mob2[0]._Property.hp = 1;
	for (int i = 1; i < 10; i++) {
		mob2[i]._Image = mob2[0]._Image;
	}

	mob3[0]._Image.rect = { 0,0,24,32 };
	mob3[0]._Image.center = { 0,0,0 };
	mob3[0]._Image.pos = { 0,0,0 };
	mob3[0]._Image.visible = FALSE;
	mob3[0]._Property.hp = 1;
	mob3[0]._Property.speed = 5.0f;
	for (int i = 1; i < 10; i++) {
		mob3[i]._Image = mob3[0]._Image;
	}

	mob4[0]._Image.rect = { 0,0,28,34 };
	mob4[0]._Image.center = { 0,0,0 };
	mob4[0]._Image.pos = { 0,0,0 };
	mob4[0]._Image.visible = FALSE;
	mob4[0]._Property.hp = 1;
	mob4[0]._Property.speed = 10.0f;
	for (int i = 1; i < 10; i++) {
		mob4[i]._Image = mob4[0]._Image;
	}

	mob1spawn._Image.pos = { 0,-50,0 };
	mob2spawn._Image.pos = { 0,-50,0 };
	mob3spawn._Image.pos = { 400,-50,0 };
	mob4spawn._Image.pos = { 0,-50,0 };
	hpitemspawn._Image.pos = { 0,-50,0 };

	mboss._Image.rect = { 0,0,148,148 };
	//mboss._Image.center = { 0,0,0 };
	//mboss._Image.pos = { 0,0,0 };
	mboss._Image.visible = FALSE;
	mboss._Property.hp = 20;
	//movecheck = 0;

	srand((unsigned)time(NULL));

}

VOID playercontrol() {
	player._Image.rect = { 30,2,41,50 };

	if (player._Image.pos.y > 0) {
		if (GetKeyState(VK_UP) & 0x80000000) {
			player._Image.pos.y -= 3;
		}
	}

	if (player._Image.pos.y < 600-48) {
		if (GetKeyState(VK_DOWN) & 0x80000000) {
			player._Image.pos.y += 3;
		}
	}

	if (player._Image.pos.x > 0) {
		if (GetKeyState(VK_LEFT) & 0x80000000) {
			player._Image.rect = { 8,2,23,50 };
			player._Image.pos.x -= 3;
		}
	}

	if (player._Image.pos.x < 800-12) {
		if (GetKeyState(VK_RIGHT) & 0x80000000) {
			player._Image.rect = { 50,2,66,50 };
			player._Image.pos.x += 3;
		}
	}

	if ((GetKeyState(VK_SPACE) & 0x80000000) && bulletdelay >= 10)
	{
		if (bulletcount >= 100)
		{
			bulletcount = 0;
		}
		bulletdelay = 0;
		pb[bulletcount]._Image.pos.x = player._Image.pos.x+3;
		pb[bulletcount]._Image.visible = TRUE;
		if (bulletcount < 100)
			bulletcount++;
	}
	if (player._Property.hp == 0) {
		g_GameState = FAILED;
	}
}

VOID mob1control() {
	if (mob1delay == 10) {
		mob1delay = 0;
		if (mob1[mob1count]._Image.visible == FALSE) {
			mob1[mob1count]._Image.pos = mob1spawn._Image.pos;
			mob1[mob1count]._Image.pos.x = rand() % 800;
			mob1[mob1count]._Image.visible = TRUE;
		}
	}
	else {
		mob1delay++;
	}
	
	if (mob1count <= 10) {
		mob1count++;
	}
	else {
		mob1count = 0;
	}

	for (int i = 0; i < 20; i++) {
		if (mob1[i]._Image.visible == TRUE)
		{
			mob1[i]._Image.pos.y += 5;
		}
		if (mob1[i]._Image.pos.y > 590) {
			mob1[i]._Image.visible = FALSE;
		}
	}
//------------------밑에서부터 충돌처리----------------------
	
	for (int ci = 0; ci < 20; ci++) {
		if (Collision(mob1[ci]._Image.pos, 13, player._Image.pos, 5.5)) {
			player._Property.hp--;
			//mob1[ci]._Image.visible = FALSE;
		}
	}

	for (int ci = 0; ci < 20; ci++) {
		for (int cj = 0; cj < 100; cj++) {
			if (mob1[ci]._Image.visible == TRUE && pb[cj]._Image.visible == TRUE) {
				if (Collision(mob1[ci]._Image.pos, 13, pb[cj]._Image.pos, 3)) {
					pb[cj]._Image.visible = FALSE;
					mob1[ci]._Image.visible = FALSE;
				}
			}
		}
	}
}

VOID mob2control() {
	if (mob2delay == 30) {
		mob2delay = 0;
		if (mob2[mob2count]._Image.visible == FALSE) {
			mob2[mob2count]._Image.pos = mob2spawn._Image.pos;
			mob2[mob2count]._Image.pos.x = rand() % 800;
			mob2[mob2count]._Image.visible = TRUE;
		}
	}
	else {
		mob2delay++;
	}

	if (mob2count <= 10) {
		mob2count++;
	}
	else {
		mob2count = 0;
	}

	for (int i = 0; i < 10; i++) {
		if (mob2[i]._Image.visible == TRUE)
		{
			if (randdelay == 30) {
				movecheck = rand() % 2;
				randdelay = 0;
			}
			else {
				randdelay++;
			}
			
			if (movecheck == 1) {
				mob2[i]._Image.pos.x += 5;
			}
			if (movecheck == 0) {
				mob2[i]._Image.pos.x -= 5;
			}
			mob2[i]._Image.pos.y += 5;
		}
		if (mob2[i]._Image.pos.y > 590) {
			mob2[i]._Image.visible = FALSE;
		}
	}
	//------------------밑에서부터 충돌처리----------------------

	for (int ci = 0; ci < 10; ci++) {
		if (Collision(mob2[ci]._Image.pos, 13, player._Image.pos, 5.5)) {
			player._Property.hp--;
			//mob1[ci]._Image.visible = FALSE;
		}
	}

	for (int cci = 0; cci < 10; cci++) {
		for (int cj = 0; cj < 100; cj++) {
			if (pb[cj]._Image.visible == TRUE) {
				if (Collision(mob2[cci]._Image.pos, 13, pb[cj]._Image.pos, 3)) {
					pb[cj]._Image.visible = FALSE;
					mob2[cci]._Image.visible = FALSE;
				}
			}
		}
	}
}

VOID mob3control() {
	double x;
	if (mob3delay == 50) {
		mob3delay = 0;
		if (mob3[mob3count]._Image.visible == FALSE) {
			mob3[mob3count]._Image.pos = mob3spawn._Image.pos;
			//mob3[mob3count]._Image.pos.x = rand() % 800;
			mob3[mob3count]._Image.visible = TRUE;
		}
	}
	else {
		mob3delay++;
	}

	for (int i = 0; i < 10; i++) {
		if (mob3[i]._Image.visible == TRUE) {
			mob3[i]._Image.pos.y += 3;
			x = fx(player._Image.pos.x, player._Image.pos.y, mob3spawn._Image.pos.x, mob3spawn._Image.pos.y);
			if (x) {
				mob3[i]._Image.speedpos.x = (player._Image.pos.x - mob3spawn._Image.pos.x) / x*mob3[i]._Property.speed;
				mob3[i]._Image.speedpos.y = (player._Image.pos.y - mob3spawn._Image.pos.y) / x*mob3[i]._Property.speed;
			}
			else {
				mob3[i]._Image.speedpos.x = 0;
				mob3[i]._Image.speedpos.y = mob3[i]._Property.speed;
			}
			MoveAimingBullet(mob3[i]._Image.pos.x, mob3[i]._Image.pos.y, mob3[i]._Image.speedpos.x, mob3[i]._Image.speedpos.y);
		}
		if (mob3[i]._Image.visible == FALSE){
			InitAimingBullet(player._Image.pos.x, player._Image.pos.y, mob3spawn._Image.pos.x, mob3spawn._Image.pos.y, mob3[i]._Property.speed, mob3[i]._Image.pos.x, mob3[i]._Image.pos.y, mob3[i]._Image.speedpos.x, mob3[i]._Image.speedpos.y);
		}
	}

	for (int i = 0; i < 20; i++) {
		if (mob3[i]._Image.pos.y > 590) {
			mob3[i]._Image.visible = FALSE;
		}
	}
	//------------------밑에서부터 충돌처리----------------------

	for (int ci = 0; ci < 10; ci++) {
		if (Collision(mob3[ci]._Image.pos, 12, player._Image.pos, 5.5)) {
			player._Property.hp--;
			//mob1[ci]._Image.visible = FALSE;
		}
	}

	for (int ci = 0; ci < 10; ci++) {
		for (int cj = 0; cj < 100; cj++) {
			if (mob3[ci]._Image.visible == TRUE && pb[cj]._Image.visible == TRUE) {
				if (Collision(mob3[ci]._Image.pos, 12, pb[cj]._Image.pos, 3)) {
					pb[cj]._Image.visible = FALSE;
					mob3[ci]._Image.visible = FALSE;
				}
			}
		}
	}

}

VOID mob4control() {
	if (mob4delay == 20) {
		mob4delay = 0;
		if (mob4[mob4count]._Image.visible == FALSE) {
			mob4[mob4count]._Image.pos = mob4spawn._Image.pos;
			mob4[mob4count]._Image.pos.x = rand() % 800;
			mob4[mob4count]._Image.visible = TRUE;
		}
	}
	else {
		mob4delay++;
	}

	if (mob4count <= 10) {
		mob4count++;
	}
	else {
		mob4count = 0;
	}

	for (int i = 0; i < 20; i++) {
		if (mob4[i]._Image.visible == TRUE)
		{
			mob4[i]._Image.pos.y += 5;
			if (mob4[i]._Image.pos.y >= 200) {
				mob4[i]._Image.pos.y += 10;
			}
		}
		if (mob4[i]._Image.pos.y > 590) {
			mob4[i]._Image.visible = FALSE;
		}
	}
	//------------------밑에서부터 충돌처리----------------------

	for (int ci = 0; ci < 10; ci++) {
		if (Collision(mob4[ci]._Image.pos, 14, player._Image.pos, 5.5)) {
			player._Property.hp--;
			//mob1[ci]._Image.visible = FALSE;
		}
	}

	for (int ci = 0; ci < 10; ci++) {
		for (int cj = 0; cj < 100; cj++) {
			if (mob4[ci]._Image.visible == TRUE && pb[cj]._Image.visible == TRUE) {
				if (Collision(mob4[ci]._Image.pos, 14, pb[cj]._Image.pos, 3)) {
					pb[cj]._Image.visible = FALSE;
					mob4[ci]._Image.visible = FALSE;
				}
			}
		}
	}
}

VOID mbosscontrol() {
	if (mboss._Property.hp > 0) {
		mboss._Image.visible = TRUE;
	}
	if (mboss._Property.hp == 0) {
		bg._Image.pos.y += 1.5;
		mbosstime = FALSE;
		mboss._Image.visible = FALSE;
		
	}

		
		if (mboss._Image.pos.x >= 800-148) {
			mbossmove = 0;
		}
		if (mboss._Image.pos.x <= 0) {
			mbossmove = 1;
		}

		if (mbossmove == 0) {
			//mboss._Image.pos.x -= 0.05f;
			mboss._Image.pos.x-=3;
		}
		if (mbossmove == 1) {
			//mboss._Image.pos.x += 0.05;
			mboss._Image.pos.x+=3;
		}
	//-----------------------------------------------------
		if (mboss._Image.pos.y >= 600-148) {
			mbossmoveud = 0;
		}
		if (mboss._Image.pos.y <= 0) {
			mbossmoveud = 1;
		}
		if (mbossmoveud == 0) {
			//mboss._Image.pos.y -= 0.05f;
			mboss._Image.pos.y-=3;
		}
		if (mbossmoveud == 1) {
			//mboss._Image.pos.y += 0.05f;
			mboss._Image.pos.y+=3;
		}
		//------------------밑에서부터 충돌처리----------------------

			if (Collision(mboss._Image.pos, 74, player._Image.pos, 5.5)) {
				player._Property.hp--;
			}

			for (int cj = 0; cj < 100; cj++) {
				if (mboss._Image.visible == TRUE && pb[cj]._Image.visible == TRUE) {
					if (Collision(mboss._Image.pos, 74, pb[cj]._Image.pos, 3)) {
						pb[cj]._Image.visible = FALSE;
						mboss._Property.hp--;
						//mboss._Image.visible = FALSE;
					}
				}
		}
}

VOID bosscontrol() {
	if (boss._Property.hp > 0) {
		boss._Image.visible = TRUE;
	}
	if (boss._Property.hp == 0) {
		boss._Image.visible = FALSE;
		g_GameState = SUCCESS;
	}


	if (boss._Image.pos.x >= 800 - 148) {
		bossmove = 0;
	}
	if (boss._Image.pos.x <= 0) {
		bossmove = 1;
	}

	if (bossmove == 0) {
		boss._Image.pos.x -= 3;
	}
	if (bossmove == 1) {
		boss._Image.pos.x += 3;
	}

	//------------------밑에서부터 충돌처리----------------------

	if (Collision(boss._Image.pos, 84, player._Image.pos, 5.5)) {
		player._Property.hp--;
	}

	for (int cj = 0; cj < 100; cj++) {
		if (boss._Image.visible == TRUE && pb[cj]._Image.visible == TRUE) {
			if (Collision(boss._Image.pos, 84, pb[cj]._Image.pos, 3)) {
				pb[cj]._Image.visible = FALSE;
				boss._Property.hp--;
				//mboss._Image.visible = FALSE;
			}
		}
	}
}

VOID boss1control() {
	if (boss1._Image.pos.x >= 800 - 62) {
		bossmove1 = 0;
	}
	if (boss1._Image.pos.x <= 0) {
		bossmove1 = 1;
	}

	if (bossmove1 == 0) {
		//boss1._Image.pos.x -= 0.05f;
		boss1._Image.pos.x -= 5;
	}
	if (bossmove1 == 1) {
		//boss1._Image.pos.x += 0.05;
		boss1._Image.pos.x += 5;
	}
	//-----------------------------------------------------
	if (boss1._Image.pos.y >= 600 - 72) {
		bossmove1ud = 0;
	}
	if (boss1._Image.pos.y <= 0) {
		bossmove1ud = 1;
	}
	if (bossmove1ud == 0) {
		//boss1._Image.pos.y -= 0.05f;
		boss1._Image.pos.y -= 5;
	}
	if (bossmove1ud == 1) {
		//boss1._Image.pos.y += 0.05f;
		boss1._Image.pos.y += 5;
	}
	//------------------밑에서부터 충돌처리----------------------

	if (Collision(boss1._Image.pos, 31, player._Image.pos, 5.5)) {
		player._Property.hp--;
	}
}

VOID boss2control() {
	if (boss2._Image.pos.x >= 800 - 62) {
		bossmove2 = 0;
	}
	if (boss2._Image.pos.x <= 0) {
		bossmove2 = 1;
	}

	if (bossmove2 == 0) {
		//boss1._Image.pos.x -= 0.05f;
		boss2._Image.pos.x -= 5;
	}
	if (bossmove2 == 1) {
		//boss1._Image.pos.x += 0.05;
		boss2._Image.pos.x += 5;
	}
	//-----------------------------------------------------
	if (boss2._Image.pos.y >= 600 - 72) {
		bossmove2ud = 0;
	}
	if (boss2._Image.pos.y <= 0) {
		bossmove2ud = 1;
	}
	if (bossmove2ud == 0) {
		//boss1._Image.pos.y -= 0.05f;
		boss2._Image.pos.y -= 5;
	}
	if (bossmove2ud == 1) {
		//boss1._Image.pos.y += 0.05f;
		boss2._Image.pos.y += 5;
	}
	//------------------밑에서부터 충돌처리----------------------

	if (Collision(boss2._Image.pos, 31, player._Image.pos, 5.5)) {
		player._Property.hp--;
	}
}

VOID hpitemcontrol() {
	if (hpitemdelay == 10) {
		hpitemdelay = 0;
		if (hpitem[hpitemcount]._Image.visible == FALSE) {
			hpitem[hpitemcount]._Image.pos = hpitemspawn._Image.pos;
			hpitem[hpitemcount]._Image.pos.x = rand() % 800;
			hpitem[hpitemcount]._Image.visible = TRUE;
		}
	}
	else {
		hpitemdelay++;
	}

	if (hpitemcount <= 10) {
		hpitemcount++;
	}
	else {
		hpitemcount = 0;
	}

	for (int i = 0; i < 20; i++) {
		if (hpitem[i]._Image.visible == TRUE)
		{
			hpitem[i]._Image.pos.y += 5;
		}
		if (hpitem[i]._Image.pos.y > 590) {
			hpitem[i]._Image.visible = FALSE;
		}
	}
	//------------------밑에서부터 충돌처리----------------------

	//for (int ci = 0; ci < 20; ci++) {
	//	if (Collision(hpitem[ci]._Image.pos, 10, player._Image.pos, 5.5)) {
	//		player._Property.hp++;
	//		//mob1[ci]._Image.visible = FALSE;
	//	}
	//}

	for (int ci = 0; ci < 20; ci++) {
		for (int cj = 0; cj < 100; cj++) {
			if (hpitem[ci]._Image.visible == TRUE && pb[cj]._Image.visible == TRUE) {
				if (Collision(hpitem[ci]._Image.pos, 10, pb[cj]._Image.pos, 3)) {
					pb[cj]._Image.visible = FALSE;
					hpitem[ci]._Image.visible = FALSE;
					player._Property.hp++;
				}
			}
		}
	}
}

VOID Update() {
	clock_t CurTime = clock();//시간

	if (GetKeyState(VK_ESCAPE) & 0x80000000) {
		Cleanup();
		PostQuitMessage(0);
		ExitProcess(0);
	}

	switch (g_GameState){
	case INTRO1:
		if (CurTime - g_OldTime > 5000) {
			g_GameState = INTRO2;
		}
		break;
	case INTRO2:
		if (CurTime - g_OldTime > 8000) {
			g_GameState = INTRO3;
		}
		break;
	case INTRO3:
		if (CurTime - g_OldTime > 11000) {
			g_GameState = INTRO4;
		}
		break;
	case INTRO4:
		if (CurTime - g_OldTime > 14000) {
			g_GameState = INTRO5;
		}
		break;
	case INTRO5:
		if (CurTime - g_OldTime > 17000) {
			g_GameState = INIT;
		}
		break;
	case INIT:
		if (CurTime - g_OldTime > 20000) {
			g_GameState = READY;
		}
		break;
	case READY:
		if (GetKeyState(VK_SPACE) & 0x80000000) {
			g_GameState = RUNNING;
		}
		break;
	case RUNNING:
		playercontrol();

		hpitemcontrol();

		if (player._Property.hp > 16) {
			player._Property.hp = 15;
		}

		bulletdelay++;
		wsprintf(score.string, TEXT("점수 : %d"),player._Image.pos.x);
		for (int i = 0; i < 100; i++)
		{
			if (pb[i]._Image.visible == FALSE)
			{
				pb[i]._Image.pos.y = player._Image.pos.y;
			}
			if (pb[i]._Image.pos.y <= 0)
			{
				pb[i]._Image.visible = FALSE;
			}
			if (pb[i]._Image.visible == TRUE)
			{
				pb[i]._Image.pos.y -= 10;
			}
		}

		if (mbosstime == FALSE) {
			if (bg._Image.pos.y < 0) {
				bg._Image.pos.y += 1.5;
			}

			mob1control();
			mob2control();
			mob3control();
			mob4control();
		}



		if (mbosstime == TRUE) {
			mbosscontrol();
		}

		if (bg._Image.pos.y == -5672) {
			mbosstime = TRUE;
		}

		if (bg._Image.pos.y >= 0) {
			g_GameState = RUNNING2;
			stagedelay++;
			if (stagedelay == 50) {
				g_GameState = RUNNING2;
			}
		}



		if (GetKeyState(0x41) & 0x80000000) {
			player._Image.visible = TRUE;
		}
		break;
	case RUNNING2:
		playercontrol();

		bulletdelay++;
		for (int i = 0; i < 100; i++)
		{
			if (pb[i]._Image.visible == FALSE)
			{
				pb[i]._Image.pos.y = player._Image.pos.y;
			}
			if (pb[i]._Image.pos.y <= 0)
			{
				pb[i]._Image.visible = FALSE;
			}
			if (pb[i]._Image.visible == TRUE)
			{
				pb[i]._Image.pos.y -= 10;
			}
		}

		bosscontrol();
		boss1control();
		boss2control();
		break;
	case SUCCESS:
		break;
	case FAILED:
		if (GetKeyState(VK_SPACE) & 0x80000000) {
			Init();
			g_GameState = RUNNING;
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    if( NULL == g_pd3dDevice )
        return;

    // Clear the backbuffer to a blue color
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Rendering of scene objects can happen here
		g_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		switch (g_GameState) {
		case INIT:
			g_pSprite->Draw(init._Image.texture, &init._Image.rect, &init._Image.center, &init._Image.pos, 0xffffffff);
			break;
		case INTRO1:
			g_pSprite->Draw(intro1._Image.texture, &intro1._Image.rect, &intro1._Image.center, &intro1._Image.pos, 0xffffffff);
			break;
		case INTRO2:
			g_pSprite->Draw(intro2._Image.texture, &intro2._Image.rect, &intro2._Image.center, &intro2._Image.pos, 0xffffffff);
			break;
		case INTRO3:
			g_pSprite->Draw(intro3._Image.texture, &intro3._Image.rect, &intro3._Image.center, &intro3._Image.pos, 0xffffffff);
			break;
		case INTRO4:
			g_pSprite->Draw(intro4._Image.texture, &intro4._Image.rect, &intro4._Image.center, &intro4._Image.pos, 0xffffffff);
			break;
		case INTRO5:
			g_pSprite->Draw(intro5._Image.texture, &intro5._Image.rect, &intro5._Image.center, &intro5._Image.pos, 0xffffffff);
			break;
		case READY:
			g_pSprite->Draw(ready._Image.texture, &ready._Image.rect, &ready._Image.center, &ready._Image.pos, 0xffffffff);
			break;
		case RUNNING:
			g_pSprite->Draw(bg._Image.texture, &bg._Image.rect, &bg._Image.center, &bg._Image.pos, 0xffffffff);

			for (int i = 0; i < 100; i++) {
				if (pb[i]._Image.visible == TRUE) {
					g_pSprite->Draw(pb[i]._Image.texture, &pb[i]._Image.rect, &pb[i]._Image.center, &pb[i]._Image.pos, 0xffffffff);
				}
			}

			for (int i = 0; i < 20; i++) {
				if (mob1[i]._Image.visible == TRUE) {
					g_pSprite->Draw(mob1[i]._Image.texture, &mob1[i]._Image.rect, &mob1[i]._Image.center, &mob1[i]._Image.pos, 0xffffffff);
				}
			}

			for (int i = 0; i < 20; i++) {
				if (hpitem[i]._Image.visible == TRUE) {
					g_pSprite->Draw(hpitem[i]._Image.texture, &hpitem[i]._Image.rect, &hpitem[i]._Image.center, &hpitem[i]._Image.pos, 0xffffffff);
				}
			}

			for (int i = 0; i < 10; i++) {
				if (mob2[i]._Image.visible == TRUE) {
					g_pSprite->Draw(mob2[i]._Image.texture, &mob2[i]._Image.rect, &mob2[i]._Image.center, &mob2[i]._Image.pos, 0xffffffff);
				}
			}

			for (int i = 0; i < 10; i++) {
				if (mob3[i]._Image.visible == TRUE) {
					g_pSprite->Draw(mob3[i]._Image.texture, &mob3[i]._Image.rect, &mob3[i]._Image.center, &mob3[i]._Image.pos, 0xffffffff);
				}
			}

			for (int i = 0; i < 10; i++) {
				if (mob4[i]._Image.visible == TRUE) {
					g_pSprite->Draw(mob4[i]._Image.texture, &mob4[i]._Image.rect, &mob4[i]._Image.center, &mob4[i]._Image.pos, 0xffffffff);
				}
			}

			if (player._Image.visible == TRUE) {
				g_pSprite->Draw(player._Image.texture, &player._Image.rect, &player._Image.center, &player._Image.pos, 0xffffffff);
			}

			if (mboss._Image.visible == TRUE) {
				g_pSprite->Draw(mboss._Image.texture, &mboss._Image.rect, &mboss._Image.center, &mboss._Image.pos, 0xffffffff);
			}
			g_pFont->DrawText(g_pSprite, score.string, -1, &score.fontRect, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));

			break;

		case RUNNING2:
			g_pSprite->Draw(bg2._Image.texture, &bg2._Image.rect, &bg2._Image.center, &bg2._Image.pos, 0xffffffff);

			for (int i = 0; i < 100; i++) {
				if (pb[i]._Image.visible == TRUE) {
					g_pSprite->Draw(pb[i]._Image.texture, &pb[i]._Image.rect, &pb[i]._Image.center, &pb[i]._Image.pos, 0xffffffff);
				}
			}


			if (player._Image.visible == TRUE) {
				g_pSprite->Draw(player._Image.texture, &player._Image.rect, &player._Image.center, &player._Image.pos, 0xffffffff);
			}

			if (boss._Image.visible == TRUE) {
				g_pSprite->Draw(boss._Image.texture, &boss._Image.rect, &boss._Image.center, &boss._Image.pos, 0xffffffff);
			}
			
			g_pSprite->Draw(boss1._Image.texture, &boss1._Image.rect, &boss1._Image.center, &boss1._Image.pos, 0xffffffff);
			g_pSprite->Draw(boss2._Image.texture, &boss2._Image.rect, &boss2._Image.center, &boss2._Image.pos, 0xffffffff);


			break;
		case SUCCESS:
			g_pSprite->Draw(success._Image.texture, &success._Image.rect, &success._Image.center, &success._Image.pos, 0xffffffff);
			break;
		case FAILED:
			g_pSprite->Draw(failed._Image.texture, &failed._Image.rect, &failed._Image.center, &failed._Image.pos, 0xffffffff);
			break;
		}
		g_pSprite->End();
        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
	HWND hWnd = CreateWindow(L"D3D Tutorial", L"MapleStory",
							 WS_EX_TOPMOST | WS_POPUP, 0, 0, 800, 600,
							 GetDesktopWindow(), NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
		Init();
        // Show the window
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );

        // Enter the message loop
        MSG msg;
		ZeroMemory(&msg, sizeof(msg));
        while( msg.message != WM_QUIT )
        {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				Update();
				Render();
			}

        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



