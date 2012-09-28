#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glut.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define COINS 30

GLfloat lightpos0[] = { -0.75 , 0.0 , 0.0 , 1.0 };
GLfloat lightpos1[] = { 0.0 , 0.0 , 0.0 , 1.0 };
GLfloat lightpos2[] = { 0.75 , 0.0 , 0.0 , 1.0 };

void MyGround(double height);

/* グローバル変数宣言 */
int game_start_flag = 0;
int start_flag = 0;						/* 1ゲーム開始フラグ */	
int r[3];								/* リール1,2,3の回転角 */
int empty_r[3];
int stop_r[3];							/* リール1,2,3の回転停止角 */
int stop_color_no[3] = { 7 , 7 , 7 };	/* リール1,2,3の回転停止角を示すReelColor構造体の配列番号 */
int stop_flag[3] = { 0 , 0 , 0 };		/* リール1,2,3の回転状態を示すフラグ */
int result_view_flag = 0;				/* 結果表示の完了を示すフラグ */
int click_count = 0;					/* 左クリックの回数 */
int speed = 1;							/* 回転速度 */
int Coin;								/* コイン */
int AddCoin;							/* 加算されるコイン */

GLdouble vertex[][3] = {
	{ -0.5 , 0.5 , 1.538935 },		/* A */
	{ -0.5 , -0.5 , 1.538935 },		/* B */
	{ 0.5 , -0.5 , 1.538935 },		/* C */
	{ 0.5 , 0.5 , 1.538935 },		/* D */
	{ -0.5 , 1.309122 , 0.9511 },	/* E */
	{ 0.5 , 1.309122 , 0.9511 },	/* F */
	{ -0.5 , 1.618122 , 0 },		/* G */
	{ 0.5 , 1.618122 , 0 },			/* H */
	{ -0.5 , 1.309122 , -0.9511 },	/* I */
	{ 0.5 , 1.309122 , -0.9511 },	/* J */
	{ -0.5 , 0.5 , -1.538935 },		/* K */
	{ -0.5 , -0.5 , -1.538935 },	/* L */
	{ 0.5 , -0.5 , -1.538935 },		/* M */
	{ 0.5 , 0.5 , -1.538935 },		/* N */
	{ -0.5 , -1.309122 , -0.9511 },	/* O */
	{ 0.5 , -1.309122 , -0.9511 },	/* P */
	{ -0.5 , -1.618122 , 0 },		/* Q */
	{ 0.5 , -1.618122 , 0 },		/* R */
	{ -0.5 , -1.309122 , 0.9511 },	/* S */
	{ 0.5 , -1.309122 , 0.9511 }	/* T */
};

int face[][4] = {
	{ 8 , 6 , 7 , 9 },		/* I-G-H-J 0~35 */ 
	{ 10 , 8 , 9 , 13 },	/* K-I-J-N 36~71 */
	{ 11 , 10 , 13 , 12 },	/* L-K-N-M 72~107 */
	{ 14 , 11 , 12 , 15 },	/* O-L-M-P 108~143 */
	{ 16 , 14 , 15 , 17 },	/* Q-O-P-R 144~179 */
	{ 18 , 16 , 17 , 19 },	/* S-Q-R-T 180~215 */
	{ 1 , 18 , 19 , 2 },	/* B-S-T-C 216~251 */
	{ 0 , 1 , 2 , 3 },		/* A-B-C-D 252~287 */
	{ 4 , 0 , 3 , 5 },		/* E-A-D-F 288~323 */
	{ 6 , 4 , 5 , 7 }		/* G-E-F-H 324~360 */
};

GLdouble normal[][3] = {
	{ 0.0 , -1.0 , 1.0 },
	{ 0.0 , -1.0 , 1.0 },
	{ 0.0 , 0.0 , 1.0 },
	{ 0.0 , 1.0 , 1.0 },
	{ 0.0 , 1.0 , 1.0 },
	{ 0.0 , 1.0 , -1.0 },
	{ 0.0 , 1.0 , -1.0 },
	{ 0.0 , 0.0 , -1.0 },
	{ 0.0 , -1.0 , -1.0 },
	{ 0.0 , -1.0 , -1.0 }
};

GLfloat black[] = { 0.1 , 0.1 , 0.1 };
GLfloat spot[] = { 0.0 , 0.0 , 1.0 };

/* リールカラーの構造体宣言 */
struct ReelColor {
	GLfloat color[4];
	int color_no;
};

/* 赤:1 紫:2 緑:3 青:4 */
/* リール1のカラーレイアウト */
struct ReelColor color1[10] = {
	{ { 1.0 , 0.0 , 0.0 , 1.0 } , 1 },		// R 108
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 144
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 180
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 216
	{ { 1.0 , 0.0 , 1.0 , 1.0 } , 4 },		// P 252
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 288
	{ { 1.0 , 0.0 , 1.0 , 1.0 } , 4 },		// P 324
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 360(0)
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 36
	{ { 0.0 , 0.0 , 1.0 , 1.0} , 3 },		// B 72
};
/* リール2のカラーレイアウト */
struct ReelColor color2[10] = {
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 108
	{ { 1.0 , 0.0 , 1.0 , 1.0 } , 4 },		// P 144
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 180
	{ { 1.0 , 0.0 , 0.0 , 1.0 } , 1 },		// R 216
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 252
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 288
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 324
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 360(0)
	{ { 1.0 , 0.0 , 1.0 , 1.0 } , 4 },		// P 36
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 72
};
/* リール3のカラーレイアウト */
struct ReelColor color3[10] = {
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 108
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 144
	{ { 1.0 , 0.0 , 1.0 , 1.0 } , 4 },		// P 180
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 216
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 252
	{ { 1.0 , 0.0 , 1.0 , 1.0 } , 4 },		// P 288
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 324
	{ { 1.0 , 0.0 , 0.0 , 1.0 } , 1 },		// R 360(0)
	{ { 0.0 , 0.0 , 1.0 , 1.0 } , 3 },		// B 36
	{ { 0.0 , 1.0 , 0.0 , 1.0 } , 2 },		// G 72
};

/* ゲーム開始時の説明文 */
void IntroductionMsg(void)
{
	int load_key;
	FILE *fp;

	printf("--------------------------------------------------------------------------------\n");
	printf("スロットゲームへようこそ！\n\n");
	printf("3つのリールの真ん中の絵柄を揃えましょう。\n");
	printf("左クリックで左から順にリールの回転を止められます。\n");
	printf("右クリックでリールの回転速度調整が可能です。\n");
	printf("EscキーかQキーを押すとゲームを終了します。\n\n");
	printf("絵柄を揃えてコインを増やしましょう。\n\n");
	printf("赤:		Wonderful!	コイン+10\n");
	printf("ピンク:		Excellent!	コイン+7\n");
	printf("緑:		Great!		コイン+4\n");
	printf("青:		Good!		コイン+2\n");
	printf("一致なし:	Miss!		コイン-1\n\n");
	printf("稼いだコインはSキーを押してセーブして保存できます。\n");
	printf("Good Luck!!\n\n");
	printf("(ヒント)回転速度が速い状態で絵柄を揃えると通常よりも獲得できるコイン数がUP!\n");
	printf("  　　　ただしミスした時のコイン減少量も大きくなるので注意!\n\n");
	printf("前回セーブしたコイン枚数をロードしますか?\n");
	printf("1:はい or 2:いいえ\n");
	printf(">>");
	scanf("%d",&load_key);
	if(load_key == 1)
	{
		fp = fopen("save.txt" , "r");
		if(fp == NULL)
		{
			printf("セーブファイル読み込みに失敗しました。\n");
			Coin = COINS;
		}
		else
		{
			fscanf(fp , "%d" , &Coin);
			fclose(fp);
		}
	}
	else if(load_key == 2)
	{
		Coin = COINS;
	}
	printf("\n");
	printf("ゲーム開始時の所持コインは%dコインです！\n",Coin);
	printf("--------------------------------------------------------------------------------\n");
	PlaySound("BGM\\Start_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
}

/* リール1の描画 */
void Reel1(void)
{
	int i, j;

	glLightfv(GL_LIGHT0 , GL_POSITION , lightpos0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot);

	glEnable(GL_LIGHT0);
	glBegin(GL_QUADS);
	for(j=0; j<10; j++)
	{
		glNormal3dv(normal[j]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color1[j].color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color1[j].color);
		for(i=0; i<4; i++)
		{
			glVertex3dv(vertex[face[j][i]]);
		}
	}
	glEnd();
}

/* リール2の描画 */
void Reel2(void)
{
	int i, j;

	glLightfv(GL_LIGHT1 , GL_POSITION , lightpos1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot);

	glEnable(GL_LIGHT1);
	glBegin(GL_QUADS);
	for(j=0; j<10; j++)
	{
		glNormal3dv(normal[j]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color2[j].color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color2[j].color);
		for(i=0; i<4; i++)
		{
			glVertex3dv(vertex[face[j][i]]);
		}
	}
	glEnd();
}

/* リール3の描画 */
void Reel3(void)
{
	int i, j;

	glLightfv(GL_LIGHT2 , GL_POSITION , lightpos2);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot);

	glEnable(GL_LIGHT2);
	glBegin(GL_QUADS);
	for(j=0; j<10; j++)
	{
		glNormal3dv(normal[j]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color3[j].color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color3[j].color);
		for(i=0; i<4; i++)
		{
			glVertex3dv(vertex[face[j][i]]);
		}
	}
	glEnd();
}

/* 1ゲーム開始時の3つのリールの初期位置をランダムで決定 */
void ReelSet(void)
{
	if(game_start_flag == 0)
	{
		srand((unsigned)time(NULL));
		r[0] = rand()%361;
		while(r[0]%36 != 0)
			r[0]++;
		r[1] = rand()%361;
		while(r[1]%36 != 0)
			r[1]++;
		r[2] = rand()%361;
		while(r[2]%36 != 0)
			r[2]++;
		game_start_flag = 1;
		start_flag = 1;
	}
	else if((game_start_flag == 1)&&(start_flag == 0))
	{
		r[0] = stop_r[0];
		r[1] = stop_r[1];
		r[2] = stop_r[2];
		start_flag = 1;
	}
	else;
}

/* 縦回転アニメーション */
void ReelAnime(int reel_num)
{
	int empty;

	if(click_count < (reel_num+1))
	{
		glRotated((double)r[reel_num] , 1.0 , 0.0 , 0.0);
	}
	else if((click_count==(reel_num+1))&&(stop_flag[reel_num]==0))
	{	
		stop_r[reel_num] = r[reel_num];
		while(((stop_r[reel_num])%36) != 0)
			stop_r[reel_num]++;
		empty = stop_r[reel_num];
		while(empty != 0)
		{
			empty = empty-36;
			stop_color_no[reel_num]++;
			if(stop_color_no[reel_num] >= 10)
				stop_color_no[reel_num] = stop_color_no[reel_num] - 10;
		}
		stop_flag[reel_num] = 1;
		glRotated((double)stop_r[reel_num] , 1.0 , 0.0 , 0.0);
	}
	else if((click_count>=(reel_num+1))&&(stop_flag[reel_num]==1))
	{
		glRotated((double)stop_r[reel_num] , 1.0 , 0.0 , 0.0);
	}
}

/* 絵柄の一致判定 */
void CheckReelColor(void)
{
	char *GoodMsg1 = "Wonderful!	";
	char *GoodMsg2 = "Excellent!	";
	char *GoodMsg3 = "Great!		";
	char *GoodMsg4 = "Good!		";
	char *BadMsg = "Miss!		";

	if((click_count==3)&&(stop_flag[2]==1)&&(result_view_flag==0))
	{
		int c1_no, c2_no, c3_no;

		c1_no = color1[stop_color_no[0]].color_no;
		c2_no = color2[stop_color_no[1]].color_no;
		c3_no = color3[stop_color_no[2]].color_no;

		if((c1_no==c2_no)&&(c2_no==c3_no)&&(c1_no==c3_no))
		{
			switch(c1_no)
			{
				case 1:
					PlaySound("BGM\\Red_Stop_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
					printf(GoodMsg1);
					AddCoin = 10 * speed;
					Coin += AddCoin;
					printf("コイン:	+%d	%d\n",AddCoin,Coin);
					break;
				case 4:
					PlaySound("BGM\\Pink_Stop_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
					printf(GoodMsg2);
					AddCoin = 7 * speed;
					Coin += AddCoin;
					printf("コイン:	+%d	%d\n",AddCoin,Coin);
					break;
				case 2:
					PlaySound("BGM\\Green_Stop_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
					printf(GoodMsg3);
					AddCoin = 4 * speed;
					Coin += AddCoin;
					printf("コイン:	+%d	%d\n",AddCoin,Coin);
					break;
				case 3:
					PlaySound("BGM\\Blue_Stop_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
					printf(GoodMsg4);
					AddCoin = 2 * speed;
					Coin += AddCoin;
					printf("コイン:	+%d	%d\n",AddCoin,Coin);
					break;
			}
		}
		else
		{
			printf(BadMsg);
			AddCoin = speed;
			Coin -= AddCoin;
			if(Coin<0)
				printf("コイン:	-%d	%d\n",AddCoin,0);
			else
				printf("コイン:	-%d	%d\n",AddCoin,Coin);
			if(Coin <= 0)
			{
				int Key;
				printf("\n\n");
				printf("コインがなくなりました！\n");
				printf("キーを押してください。\n\n");
				printf("もう一度ゲームを遊ぶ:	1\n");
				printf("ゲームを終了する:	2\n");
				printf(">>");
				scanf("%d",&Key);
				if(Key == 1)
				{
					PlaySound("BGM\\Enter_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
					printf("\n");
					printf("コインの枚数がリセットされました！\n\n");
					Coin = 30;
				}
				else if(Key == 2)
					exit(0);
			}
		}
		stop_color_no[0] = stop_color_no[1] = stop_color_no[2] = 7;
		result_view_flag = 1;
	}
}

void idle(void)
{
	glutPostRedisplay();
}

void display(void)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0 , 0.0 , 10.0 , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0);

	MyGround(-2.2);

	/* リールの回転角の初期値設定 */
	ReelSet();

	/* リール1に関するアニメーション描画 */
	glPushMatrix();
	ReelAnime(0);
	glTranslated(-1.5 , 0.0 , 0.0);
	Reel1();
	glPopMatrix();

	/* リール2に関するアニメーション描画 */
	glPushMatrix();
	ReelAnime(1);
	Reel2();
	glPopMatrix();

	/* リール3に関するアニメーション描画 */
	glPushMatrix();
	ReelAnime(2);
	glTranslated(1.5 , 0.0 , 0.0);
	Reel3();
	glPopMatrix();

	glutSwapBuffers();

	/* リールの絵柄の判定 */
	CheckReelColor();

	/* 1周回ったら回転角を0に戻す */
	for(i=0; i<=2; i++)
	{
		if((r[i] += speed) >= 360)
		{
			r[i] = 0;
		}
	}
}

void resize(int w,int h)
{
	glViewport(0 , 0 , w , h);

	/* 透視変換行列の設定 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0 , (double)w / (double)h , 1.0 , 100.0);

	/* モデルビュー変換行列の設定 */
	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button,int state,int x,int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if(state == GLUT_DOWN)
		{
			click_count++;
			if(click_count <= 3)
				PlaySound("BGM\\Stop_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
			else if(click_count == 4)
			{
				/* 各変数の初期化 */
				click_count = 0;
				stop_flag[0] = stop_flag[1] = stop_flag[2] = 0;
				start_flag = 0;
				result_view_flag = 0;
				PlaySound("BGM\\Start_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if(state == GLUT_DOWN)
		{
			printf("--------------------------------------------------------------------------------\n");
			printf("[速度調整モード]\n");
			printf("リールの回転速度を設定します。\n");
			printf("お好みの速度の数字キーを押してください。\n\n");
			printf("1:普通(デフォルトの速度)\n");
			printf("2:速い\n");
			printf("3:かなり速い\n");
			printf("4:神の領域\n");
			printf(">>");
			scanf("%d",&speed);
			printf("\n");
			printf("--------------------------------------------------------------------------------\n");
			PlaySound("BGM\\Enter_Sound.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		break;
	default:
		break;
	}
}

void keyboard(unsigned char key,int x,int y)
{
	int save_key;
	FILE *fp;

	switch(key)
	{
	case 'q':
	case 'Q':
	case '\033':	/* '\033'はESCのASCIIコード */
		exit(0);
	case 's':
	case 'S':
		printf("\n");
		printf("コイン枚数をセーブしますか？\n");
		printf("1:はい or 2:いいえ\n");
		printf(">>");
		scanf("%d",&save_key);
		if(save_key == 1)
		{
			fp = fopen("save.txt" , "w");
			fprintf(fp , "%d" , Coin);
			fclose(fp);
		}
		else if(save_key == 2)
		{
			printf("\n");
		}
		break;
	default:
		break;
	}
}

void init(void)
{
	glClearColor(1.0 , 1.0 , 1.0 , 1.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_LIGHTING);
}

void MyGround(double height)
{
	static GLfloat ground[][4] = {
		{ 0.6 , 0.6 , 0.6 , 1.0 },
		{ 0.3 , 0.3 , 0.3 , 1.0 },
	};

	int i, j;

	glBegin(GL_QUADS);
	glNormal3d(0.0, 1.0, 0.0);
	for(j=-5; j<5; ++j)
	{
		for(i=-5; i<5; ++i)
		{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i+j) & 1]);
			glMaterialfv(GL_FRONT, GL_SPECULAR, ground[(i+j) & 1]);
			glMaterialfv(GL_FRONT, GL_AMBIENT, black);
			glVertex3d((GLdouble)i, height, (GLdouble)j);
			glVertex3d((GLdouble)i, height, (GLdouble)(j+1));
			glVertex3d((GLdouble)(i+1), height, (GLdouble)(j+1));
			glVertex3d((GLdouble)(i+1), height, (GLdouble)j);
		}
	}
	glEnd();
}

int main(int argc,char *argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(250,100);
	glutInitWindowSize(1000,800);
	glutCreateWindow(argv[0]);
	IntroductionMsg();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}