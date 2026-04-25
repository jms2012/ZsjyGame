//#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include"raylib.h"
#include"raymath.h"
#define RAYGUI_IMPLEMENTATION
#include"raygui.h"
using namespace std;
const int SCREEN_WIDTH=3200;
const int SCREEN_HEIGHT=SCREEN_WIDTH/16.0*9;
const int FPS=60;
const float DMGNUM_YSPD=-7.0f;
float bsizemulti=1.0f;
float bdmgmulti=1.0f;
float enemylevel=1.0f;
float shotspeedmulti=1.0f;
float bhealmulti=0.0f;
float blexplodesizmulti=1.0f;
float blexplodedmgmulti=0.1f;
float spread=2.5f;
int bpershotmulti=1;
int bcrossmulti=1;
bool blexplode=0;
bool dmgnum=1;
Sound wxyy,upgr;
Sound bgms[11],kills[11];
Rectangle shop={SCREEN_WIDTH-100,20,90,50};
unsigned long long gametime=0;
enum BulletType{
	NONE,
	FIRE,
	ICE,
	LIGHTNING
}bulletType=NONE;
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
float btypedurat=0.0f;
int btypecnt=0;
float btypedmgmulti=0.0f,btypeefc=0.0f;
struct Button{
	Rectangle rec;
	string text;
};
struct Character{
	string name;
	int skillindex;
	float maxhealth,spd;
	int skillmaxcd;
}characters[11]={
	{"ZSJY",0,1000,5,FPS*15},
	{"LTY",1,800,6,FPS*6},
	{"CZH",2,1200,4,FPS*10},
	{"ABC",3,900,5,FPS*50},
	{"LDC",4,1500,3.5f,FPS*60/7},
	{"WXYY",5,700,6,FPS*10},
	{"910",6,700,7.5f,FPS*15}
};
struct Bullet{
	Vector2 pos;
	float xspd,yspd;
	int des=FPS*12;
	float spd,dmg;
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
	float health,MaxHealth,spd,siz;
	int type;
	bool isBoss=0,candivid=0;
	float oldspeed;
	float btypedur=0.0f,btypedmg=0.0f,btypeef=0.0f;
	int btypecn=0;
};
struct Item{
	Vector2 pos;
	Upgrade up;
};
struct Particle{
	Vector2 pos,spd;
	Color col;
	float siz;
};
struct DamageNumber{
	float num;
	Vector2 pos,spd;
};
struct Turret{
	Vector2 pos;
	int des=20*FPS;
	int scd;
};
struct Goods{
	Gun gun;
	int cost;
};
//Texture zsTexture=LoadTexture("cat.png");//,ldtexture=LoadTexture("ld.jpg");
vector<Bullet>bullets;
vector<Enemy>enemies;
vector<Item>items;
vector<pair<Vector2,int>>explosions;
vector<Particle>particles;
vector<DamageNumber>damagenumbers;
vector<Turret>turrets;
vector<Goods>goods;
struct Player{
	//Image img=LoadImage("cat.png");
	//Texture tex=LoadTextureFromImage(img);
	Character ch=characters[0];
	Vector2 pos={SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
	int scd=0;
	Gun currentgun={"wxyylikeslgggg",FPS/6,10,0,17};
	vector<Gun>guns={currentgun};
	int gunindex=0;
	float health=ch.maxhealth,MaxHealth=ch.maxhealth;
	int level=1,exp=0,upexp=5;
	int skillCD;
	int gold=0;
	void Keep(){
		pos={Clamp(pos.x,20,SCREEN_WIDTH-20),Clamp(pos.y,20,SCREEN_HEIGHT-20)};
	}
	void Move(){
		
	}
	void Reset(){
		ch=characters[0];gold=0;
		pos={SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
		scd=0;gunindex=0;health=ch.maxhealth,MaxHealth=ch.maxhealth;
		currentgun={"wxyylikeslgggg",FPS/6,10,0,17};
		level=1,exp=0,upexp=5;
	}
}player;
enum GameState{
	START,
	CHOOSE_CHARACTER,
	RUNNING,
	UPGRADING,
	DEAD,
	PAUSE,
	SHOP,
	BULLET_UPGRADING
}gamestate=START;
template<typename T>
bool rmv(const T a){
	return a.des<=0;
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
	float rad = deg * (float)3.141592653 / 180.0f;
	float cosR = cos(rad);
	float sinR = sin(rad);
	float newX = off.x * cosR - off.y * sinR;
	float newY = off.x * sinR + off.y * cosR;
	return Vector2Add((Vector2){newX, newY}, from);
}
void CreateParticle(Vector2 pos,float spd,Color col,int cnt=10,float siz=5){
	int spr=20,deg=0;
	for(int i=1;i<=cnt;i++){
		Vector2 tmp={0,spd-rand()%(int)spd};
		tmp=TurnDeg({0,0},tmp,deg+rand()%(spr+1)-spr/2.0);
		particles.push_back({pos,tmp,col,siz});
		deg+=360.0/cnt;
	}
}
void ReadLeder(){
	
}
void Init(){
	bsizemulti=1.0f;
	bdmgmulti=1.0f;
	enemylevel=1.0f;
	shotspeedmulti=1.0f;
	bhealmulti=0.0f;
	blexplodesizmulti=1.0f;
	blexplodedmgmulti=0.1f;
	spread=2.5f;
	bpershotmulti=1;
	bcrossmulti=1;
	goods={
		{{"MP9",FPS/14,14,1,22},100},
		{{"AWP",int(FPS/1.5f),15,1,152},475},
		{{"LF",FPS/19,19,1,9},91}
	};
	btypedurat=0.0f;
	btypecnt=0;
	btypedmgmulti=0.0f,btypeefc=0.0f;
	gamestate=START;
	bullets.clear();
	enemies.clear();
	items.clear();
	particles.clear();
	damagenumbers.clear();
	turrets.clear();
	player.Reset();
	unsigned long long gametime=0;
}
void EnemyDamaging(Enemy &e,float damage,int &idx){
	e.health-=damage;
	damagenumbers.push_back({(float)-damage,e.pos,{float(rand()%10-5),DMGNUM_YSPD}});
	if(e.health<=0){
		CreateParticle(e.pos,e.spd,RED,10,e.siz/5);
		Sound kil=kills[rand()%2];
		SetSoundPitch(kil,1.0f+float(rand()%10)/10-0.5f);
		PlaySound(kil);
		if(e.candivid){
			Enemy divid;
			float spd=e.spd-1.0f,health=e.MaxHealth*1.5f,siz=e.siz*0.6;
			divid.spd=spd,divid.health=divid.MaxHealth=health;
			divid.siz=siz;
			divid.type=e.type;
			divid.oldspeed=divid.spd;
			for(int k=1;k<=3;k++){
				divid.pos=TurnDeg(e.pos,Vector2Add(e.pos,{0,e.siz}),rand()%360);
				enemies.push_back(divid);
			}
		}
		if(e.isBoss){
			player.exp+=10*enemylevel;
			player.gold+=10*enemylevel;
			gamestate=BULLET_UPGRADING;
		}else{
			player.exp+=enemylevel;
			player.gold+=enemylevel;
		}
		//auto it = std::find(enemies.begin(), enemies.end(), e);
		//enemies.erase(it);
		enemies.erase(enemies.begin()+idx);
		idx--;
		return;
	}
	if(e.type==4){
		e.spd=e.oldspeed=2*enemylevel;
		e.spd=min(10.0f,e.spd);
	}
}
void Draw(){
	//DrawTexture(player.tex,player.pos.x,player.pos.y,WHITE);
	for(auto p:particles){
		DrawCircle(p.pos.x,p.pos.y,p.siz,p.col);
	}
	for(auto b:bullets){
		Color col;
		switch (bulletType) {
		case NONE:
			col=GRAY;
			break;
		case FIRE:
			col=ORANGE;
			break;
		case ICE:
			col=SKYBLUE;
			break;
		case LIGHTNING:
			col=PURPLE;
			break;
		}
		DrawCircle(b.pos.x,b.pos.y,3*bsizemulti,col);
		//CreateParticle(b.pos,1,col,10,1);
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
		case 2:
			DrawText("HYJ",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		case 3:
			DrawText("LDC",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		case 4:
			DrawText("ZZR",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		case 5:
			DrawText("WSP",e.pos.x-10,e.pos.y-5,10,BLACK);
			break;
		}
		DrawRectangle(e.pos.x-20,e.pos.y-30,40,5,GRAY);
		DrawRectangle(e.pos.x-20,e.pos.y-30,1.0*e.health/e.MaxHealth*40,5,GREEN);
	}
	for(auto t:turrets){
		DrawCircleV(t.pos,20,BROWN);
		DrawText(TextFormat("%.d",t.des/60),t.pos.x-10,t.pos.y-5,10,BLACK);
	}
	for(auto it:items){
		DrawCircle(it.pos.x,it.pos.y,10,GREEN);
	}
	for(auto ex:explosions){
		DrawCircle(ex.first.x,ex.first.y,ex.second,YELLOW);
	}
	for(auto dn:damagenumbers){
		string text;
		if(dn.num>0){
			text="+";
		}
		DrawText((text+TextFormat("%.1f",dn.num)).c_str(),dn.pos.x,dn.pos.y,20,BLACK);
	}
	DrawCircle(player.pos.x,player.pos.y,20,BLUE);
	DrawRectangle(player.pos.x-20,player.pos.y-30,40,5,GRAY);
	DrawRectangle(player.pos.x-20,player.pos.y-30,1.0*player.health/player.MaxHealth*40,5,GREEN);
	DrawText(player.ch.name.c_str(),player.pos.x-10,player.pos.y-5,10,BLACK);
	DrawText(TextFormat("Level:%d EXP:%d/%d",player.level,player.exp,player.upexp),10,10,30,BLACK);
	DrawText(TextFormat("Weapon:%s Damage:%.1f Shot Speed:%.1f",
		player.currentgun.name.c_str(),
		player.currentgun.dmg*bdmgmulti,
		FPS/player.currentgun.shootcd*shotspeedmulti),10,50,30,BLACK);
	DrawText(TextFormat("HP:%.1f/%.1f Speed:%.1f",
		player.health,player.MaxHealth,
		player.ch.spd),10,90,30,BLACK);
	DrawFPS(SCREEN_WIDTH/2,50);
}
Rectangle startbutton={SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2-25,150,50};
Rectangle quitbutton={SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2+40,150,50};
Button buttons[11]={
	{{SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2-25,150,50},"Start"},
	{{SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2+40,150,50},"Exit"}
};
void DrawStartMenu(){
	ClearBackground(RAYWHITE);
	DrawText("Zsjy is game",SCREEN_WIDTH/2-100,100,50,BLACK);
	for(int i=0;i<2;i++){
		if(GuiButton(buttons[i].rec,buttons[i].text.c_str())){
			switch (i) {
			case 0:
				gamestate=CHOOSE_CHARACTER;
				break;
			case 1:
				exit(0);
				break;
			}
		}
	}
	/*DrawRectangleRec(startbutton,GRAY);
	DrawRectangleRec(quitbutton,GRAY);
	DrawText("Start",startbutton.x+30,startbutton.y+10,25,BLACK);
	DrawText("Quit",quitbutton.x+30,quitbutton.y+10,25,BLACK);
	if(CheckCollisionPointRec(GetMousePosition(),startbutton)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
	gamestate=RUNNING;
	}else if(CheckCollisionPointRec(GetMousePosition(),quitbutton)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
	exit(0);
	}*/
}
void DrawCharacterMenu(){
	Button chs[108]={
		{{SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2-25,150,50},characters[0].name},
		{{SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2+40,150,50},characters[1].name},
		{{SCREEN_WIDTH/2-75,SCREEN_HEIGHT/2+105,150,50},characters[2].name},
		{{0,0},characters[3].name}
	};
	ClearBackground(RAYWHITE);
	int choose=-1;
	for(int i=0;i<7;i++){
		chs[i].rec={float(100+250*(i%3)),float(100+150*(i/3)),150,50};
		chs[i].text=characters[i].name;
		if(GuiButton(chs[i].rec,chs[i].text.c_str())){
			choose=i;
			gamestate=RUNNING;
		}
	}
	player.ch=characters[choose];
	player.health=player.MaxHealth=player.ch.maxhealth;
}
BulletType blt;
int tmp=rand()%2;
void DrawBulletUpgrade(bool flag){
	switch (bulletType) {
	case NONE:
		ClearBackground(GRAY);
		break;
	case FIRE:
		ClearBackground(ORANGE);
		break;
	case ICE:
		ClearBackground(SKYBLUE);
		break;
	case LIGHTNING:
		ClearBackground(PURPLE);
	}
	DrawText("Boss killed!",100,50,50,BLACK);
	Rectangle rec[3];
	if(!flag){
		blt=BulletType(rand()%3+1);
	}
	if(!flag){
		tmp=rand()%2;
	}
	for(int i=0;i<3;i++){
		rec[i]={100,float(200+200*i),SCREEN_WIDTH-200,100};
		DrawRectangleRec(rec[i],WHITE);
		string text;
		if(i==0){
			if(bulletType==NONE){
				switch (blt) {
				case FIRE:
					text="Bullets can burn enemies";
					break;
				case ICE:
					text="Bullets can slow enemies down";
					break;
				case LIGHTNING:
					text="Bullets can deal damage to near enemies";
					break;
				}
			}else{
				if(tmp==0){
					switch (bulletType) {
					case FIRE:
						text="Increase burning duration";
						break;
					case ICE:
						text="Increase slow duration";
						break;
					case LIGHTNING:
						text="Increase damaging enemies count";
						break;
					}
				}else if(tmp==1){
					switch (bulletType) {
					case FIRE:
						text="Increase burning damage";
						break;
					case ICE:
						text="Increase slow effect";
						break;
					case LIGHTNING:
						text="Increase lightning damage";
						break;
					}
				}
			}
		}else if(i==1){
			text="Increase shot speed";
		}else if(i==2){
			text="Increase bullet damage";
		}
		DrawText(text.c_str(),100,200+200*i,40,BLACK);
	}
	for(int i=0;i<3;i++){
		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
			if(CheckCollisionPointRec(GetMousePosition(),rec[i])){
				if(i==0&&bulletType==NONE){
					bulletType=blt;
					switch(bulletType){
					case FIRE:
						btypedurat=1.0f;
						btypedmgmulti=0.5f;
						break;
					case ICE:
						btypedurat=0.5f;
						btypeefc=0.35f;
						break;
					case LIGHTNING:
						btypedmgmulti=0.2f;
						btypecnt=2;
						break;
					}
				}else if(i==0&&bulletType!=NONE){
					if(tmp==0){
						switch (bulletType) {
						case FIRE:
							btypedurat+=0.1f;
							break;
						case ICE:
							btypedurat+=0.3f;
							break;
						case LIGHTNING:
							btypecnt++;
							break;
						}
					}else if(tmp==1){
						switch (bulletType) {
						case FIRE:
							btypedmgmulti+=0.1f;
							break;
						case ICE:
							btypeefc+=0.1f;
							break;
						case LIGHTNING:
							btypedmgmulti+=0.1f;
							break;
						}
					}
				}else if(i==1){
					shotspeedmulti+=0.3;
				}else if(i==2){
					bdmgmulti+=0.3;
				}
				gamestate=RUNNING;
				break;
			}
		}
	}
}
void Skill(){
	player.skillCD--;
	player.skillCD=max(0,player.skillCD);
	Vector2 playerToMouse = Vector2Subtract(GetMousePosition(), player.pos); // 获取玩家到鼠标的向量
	playerToMouse = Vector2Normalize(playerToMouse); // 归一化，使其长度为1
	Vector2 endpos = Vector2Scale(playerToMouse, 500.0f); 
	if(IsKeyDown(KEY_SPACE)){
		Color col=(player.skillCD>0)?Color({230,41,55,128}):Color({0,121,241,128});
		switch (player.ch.skillindex) {
		case 0:
			DrawCircle(player.pos.x,player.pos.y,400,col);
			break;
		case 1:
		case 3:
			DrawCircleV(GetMousePosition(),20,col);
			break;
		case 2:
			DrawLineEx(player.pos,Vector2Add(player.pos,endpos),40,col);
			break;
		case 5:
			DrawCircleV(GetMousePosition(),200,col);
			break;
		case 6:
			DrawLineEx(player.pos,Vector2Add(player.pos,Vector2Scale(playerToMouse,10000.0f)),5,col);
			break;
		}
	}
	if(IsKeyReleased(KEY_SPACE)&&player.skillCD==0){
		player.skillCD=player.ch.skillmaxcd;
		switch (player.ch.skillindex) {
		case 0:
			for(int j=0;j<enemies.size();j++){
				auto &e=enemies[j];
				if(CheckCollisionCircles(player.pos,400,e.pos,e.siz)){
					//e.health-=10*player.currentgun.dmg*bdmgmulti*bpershotmulti;
					EnemyDamaging(e,10*player.currentgun.dmg*bdmgmulti*bpershotmulti,j);
				}
			}
			break;
		case 1:
			player.pos=GetMousePosition();
			break;
		case 2:
			//Vector2 playerToMouse = Vector2Subtract(GetMousePosition(), player.pos); // 获取玩家到鼠标的向量
			//playerToMouse = Vector2Normalize(playerToMouse); // 归一化，使其长度为1
			//Vector2 endpos = Vector2Scale(playerToMouse, 500.0f); 
			for(int j=0;j<enemies.size();j++){
				auto &e=enemies[j];
				if(CheckCollisionCircleLine(e.pos,e.siz,player.pos,Vector2Add(player.pos,endpos))){
					e.health-=5*player.currentgun.dmg*bdmgmulti*bpershotmulti;
					EnemyDamaging(e,5*player.currentgun.dmg*bdmgmulti*bpershotmulti,j);
				}
			}
			player.pos=Vector2Add(player.pos,endpos);
			break;
		case 3:
			turrets.push_back({GetMousePosition()});
			break;
		case 4:
			player.health+=player.MaxHealth/7;
			damagenumbers.push_back({player.MaxHealth/7,player.pos,{5,DMGNUM_YSPD}});
			break;
		case 5:
			for(int j=0;j<enemies.size();j++){
				auto &e=enemies[j];
				if(CheckCollisionCircles(GetMousePosition(),200,e.pos,e.siz)){
					//e.health-=10*player.currentgun.dmg*bdmgmulti*bpershotmulti;
					e.spd+=0.5f;
					EnemyDamaging(e,e.MaxHealth/2,j);
				}
			}
			break;
		case 6:
			for(int j=0;j<enemies.size();j++){
				auto &e=enemies[j];
				if(CheckCollisionCircleLine(e.pos,e.siz,player.pos,Vector2Add(player.pos,Vector2Scale(playerToMouse,10000.0f)))){
					e.health-=10*player.currentgun.dmg*bdmgmulti*bpershotmulti;
					EnemyDamaging(e,10*player.currentgun.dmg*bdmgmulti*bpershotmulti,j);
				}
			}
			break;
		}
		
	}
}
bool rmv_par(Particle a){
	return Vector2Distance({0,0},a.spd)<0.4;
}
void Move(){
	if(IsKeyDown(KEY_W)){player.pos.y-=player.ch.spd;}
	if(IsKeyDown(KEY_S)){player.pos.y+=player.ch.spd;}
	if(IsKeyDown(KEY_A)){player.pos.x-=player.ch.spd;}
	if(IsKeyDown(KEY_D)){player.pos.x+=player.ch.spd;}
	//CreateParticle(player.pos,2,BLUE);
	for(auto &e:enemies){
		Vector2 eoff=Vector2Subtract(player.pos,e.pos);
		double dist=Vector2Distance(e.pos,player.pos);
		double mult=e.spd/dist*1.0;
		float xs=eoff.x*mult,ys=eoff.y*mult;
		e.pos={e.pos.x+xs,e.pos.y+=ys};
	}
	for(auto &p:particles){
		p.pos=Vector2Add(p.pos,p.spd);
		p.spd=Vector2Scale(p.spd,0.9f);
		p.siz*=0.95f;
		if(rmv_par(p)){
			remove_if(particles.begin(),particles.end(),rmv_par);
			particles.pop_back();
		}
	}
	if(IsKeyPressed(KEY_O)){
		dmgnum=dmgnum?0:1;
	}
	if(!dmgnum)damagenumbers.clear();
	int i=0;
	for(auto &dn:damagenumbers){
		if(dn.spd.y>-DMGNUM_YSPD){
			damagenumbers.erase(damagenumbers.begin()+i);
			continue;
		}
		dn.pos=Vector2Add(dn.pos,dn.spd);
		dn.spd.y+=0.5;
		i++;
	}
}
void ShopBehavior(){
	if(gamestate==SHOP){
		if(IsKeyPressed(KEY_SPACE)){
			player.gold+=100;
		}
		DrawText("Shop",100,50,50,BLACK);
		if(GuiButton(shop,"Exit")){
			gamestate=RUNNING;
		}
		Button chs[108];
		int choose=-1;
		for(int i=0;i<min(5,int(goods.size()));i++){
			chs[i].rec={float(100+250*(i%3)),float(100+150*(i/3)),150,50};
			chs[i].text=goods[i].gun.name;
			if(GuiButton(chs[i].rec,chs[i].text.c_str())&&player.gold>=goods[i].cost){
				choose=i;
				player.guns.push_back(goods[i].gun);
				goods.erase(goods.begin()+i);
				break;
			}
		}
	}else if(gamestate==RUNNING){
		if(GuiButton(shop,"Shop")){
			gamestate=SHOP;
		}
	}
	
}
bool operator==(Enemy a,Enemy b){
	return a.pos==b.pos;
}
bool operator==(pair<Vector2,int> a,pair<Vector2,int> b){
	return a.first==b.first;
}
bool operator==(Bullet a,Bullet b){
	return a.pos==b.pos;
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
	for(int i=0;i<bullets.size();i++){
		auto &b=bullets[i];
		f=0;
		for(int j=0;j<enemies.size();j++){
			auto &e=enemies[j];
			if(CheckCollisionCircles(b.pos,3*bsizemulti,e.pos,e.siz)){
				f=1;
				if(b.active){
					EnemyDamaging(e,b.dmg,j);
					if(bulletType==FIRE){
						e.btypedur=btypedurat*FPS;
						e.btypedmg=b.dmg*bdmgmulti*btypedmgmulti/FPS;
					}else if(bulletType==ICE){
						e.btypedur=btypedurat*FPS;
						e.btypeef=btypeefc;
						e.spd=e.oldspeed*(1.0f-e.btypeef);
					}else if(bulletType==LIGHTNING){
						e.btypecn+=btypecnt;
						e.btypedmg=b.dmg*bdmgmulti*btypedmgmulti;
					}
					if(blexplode){
						explosions.push_back({b.pos,100*blexplodesizmulti});
					}
					player.health+=b.dmg*bhealmulti;
					if(bhealmulti>0)
						damagenumbers.push_back({(float)b.dmg*bhealmulti,player.pos,{float(rand()%10-5),DMGNUM_YSPD}});
					if(player.health>player.MaxHealth){
						player.health=player.MaxHealth;
					}
					b.active=0;
					b.cross--;
					/*if(e.health<=0){
					CreateParticle(e.pos,e.spd,RED,10,e.siz/5);
					Sound kil=kills[rand()%2];
					SetSoundPitch(kil,1.0f+float(rand()%10)/10-0.5f);
					PlaySound(kil);
					if(e.type==5&&e.candivid){
					Enemy divid;
					float spd=e.spd-1.0f,health=e.MaxHealth*1.5f,siz=e.siz*0.6;
					divid.spd=spd,divid.health=divid.MaxHealth=health;
					divid.siz=siz;
					divid.type=e.type;
					divid.oldspeed=divid.spd;
					for(int k=1;k<=4;k++){
					divid.pos=TurnDeg(e.pos,Vector2Add(e.pos,{0,e.siz}),rand()%360);
					enemies.push_back(divid);
					}
					}
					if(e.isBoss){
					player.exp+=10*enemylevel;
					gamestate=BULLET_UPGRADING;
					}else{
					player.exp+=enemylevel;
					}
					//auto it = std::find(enemies.begin(), enemies.end(), e);
					//enemies.erase(it);
					enemies.erase(enemies.begin()+j);
					j--;
					continue;
					}
					if(e.type==4){
					e.spd=e.oldspeed=2*enemylevel;
					e.spd=min(10.0f,e.spd);
					}*/
					if(b.cross<=0){
						//auto it = std::find(bullets.begin(), bullets.end(), b);
						//bullets.erase(it);
						b.des=0;
					}else{
						b.dmg=b.dmg/3;
					}
				}
			}
		}
		if(!f){
			b.active=1;
		}
	}
	for(int i=0;i<enemies.size();i++){
		auto &e=enemies[i];
		if(bulletType==ICE&&e.btypedur<=0){
			e.spd=e.oldspeed;
		}
		if(bulletType==FIRE&&e.btypedur>0){
			e.health-=e.btypedmg;
			damagenumbers.push_back({(float)-e.btypedmg,e.pos,{float(rand()%10-5),DMGNUM_YSPD}});
			CreateParticle(e.pos,3,ORANGE);
		}else if(bulletType==ICE&&e.btypedur>0){
			CreateParticle(e.pos,3,SKYBLUE);
		}else if(bulletType==LIGHTNING&&e.btypecn){
			for(int i=0;i<(int)enemies.size();i++){
				auto &en=enemies[i];
				if(CheckCollisionCircles(e.pos,e.siz*10,en.pos,en.siz)){
					en.health-=e.btypedmg;
					damagenumbers.push_back({(float)-e.btypedmg,e.pos,{float(rand()%10-5),DMGNUM_YSPD}});
					DrawLineEx(e.pos,en.pos,5,PURPLE);
				}
				e.btypecn--;
				if(e.btypecn<=0)break;
			}
		}
		e.btypedur--;
		e.btypedur=max(0.0f,e.btypedur);
		for(auto &en:enemies){
			if(en.type==3){
				if(CheckCollisionCircles(en.pos,en.siz*3,e.pos,e.siz)){
					e.health+=enemylevel*2/FPS;
					CreateParticle(e.pos,3,GREEN);
					if(e.health>e.MaxHealth){
						e.health=e.MaxHealth;
					}
				}
			}
		}
		if(CheckCollisionCircles(player.pos,20,e.pos,e.siz)){
			player.health-=e.health;
			damagenumbers.push_back({(float)-e.health,player.pos,{float(rand()%10-5),DMGNUM_YSPD}});
			if(e.type==4){
				player.health-=e.health;
				damagenumbers.push_back({(float)-e.health,player.pos,{float(rand()%10-5),DMGNUM_YSPD}});
			}
			if(player.health<=0){
				gamestate=DEAD;
				return;
			}
			enemies.erase(enemies.begin()+i);
			i--;
			/*auto it = std::find(enemies.begin(), enemies.end(), e);
			if (it != enemies.end()) {
			enemies.erase(it);
			continue;
			}*/
		}
	}
	for(int i=0;i<turrets.size();i++){
		Turret &t=turrets[i];
		for(int j=0;j<enemies.size();j++){
			Enemy &e=enemies[i];
			if(CheckCollisionCircles(e.pos,e.siz,t.pos,20)){
				t.des-=e.health/100*FPS;
				enemies.erase(enemies.begin()+j);
				j--;
				if(t.des<0){
					turrets.erase(turrets.begin()+i);
					i--;
					break;
				}
				continue;
			}
		}
	}
}
void GenerateEnemy(int times){
	if(times%int(FPS*2/enemylevel)==0||times%int(FPS*60.0)==0||IsKeyPressed(KEY_KP_0)){
		int dir=rand()%4;
		float x,y;
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
		int type=rand()%6;
		float spd,health,siz;
		bool cn=0;
		switch (type) {
		case 0:
			spd=4,health=100,siz=20;
			break;
		case 1:
			spd=5,health=50,siz=15;
			break;
		case 2:
			spd=3,health=150,siz=25;
			break;
		case 3:
			spd=4,health=100,siz=23;
			break;
		case 4:
			spd=6,health=35,siz=13;
			break;
		case 5:
			spd=6,health=30,siz=15+rand()%10;
			cn=1;
			break;
		}
		spd*=enemylevel,health*=enemylevel*enemylevel,siz*=enemylevel;
		spd=min(spd,7.0f);
		siz=min(siz,40.0f);
		Enemy e;
		/**/if(times%int(FPS*60.0)==0||IsKeyPressed(KEY_KP_0)){
			spd=1,health=health*15,siz=50*enemylevel;
			e.isBoss=1;
			cn=0;
		}
		e.pos={x,y};
		e.health=e.MaxHealth=health;
		e.spd=spd;
		e.siz=siz;
		e.type=type;
		e.candivid=cn;
		e.oldspeed=e.spd;
		enemies.push_back(e);
		enemylevel+=0.005;
	}
}
void Shoot(){
	if(IsKeyPressed(KEY_Q)){
		player.gunindex--;
		player.gunindex=max(0,player.gunindex);
		player.currentgun=player.guns[player.gunindex];
		player.scd=player.currentgun.shootcd/shotspeedmulti;
	}
	if(IsKeyPressed(KEY_E)){
		player.gunindex++;
		player.gunindex=min((int)player.guns.size()-1,player.gunindex);
		player.currentgun=player.guns[player.gunindex];
		player.scd=player.currentgun.shootcd/shotspeedmulti;
	}
	player.scd--;
	player.scd=(player.scd<0)?0:player.scd;
	for(auto &b:bullets){
		b.pos={b.pos.x+b.xspd,b.pos.y+b.yspd};
		b.des--;
		bullets.erase(remove_if(bullets.begin(),bullets.end(),rmv<Bullet>),bullets.end());
	}
	for(auto &t:turrets){
		t.scd--;
		t.scd=max(0,t.scd);
		t.des--;
		turrets.erase(remove_if(turrets.begin(),turrets.end(),rmv<Turret>),turrets.end());
		if(t.scd==0){
			float mindist=100000;
			int minidx=-1;
			int i=0;
			for(auto e:enemies){
				if(Vector2Distance(e.pos,t.pos)<mindist){
					mindist=Vector2Distance(e.pos,t.pos);
					minidx=i;
				}
				i++;
			}
			if(enemies.size()>0){
				Vector2 mousepos=enemies[minidx].pos;
				Vector2 moff={mousepos.x-t.pos.x,mousepos.y-t.pos.y};
				double dist=sqrt(moff.x*moff.x+moff.y*moff.y);
				double mult=player.currentgun.bulletspeed/dist*1.0;
				float xs=moff.x*mult,ys=moff.y*mult;
				Bullet b={t.pos,xs,ys};
				b.spd=player.currentgun.bulletspeed;
				b.dmg=player.currentgun.dmg*bdmgmulti;
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
				t.scd=player.currentgun.shootcd/shotspeedmulti;
			}
			
		}
	}
	if((player.currentgun.continuous?IsMouseButtonDown(MOUSE_BUTTON_LEFT):IsMouseButtonPressed(MOUSE_BUTTON_LEFT))&&player.scd==0){
		Vector2 mousepos=GetMousePosition();
		Vector2 moff={mousepos.x-player.pos.x,mousepos.y-player.pos.y};
		double dist=sqrt(moff.x*moff.x+moff.y*moff.y);
		double mult=player.currentgun.bulletspeed/dist*1.0;
		float xs=moff.x*mult,ys=moff.y*mult;
		Bullet b={player.pos,xs,ys};
		b.spd=player.currentgun.bulletspeed;
		b.dmg=player.currentgun.dmg*bdmgmulti;
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
	for(int i=1;i<=4;i++){
		if(IsKeyPressed(i+48)){
			for(int j=1;j<=4;j++){
				StopSound(musics[j-1]);
			}
			PlaySound(musics[i-1]);
		}
	}
	if(IsKeyPressed(KEY_ZERO)){
		for(int i=1;i<=4;i++){
			StopSound(musics[i-1]);
		}
	}
}
void LoadSongs(Sound bgms[],string files,int num){
	for(int i=1;i<=num;i++){
		string file=files;
		if(i<10)file+="0";
		file+=to_string(i)+".mp3";
		bgms[i-1]=LoadSound(file.c_str());
		SetSoundVolume(bgms[i-1],0.5f);
	}
}
void UnloadSongs(Sound bgms[],int num){
	for(int i=1;i<=num;i++){
		UnloadSound(bgms[i-1]);
	}
}
void HandleUpgrade(){
	if(IsKeyDown(KEY_M)&&IsKeyDown(KEY_A)&&IsKeyDown(KEY_N)){
		player.level=114;
		player.health=player.MaxHealth=114514;
		bpershotmulti=5;
		bdmgmulti=10;
		bhealmulti=10;
		bcrossmulti=10;
		player.currentgun={"MAN",FPS/60,14,1,100};
		player.guns.push_back(player.currentgun);
	}
	else if(IsKeyDown(KEY_SIX)&&IsKeyDown(KEY_ONE)){
		player.currentgun={"sixone",FPS/60,6,1,99};
		player.guns.push_back(player.currentgun);
		player.gunindex=player.guns.size()-1;
	}
	if(player.exp>=player.upexp){
		SetSoundPitch(upgr,1.0f+float(rand()%10)/10-0.5f);
		PlaySound(upgr);
		player.exp-=player.upexp;
		player.level++;
		player.upexp=3+2*player.level;
		gamestate=UPGRADING;
		switch (player.level) {
		case 3:
			player.currentgun={"Deagle",FPS/8,13,0,20};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		case 5:
			player.currentgun={"wxyyalg",FPS/7,10,1,16};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		case 9:
			player.currentgun={"Uzi",FPS/8,14,1,15};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		case 13:
			player.currentgun={"MP7",FPS/10,14,1,13};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		case 17:
			player.currentgun={"AK47",FPS/9,12,1,16};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		case 22:
			player.currentgun={"M250",FPS/12,13,1,20};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		case 30:
			bpershotmulti+=2;
			player.currentgun={"AA12",FPS/15,14,1,25};
			player.guns.push_back(player.currentgun);
			player.gunindex=player.guns.size()-1;
			break;
		}
	}
}
bool operator==(Item a,Item b){
	return a.pos==b.pos;
}
void GenerateItems(int times){
	if(times%int(FPS*20)==0){
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
				player.health+=player.MaxHealth/5;
				break;
			case INCREASE_BULLET_SIZE:
				bsizemulti+=0.1;
				break;
			case INCREASE_MAX_HEALTH:
				player.health+=player.MaxHealth/25*enemylevel*enemylevel;
				player.MaxHealth+=player.MaxHealth/25*enemylevel*enemylevel;
				break;
			case INCREASE_SPEED:
				player.ch.spd+=0.2;
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
				bdmgmulti-=(1.0/(bpershotmulti-1))-(1.0/bpershotmulti);
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
	DrawText("Upgrade!",100,50,50,BLACK);
	if(!flag){
		for(int i=0;i<3;i++){
			upgrade[i]=ups[rand()%UpgradingSize];
		}
	}
	Rectangle rec[3];
	for(int i=0;i<3;i++){
		rec[i]={100,float(200+200*i),SCREEN_WIDTH-200,100};
		DrawRectangleRec(rec[i],WHITE);
		string text;
		switch (upgrade[i]) {
		case FULL_HEAL:
			text="Heal to max health\n";
			text+=TextFormat("%.1f -> %.1f",player.health,player.MaxHealth);
			break;
		case INCREASE_BULLET_SIZE:
			text="Increase bullet size\n";
			text+=TextFormat("%.1f -> %.1f",3*bsizemulti,3*(bsizemulti+0.5));
			break;
		case INCREASE_MAX_HEALTH:
			text="Increase max health\n";
			text+=TextFormat("%.1f -> %.1f",player.MaxHealth,player.MaxHealth+200*enemylevel*enemylevel);
			break;
		case INCREASE_SPEED:
			text="Increase speed\n";
			text+=TextFormat("%.1f -> %.1f",player.ch.spd,player.ch.spd+1);
			break;
		case INCREASE_BULLET_DAMAGE:
			text="Increase bullet damage\n";
			text+=TextFormat("%.1f -> %.1f",player.currentgun.dmg*bdmgmulti,player.currentgun.dmg*(bdmgmulti+0.1));
			break;
		case INCREASE_SHOT_SPEED:
			text="Increase shot speed\n";
			text+=TextFormat("%.1f -> %.1f",FPS/player.currentgun.shootcd*shotspeedmulti,FPS/player.currentgun.shootcd*(shotspeedmulti+0.3));
			break;
		case HEALING_BULLETS:
			text="Bullets heal when hit enemies\n";
			text+=TextFormat("%.1f -> %.1f",bhealmulti,bhealmulti+0.1);
			break;
		case INCREASE_BULLET_CROSS:
			text="Bullets pass through more enemies\n";
			text+=TextFormat("%d -> %d",bcrossmulti,bcrossmulti+1);
			break;
		case ADD_BULLETS_PER_SHOT:
			text="Add bullets per shot, but dereases damage\n";
			text+=TextFormat("%d -> %d %.1f -> %.1f",bpershotmulti,bpershotmulti+1,player.currentgun.dmg*bdmgmulti,player.currentgun.dmg*(bdmgmulti-(1.0/(bpershotmulti))+(1.0/(bpershotmulti+1))));
			break;
		}
		DrawText(text.c_str(),100,200+200*i,40,BLACK);
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
					player.ch.spd+=1;
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
					bdmgmulti-=(1.0/(bpershotmulti-1))-(1.0/bpershotmulti);
					break;
				}
				gamestate=RUNNING;
				break;
			}
			
		}
	}
}
void RandomEvent(int times){
	if(times%100==0){
		
	}
}
int main(){
	srand(time(0));
	InitAudioDevice();
	upgr=LoadSound("sounds/sound_upgrade.mp3");
	SetSoundVolume(wxyy, 1.0f);
	SetTargetFPS(FPS);
//	Sound bgm=LoadSound("sya.mp3");
	LoadSongs(bgms,"songs/bgm_",0);
	LoadSongs(kills,"sounds/sound_kill_",0);
	SetSoundVolume(bgms[0],0.1f);
	SetSoundVolume(bgms[1],0.6f);
	SetSoundVolume(bgms[2],0.2f);
	SetSoundVolume(bgms[3],0.4f);
	SetSoundVolume(kills[1],0.3f);
//	PlaySound(bgm);
	Init();
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"zsjy is game 张顺甲鱼是游戏");
	bool flag=0;
	PlaySound(bgms[rand()%4]);
	GuiSetStyle(DEFAULT,TEXT_SIZE,20);
	random_shuffle(goods.begin(),goods.end());
	while(!WindowShouldClose()){
		if(gamestate==START){
			BeginDrawing();
			DrawStartMenu();
			EndDrawing();
		}else if(gamestate==CHOOSE_CHARACTER){
			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawCharacterMenu();
			EndDrawing();
		}else if(gamestate==RUNNING){
			gametime++;
			flag=0;
			GenerateEnemy(gametime);
			GenerateItems(gametime);
			CheckExplosion();
			ShopBehavior();
			PlayMusic(bgms);
			Damage();
			if(gamestate==RUNNING){
				HandleUpgrade();
			}
			Move();
			player.Keep();
			Shoot();
			if(IsKeyPressed(KEY_P)){
				gamestate=PAUSE;
			}
			BeginDrawing();
			ClearBackground(RAYWHITE);
			Skill();
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
			if(IsKeyPressed(KEY_R)){
				Init();
			}
			BeginDrawing();
			ClearBackground(RED);
			DrawText("Game over",SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2-25,50,BLACK);
			DrawText("Press R to restart",SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2+50,30,BLACK);
			DrawText("Press Space to quit",SCREEN_WIDTH/2-100,SCREEN_HEIGHT/2+100,30,BLACK);
			EndDrawing();
		}else if(gamestate==SHOP){
			ShopBehavior();
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
		}else if(gamestate==BULLET_UPGRADING){
			BeginDrawing();
			DrawBulletUpgrade(flag);
			flag=1;
			EndDrawing();
		}
	}
	UnloadSound(upgr);
	UnloadSongs(bgms,0);
	UnloadSongs(kills,0);
//	UnloadImage(player.img);
	return 0;
}
