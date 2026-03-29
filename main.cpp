//#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include<raylib.h>
#include<raymath.h>
#define M_PI 3.1415926
using namespace std;
const int SCREEN_WIDTH=1600;
const int SCREEN_HEIGHT=900;
const int FPS=60;
float bsizemulti=1.0f;
float bdmgmulti=1.0f;
float enemylevel=1.0f;
float shotspeedmulti=1.0f;
float bhealmulti=0.0f;
float blexplodesizmulti=1.0f;
float blexplodedmgmulti=0.1f;
float spread=5.0f;
int bpershotmulti=1;
int bcrossmulti=1;
bool blexplode=0;
Sound wxyy;
Rectangle shop={SCREEN_WIDTH-100,20,90,50};
enum Upgrade{
	FULL_HEAL=0,
	INCREASE_MAX_HEALTH=1,
	INCREASE_BULLET_SIZE=2,
	INCREASE_SPEED=3,
	INCREASE_BULLET_DAMAGE=4,
	INCREASE_SHOT_SPEED=5,
	HEALING_BULLETS=6,
	INCREASE_BULLET_CROSS=7,
	ADD_BULLETS_PER_SHOT=8
};
Upgrade ups[]={
	FULL_HEAL,
	INCREASE_MAX_HEALTH,
	INCREASE_BULLET_SIZE,
	INCREASE_SPEED,
	INCREASE_BULLET_DAMAGE,
	INCREASE_SHOT_SPEED,
	HEALING_BULLETS,
	INCREASE_BULLET_CROSS,
	ADD_BULLETS_PER_SHOT
};
const int UpgradingSize=9;
struct Bullet{
	Vector2 pos;
	float xspd,yspd;
	int des=FPS*5;
	int spd,dmg;
	bool active=1;
	int cross;
};
struct Gun{
	string name;
	int shootcd;
	int bulletspeed;
	bool continuous;
	int dmg;
};
struct Enemy{
	Vector2 pos;
	int health,MaxHealth,spd,siz;
	int type;
};
struct Item{
	Vector2 pos;
	Upgrade up;
};
//Texture zsTexture=LoadTexture("cat.png");//,ldtexture=LoadTexture("ld.jpg");
vector<Bullet>bullets;
vector<Enemy>enemies;
vector<Item>items;
vector<pair<Vector2,int>>explosions;
struct Player{
	//Image img=LoadImage("cat.png");
	//Texture tex=LoadTextureFromImage(img);
	Vector2 pos={SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
	float spd=5;
	int scd=0;
	Gun currentgun={"wxyylikes91",FPS/6,10,0,16};
	int health=1000,MaxHealth=1000;
	int level=1,exp=0,upexp=5;
	void Keep(){
		pos={Clamp(pos.x,20,SCREEN_WIDTH-20),Clamp(pos.y,20,SCREEN_HEIGHT-20)};
	}
}player;
enum GameState{RUNNING,UPGRADING,DEAD,PAUSE,SHOP}gamestate=RUNNING;
bool rmv(const Bullet b){
	return b.des<=0||b.cross<=0;
}
/*Vector2 TurnDeg(Vector2 from,Vector2 to,float deg){
float dist=Vector2Distance(from,to);
Vector2 off=Vector2Subtract(to,from);
float alpha=asin(dist/off.y)+deg;
float x=sin(alpha)*dist,y=cos(alpha)*dist;
return {x,y};
}*/
Vector2 TurnDeg(Vector2 from,Vector2 to,float deg){
	Vector2 off = Vector2Subtract(to, from);
	float rad = deg * (float)M_PI / 180.0f;
	float cosR = cos(rad);
	float sinR = sin(rad);
	float newX = off.x * cosR - off.y * sinR;
	float newY = off.x * sinR + off.y * cosR;
	return Vector2Add((Vector2){newX, newY}, from);
}
void Draw(){
	//DrawTexture(player.tex,player.pos.x,player.pos.y,WHITE);
	for(auto b:bullets){
		DrawCircle(b.pos.x,b.pos.y,3*bsizemulti,ORANGE);
	}
	for(auto e:enemies){
		DrawCircle(e.pos.x,e.pos.y,e.siz,RED);
		switch (e.type) {
		case 0:
			DrawText("WXYY",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		case 1:
			DrawText("LTY",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		default:
			DrawText("CZH",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		}
		DrawRectangle(e.pos.x-20,e.pos.y-30,40,5,GRAY);
		DrawRectangle(e.pos.x-20,e.pos.y-30,1.0*e.health/e.MaxHealth*40,5,GREEN);
	}
	for(auto it:items){
		DrawCircle(it.pos.x,it.pos.y,10,GREEN);
	}
	for(auto ex:explosions){
		DrawCircle(ex.first.x,ex.first.y,ex.second,YELLOW);
	}
	DrawCircle(player.pos.x,player.pos.y,20,BLUE);
	DrawRectangle(player.pos.x-20,player.pos.y-30,40,5,GRAY);
	DrawRectangle(player.pos.x-20,player.pos.y-30,1.0*player.health/player.MaxHealth*40,5,GREEN);
	DrawText("ZSJY",player.pos.x-10,player.pos.y-5,10,BLACK);
	DrawText(TextFormat("EXP:%d/%d",player.exp,player.upexp),10,10,30,BLACK);
	DrawText(TextFormat("Weapon:%s Damage:%.1f Shot Speed:%.1f",
		player.currentgun.name.c_str(),
		player.currentgun.dmg*bdmgmulti,
		FPS/player.currentgun.shootcd*shotspeedmulti),10,50,30,BLACK);
	DrawRectangleRec(shop,GRAY);
	DrawText("Shop",SCREEN_WIDTH-100,20,30,BLACK);
}
void Move(){
	if(IsKeyDown(KEY_W)){player.pos.y-=player.spd;}
	if(IsKeyDown(KEY_S)){player.pos.y+=player.spd;}
	if(IsKeyDown(KEY_A)){player.pos.x-=player.spd;}
	if(IsKeyDown(KEY_D)){player.pos.x+=player.spd;}
	for(auto &e:enemies){
		Vector2 eoff=Vector2Subtract(player.pos,e.pos);
		double dist=Vector2Distance(e.pos,player.pos);
		double mult=e.spd/dist*1.0;
		float xs=eoff.x*mult,ys=eoff.y*mult;
		e.pos={e.pos.x+xs,e.pos.y+=ys};
	}
}
void ShopBehavior(){
	if(CheckCollisionPointRec(GetMousePosition(),shop)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
		gamestate=SHOP;
	}
}
bool operator==(Enemy a,Enemy b){
	return a.pos==b.pos;
}
bool operator==(pair<Vector2,int> a,pair<Vector2,int> b){
	return a.first==b.first;
}
void CheckExplosion(){
	for(auto &ex:explosions){
		for(auto &e:enemies){
			if(CheckCollisionCircles(e.pos,e.siz,ex.first,ex.second)){
				e.health-=player.currentgun.dmg*blexplodedmgmulti;
				if(e.health<=0){
					//PlaySound(wxyy);
					player.exp+=1;
					remove(enemies.begin(),enemies.end(),e);
					enemies.pop_back();
				}
			}
		}
		remove(explosions.begin(),explosions.end(),ex);
		explosions.pop_back();
	}
}
void Damage(){
	bool f=0;
	for(auto &b:bullets){
		f=0;
		for(auto &e:enemies){
			if(CheckCollisionCircles(b.pos,3*bsizemulti,e.pos,e.siz)){
				f=1;
				if(b.active){
					e.health-=b.dmg*bdmgmulti;
					if(blexplode){
						explosions.push_back({b.pos,100*blexplodesizmulti});
					}
					player.health+=b.dmg*bdmgmulti*bhealmulti;
					if(player.health>player.MaxHealth){
						player.health=player.MaxHealth;
					}
					b.active=0;
					b.cross--;
					if(b.cross<=0){
						remove_if(bullets.begin(),bullets.end(),rmv);
						bullets.pop_back();
					}else{
						b.dmg/=5;
					}
					if(e.health<=0){
						PlaySound(wxyy);
						player.exp+=1;
						remove(enemies.begin(),enemies.end(),e);
						enemies.pop_back();
					}
				}
			}
		}
		if(!f){
			b.active=1;
		}
	}
	for(auto &e:enemies){
		if(CheckCollisionCircles(player.pos,20,e.pos,e.siz)){
			player.health-=e.health;
			if(player.health<=0){
				gamestate=DEAD;
			}
			remove(enemies.begin(),enemies.end(),e);
			enemies.pop_back();
		}
	}
}
void GenerateEnemy(int times){
	if(times%int(FPS*2/enemylevel/1.2)!=0)return;
	int dir=rand()%4,x,y;
	switch (dir) {
		case 0://上
		x=rand()%SCREEN_WIDTH,y=-50;
		break;
		case 1://左
		x=-50,y=rand()%SCREEN_HEIGHT;
		break;
		case 2://下
		x=rand()%SCREEN_WIDTH,y=SCREEN_HEIGHT+50;
		break;
		case 3://右
		x=SCREEN_WIDTH+50,y=rand()%SCREEN_HEIGHT;
		break;
	}
	int type=rand()%3,spd,health,siz;
	switch (type) {
	case 0:
		spd=4,health=100,siz=20;
		break;
	case 1:
		spd=5,health=50,siz=15;
		break;
	default:
		spd=3,health=150,siz=25;
		break;
	}
	spd*=enemylevel,health*=enemylevel*enemylevel,siz*=enemylevel;
	spd=min(spd,7);
	siz=min(siz,40);
	enemies.push_back({{(float)x,(float)y},health,health,spd,siz,type});
	enemylevel+=0.007;
}
void Shoot(){
	player.scd--;
	player.scd=(player.scd<0)?0:player.scd;
	for(auto &b:bullets){
		b.pos={b.pos.x+b.xspd,b.pos.y+b.yspd};
		b.des--;
		bullets.erase(remove_if(bullets.begin(),bullets.end(),rmv),bullets.end());
	}
	if((player.currentgun.continuous?IsMouseButtonDown(MOUSE_BUTTON_LEFT):IsMouseButtonPressed(MOUSE_BUTTON_LEFT))&&player.scd==0){
		Vector2 mousepos=GetMousePosition();
		Vector2 moff={mousepos.x-player.pos.x,mousepos.y-player.pos.y};
		double dist=sqrt(moff.x*moff.x+moff.y*moff.y);
		double mult=player.currentgun.bulletspeed/dist*1.0;
		float xs=moff.x*mult,ys=moff.y*mult;
		Bullet b={player.pos,xs,ys,FPS*10};
		b.spd=player.currentgun.bulletspeed;
		b.dmg=player.currentgun.dmg;
		b.cross=bcrossmulti;
		float deg=bpershotmulti/2*spread;
		if(bpershotmulti%2==0){
			deg-=spread/2;
		}
		for(int i=1;i<=bpershotmulti;i++){
			Vector2 spd=TurnDeg({0,0},{xs,ys},deg);
			deg-=spread;
			b.xspd=spd.x,b.yspd=spd.y;
			bullets.push_back(b);
		}
		player.scd=player.currentgun.shootcd/shotspeedmulti;
	}
}
void PlayMusic(Sound musics[]){
	for(int i=1;i<=9;i++){
		if(IsKeyPressed(i+48)){
			for(int j=1;j<=9;j++){
				StopSound(musics[j-1]);
			}
			PlaySound(musics[i-1]);
		}
	}
	if(IsKeyPressed(KEY_ZERO)){
		for(int i=1;i<=9;i++){
			StopSound(musics[i-1]);
		}
	}
}
void LoadSongs(Sound bgms[],string files,int num){
	for(int i=1;i<=num;i++){
		string file=files;
		if(i<10)file+="0";
		file+=to_string(i)+".mp3";
		cout<<file<<' ';
		bgms[i-1]=LoadSound(file.c_str());
	}
}
void HandleUpgrade(){
	if(player.exp>=player.upexp){
		player.exp-=player.upexp;
		player.level++;
		player.upexp=3+2*player.level;
		gamestate=UPGRADING;
	}
	switch (player.level) {
	case 3:
		player.currentgun={"Deagle",FPS/8,13,0,20};
		break;
	case 5:
		player.currentgun={"wxyyalg",FPS/8,10,1,10};
		break;
	case 7:
		player.currentgun={"Uzi",FPS/7,14,1,16};
		break;
	case 9:
		blexplode=1;
		break;
	case 10:
		player.currentgun={"AK47",FPS/9,12,1,20};
		break;
	case 12:
		player.currentgun={"WXYY's dick",FPS/12,13,1,25};
		break;
	}
}
bool operator==(Item a,Item b){
	return a.pos==b.pos;
}
void GenerateItems(int times){
	if(times%(FPS*20)==0){
		Item it;
		int x=rand()%SCREEN_WIDTH,y=rand()%SCREEN_HEIGHT;
		it.pos={(float)x,(float)y};
		it.up=ups[rand()%UpgradingSize];
		items.push_back(it);
	}
	for(auto &it:items){
		if(CheckCollisionCircles(it.pos,10,player.pos,20)){
			switch (it.up) {
			case FULL_HEAL:
				player.health+=50;
				break;
			case INCREASE_BULLET_SIZE:
				bsizemulti+=0.1;
				break;
			case INCREASE_MAX_HEALTH:
				player.health+=20;
				player.MaxHealth+=20;
				break;
			case INCREASE_SPEED:
				player.spd+=0.2;
				break;
			case INCREASE_BULLET_DAMAGE:
				bdmgmulti+=0.02;
				break;
			case INCREASE_SHOT_SPEED:
				shotspeedmulti+=0.1;
				break;
			case HEALING_BULLETS:
				bhealmulti+=0.02;
				break;
			case INCREASE_BULLET_CROSS:
				bcrossmulti++;
				break;
			case ADD_BULLETS_PER_SHOT:
				bpershotmulti++;
				bdmgmulti-=0.3;
				break;
			}
			remove(items.begin(),items.end(),it);
			items.pop_back();
		}
	}
}
Upgrade upgrade[3]={ups[rand()%UpgradingSize],ups[rand()%UpgradingSize],ups[rand()%UpgradingSize]};
void DrawUpgrade(bool flag){
	ClearBackground(GRAY);
	DrawText("Upgrade!",50,50,50,BLACK);
	if(!flag){
		for(int i=0;i<3;i++){
			upgrade[i]=ups[rand()%UpgradingSize];
		}
	}
	Rectangle rec[3];
	for(int i=0;i<3;i++){
		rec[i]={100,float(100+200*i),SCREEN_WIDTH-200,100};
		DrawRectangleRec(rec[i],WHITE);
		string text;
		switch (upgrade[i]) {
		case FULL_HEAL:
			text="Heal to max health";
			break;
		case INCREASE_BULLET_SIZE:
			text="Increase bullet size";
			break;
		case INCREASE_MAX_HEALTH:
			text="Increase max health";
			break;
		case INCREASE_SPEED:
			text="Increase speed";
			break;
		case INCREASE_BULLET_DAMAGE:
			text="Increase bullet damage";
			break;
		case INCREASE_SHOT_SPEED:
			text="Increase shot speed";
			break;
		case HEALING_BULLETS:
			text="Bullets heal when hit enemies";
			break;
		case INCREASE_BULLET_CROSS:
			text="Bullets pass through more enemies";
			break;
		case ADD_BULLETS_PER_SHOT:
			text="Add bullets per shot, but dereases damage";
			break;
		}
		DrawText(text.c_str(),100,100+200*i,40,BLACK);
	}
	for(int i=0;i<3;i++){
		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
			if(CheckCollisionPointRec(GetMousePosition(),rec[i])){
				switch (upgrade[i]) {
				case FULL_HEAL:
					player.health=player.MaxHealth;
					break;
				case INCREASE_BULLET_SIZE:
					bsizemulti+=0.5;
					break;
				case INCREASE_MAX_HEALTH:
					player.health+=200*enemylevel*enemylevel;
					player.MaxHealth+=200*enemylevel*enemylevel;
					break;
				case INCREASE_SPEED:
					player.spd+=1;
					break;
				case INCREASE_BULLET_DAMAGE:
					bdmgmulti+=0.1;
					break;
				case INCREASE_SHOT_SPEED:
					shotspeedmulti+=0.3;
					break;
				case HEALING_BULLETS:
					bhealmulti+=0.1;
					break;
				case INCREASE_BULLET_CROSS:
					bcrossmulti++;
					break;
				case ADD_BULLETS_PER_SHOT:
					bpershotmulti++;
					float mi=1/bpershotmulti-0.1;
					mi=max(0.05f,mi);
					bdmgmulti-=mi;
					bdmgmulti=max(0.1f,bdmgmulti);
					break;
				}
				gamestate=RUNNING;
				break;
			}
			
		}
	}
}
int main(){
	unsigned long long gametime=0;
	srand(time(0));
	InitAudioDevice();
	wxyy=LoadSound("wxyy.mp3");
	SetTargetFPS(FPS);
//	Sound bgm=LoadSound("sya.mp3");
	Sound bgms[11];
	LoadSongs(bgms,"songs/bgm_",0);
//	PlaySound(bgm);
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"zsjy is game 张顺甲鱼是游戏");
	bool flag=0;
	while(!WindowShouldClose()){
		if(gamestate==RUNNING){
			gametime++;
			flag=0;
			GenerateEnemy(gametime);
			GenerateItems(gametime);
			CheckExplosion();
			ShopBehavior();
			PlayMusic(bgms);
			HandleUpgrade();
			Move();
			player.Keep();
			Shoot();
			Damage();
			if(IsKeyPressed(KEY_P)){
				gamestate=PAUSE;
			}
			BeginDrawing();
			ClearBackground(RAYWHITE);
			Draw();
			EndDrawing();
		}else if(gamestate==UPGRADING){
			BeginDrawing();
			DrawUpgrade(flag);
			flag=1;
			EndDrawing();
		}else if(gamestate==DEAD){
			if(IsKeyPressed(KEY_SPACE)){
				return 0;
			}
			BeginDrawing();
			ClearBackground(RED);
			DrawText("Game over",SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2-25,50,BLACK);
			EndDrawing();
		}else if(gamestate==SHOP){
			BeginDrawing();
			ClearBackground(GRAY);
			EndDrawing();
		}else if(gamestate==PAUSE){
			if(IsKeyPressed(KEY_P)){
				gamestate=RUNNING;
			}
			BeginDrawing();
			ClearBackground(RAYWHITE);
			Draw();
			DrawText("Paused",SCREEN_WIDTH/2-50,SCREEN_HEIGHT/2-25,50,BLACK);
			EndDrawing();
		}
	}
//	UnloadSound(bgm);
//	UnloadImage(player.img);
	return 0;
}
