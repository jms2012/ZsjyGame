//#include<bits/stdc++.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include<raylib.h>
#include<raymath.h>
using namespace std;
const int SCREEN_WIDTH=1800;
const int SCREEN_HEIGHT=SCREEN_WIDTH/16.0*9;
const int FPS=60;
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
Sound wxyy,upgr;
Sound bgms[11],kills[11];
Rectangle shop={SCREEN_WIDTH-100,20,90,50};
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
struct Bullet{
	Vector2 pos;
	float xspd,yspd;
	int des=FPS*5;
	int spd,dmg;
	bool active=1;
	int cross;
	BulletType btype=bulletType;
	float duration,cnt;
	float damage,effect;
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
//Texture zsTexture=LoadTexture("cat.png");//,ldtexture=LoadTexture("ld.jpg");
vector<Bullet>bullets;
vector<Enemy>enemies;
vector<Item>items;
vector<pair<Vector2,int>>explosions;
vector<Particle>particles;
struct Player{
	//Image img=LoadImage("cat.png");
	//Texture tex=LoadTextureFromImage(img);
	Vector2 pos={SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
	float spd=5;
	int scd=0;
	Gun currentgun={"wxyylikes91999",FPS/6,10,0,17};
	float health=1000,MaxHealth=1000;
	int level=1,exp=0,upexp=5;
	int skillMaxCD=FPS*10,skillCD;
	void Keep(){
		pos={Clamp(pos.x,20,SCREEN_WIDTH-20),Clamp(pos.y,20,SCREEN_HEIGHT-20)};
	}
}player;
enum GameState{
	RUNNING,
	UPGRADING,
	DEAD,
	PAUSE,
	SHOP,
	BULLET_UPGRADING
}gamestate=RUNNING;
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
	for(auto it:items){
		DrawCircle(it.pos.x,it.pos.y,10,GREEN);
	}
	for(auto ex:explosions){
		DrawCircle(ex.first.x,ex.first.y,ex.second,YELLOW);
	}
	DrawCircle(player.pos.x,player.pos.y,20,BLUE);
	DrawRectangle(player.pos.x-20,player.pos.y-30,40,5,GRAY);
	DrawRectangle(player.pos.x-20,player.pos.y-30,1.0*player.health/player.MaxHealth*40,5,GREEN);
	DrawRectangle(player.pos.x-20,player.pos.y-35,40,5,GRAY);
	DrawRectangle(player.pos.x-20,player.pos.y-35,1.0*player.exp/player.upexp*40,5,SKYBLUE);
	DrawText("ZSJY",player.pos.x-10,player.pos.y-5,10,BLACK);
	DrawText(TextFormat("Level:%d EXP:%d/%d",player.level,player.exp,player.upexp),10,10,30,BLACK);
	DrawText(TextFormat("Weapon:%s Damage:%.1f Shot Speed:%.1f",
		player.currentgun.name.c_str(),
		player.currentgun.dmg*bdmgmulti,
		FPS/player.currentgun.shootcd*shotspeedmulti),10,50,30,BLACK);
	DrawText(TextFormat("HP:%.1f/%.1f Speed:%.1f",
		player.health,player.MaxHealth,
		player.spd),10,90,30,BLACK);
	DrawRectangleRec(shop,GRAY);
	DrawText("Shop",SCREEN_WIDTH-100,20,30,BLACK);
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
						btypedurat=0.5f;
						btypedmgmulti=0.2f;
						break;
					case ICE:
						btypedurat=0.5f;
						btypeefc=0.2f;
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
	if(IsKeyDown(KEY_SPACE)){
		if(player.skillCD>0){
			DrawCircle(player.pos.x,player.pos.y,400,{230,41,55,128});
		}else{
			DrawCircle(player.pos.x,player.pos.y,400,{0,121,241,128});
		}
	}
	if(IsKeyReleased(KEY_SPACE)&&player.skillCD==0){
		player.skillCD=player.skillMaxCD;
		int i=0;
		for(int j=0;j<enemies.size();j++){
			auto &e=enemies[j];
			if(CheckCollisionCircles(player.pos,400,e.pos,e.siz)){
				e.health-=10*player.currentgun.dmg*bdmgmulti*bpershotmulti;
				if(e.health<=0){
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
						for(int i=1;i<=4;i++){
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
				}
			}
		}
	}
}
bool rmv_par(Particle a){
	return Vector2Distance({0,0},a.spd)<0.4;
}
void Move(){
	if(IsKeyDown(KEY_W)){player.pos.y-=player.spd;}
	if(IsKeyDown(KEY_S)){player.pos.y+=player.spd;}
	if(IsKeyDown(KEY_A)){player.pos.x-=player.spd;}
	if(IsKeyDown(KEY_D)){player.pos.x+=player.spd;}
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
}
void ShopBehavior(){
	if(CheckCollisionPointRec(GetMousePosition(),shop)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
		gamestate=(gamestate==RUNNING)?SHOP:RUNNING;
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
					e.health-=b.dmg;
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
					if(player.health>player.MaxHealth){
						player.health=player.MaxHealth;
					}
					b.active=0;
					b.cross--;
					if(e.health<=0){
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
							for(int i=1;i<=4;i++){
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
					}
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
			CreateParticle(e.pos,3,ORANGE);
		}else if(bulletType==ICE&&e.btypedur>0){
			CreateParticle(e.pos,3,SKYBLUE);
		}else if(bulletType==LIGHTNING&&e.btypecn){
			for(int i=0;i<(int)enemies.size();i++){
				auto &en=enemies[i];
				if(CheckCollisionCircles(e.pos,e.siz*10,en.pos,en.siz)){
					en.health-=e.btypedmg;
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
			if(e.type==4){
				player.health-=e.health;
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
			spd=1,health=health*10,siz=50;
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
		enemylevel+=0.007;
	}
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
	}
	else if(IsKeyDown(KEY_SIX)&&IsKeyDown(KEY_ONE)){
		player.currentgun={"sixone",FPS/60,6,1,99};
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
			break;
		case 5:
			player.currentgun={"wxyyalg",FPS/7,10,1,16};
			break;
		case 9:
			player.currentgun={"Uzi",FPS/8,14,1,15};
			break;
		case 13:
			player.currentgun={"MP7",FPS/10,14,1,13};
			break;
		case 17:
			player.currentgun={"AK47",FPS/9,12,1,16};
			break;
		case 22:
			player.currentgun={"M250",FPS/12,13,1,20};
			break;
		case 30:
			bpershotmulti+=2;
			player.currentgun={"AA12",FPS/15,14,1,25};
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
			text+=TextFormat("%.1f -> %.1f",player.spd,player.spd+1);
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
					bdmgmulti-=(1.0/(bpershotmulti-1))-(1.0/bpershotmulti);
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
	upgr=LoadSound("sounds/sound_upgrade.mp3");
	SetSoundVolume(wxyy, 1.0f);
	SetTargetFPS(FPS);
//	Sound bgm=LoadSound("sya.mp3");
	LoadSongs(bgms,"songs/bgm_",4);
	LoadSongs(kills,"sounds/sound_kill_",2);
	SetSoundVolume(bgms[0],0.1f);
	SetSoundVolume(bgms[1],0.6f);
	SetSoundVolume(bgms[2],0.2f);
	SetSoundVolume(bgms[3],0.4f);
	SetSoundVolume(kills[1],0.3f);
//	PlaySound(bgm);
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"zsjy is game 张顺甲鱼是游戏");
	bool flag=0;
	PlaySound(bgms[rand()%4]);
	while(!WindowShouldClose()){
		if(gamestate==RUNNING){
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
				system("start main.exe");
				return 0;
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
	UnloadSongs(bgms,4);
	UnloadSongs(kills,2);
//	UnloadImage(player.img);
	return 0;
}
