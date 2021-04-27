#include <Windows.h>
#include <iostream>
#include <stdio.h> 
#include <math.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;


DWORD pid_hl2;
HANDLE pHandle_hl2;

DWORD Pitch = 0x203953F8;
DWORD Yaw = 0x203953FC;
DWORD X = 0x243EE0C4;
DWORD Y = 0x243EE0C8;
DWORD Z = 0x243EE0CC;

DWORD Local_player_address = 0x243B51C4;

DWORD Local_player;

DWORD Recoil_x;
DWORD Recoil_y;
DWORD Local_player_team;
DWORD No_flash;

DWORD Player_base_address = 0x244035C0;

DWORD Player_base;
DWORD Name_base;
DWORD Team_base;
DWORD Health_base;
DWORD X_base;
DWORD Y_base;
DWORD Z_base;
int Delta_player = 0x140;
int name_size = 20;


float yaw;
float pitch;
float player_x;
float player_y;
float player_z;
int local_player_team;
int no_flash;

float recoil_x;
float recoil_y;
float corr_yaw;
float corr_pitch;

double PI = 3.14159265359;

DWORD Point_x = 0x204FDC6C;
DWORD Point_y = 0x204FDC70;
float point_x;
float point_y;

class Vector3
{
	 public:
    	float x, y, z;
    
	 Vector3(float _x, float _y, float _z) {
	      x = _x;
	      y = _y;
	      z = _z;
	    }
	    
	 float distance(Vector3 v){
	 	
	 	float delta_x = x - v.x;
	 	float delta_y = y - v.y;
	 	float delta_z = x - v.z;
	 	
	 	return sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
	 	
	 }
};


class Player
{
	public:
		int name;
		int team;
		int health;
		Vector3 pos;
		float distance_from_player;

	Player(int _name, int _team, int _health, Vector3 _pos){
		name = _name;
		team = _team;
		health = _health;
		pos = _pos;
		
	}
	
	float distance(Player p){
		
		return pos.distance(p.pos);
		
	}

};

//vector< Player > own_team;
vector< Player > enemy_team;


void aim_at(float target_x, float target_y, float target_z){
     
	 float delta_x = target_x - player_x;
	 float delta_y = target_y - player_y;
	 float delta_z = target_z - player_z;
	 
	 float distance = sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
	 
	 
	 float new_yaw = atan2(delta_y, delta_x) * (180 / PI);
	 float new_pitch = -asin(delta_z / distance) * (180 / PI);
	 float diff_yaw = new_yaw - yaw;
	 
	 yaw = new_yaw;
	 pitch = new_pitch;
	 
	 
	 Recoil_x = Local_player + 0xBB0;
	 Recoil_y = Local_player + 0xBB4;
     ReadProcessMemory(pHandle_hl2, (void*)Recoil_x, &recoil_x, sizeof(float), 0);
     ReadProcessMemory(pHandle_hl2, (void*)Recoil_y, &recoil_y, sizeof(float), 0);
	 
	 //cout << recoil_x << "   |   " << recoil_y << endl;
	 
	 corr_yaw = yaw - 2*recoil_y;
	 corr_pitch = pitch - 2*recoil_x;
	 
	 
	 //cout << diff_yaw;
	 
	 if (abs(diff_yaw) < 1000.0){
	 
	 WriteProcessMemory(pHandle_hl2, (LPVOID)Yaw, &corr_yaw, sizeof(float), 0);
	 WriteProcessMemory(pHandle_hl2, (LPVOID)Pitch, &corr_pitch, sizeof(float), 0);
     }

     }

void get_players(){
     
    enemy_team.clear();
    //own_team.clear();
	
	int name;
	int team;
	int health;
	float x;
	float y;
	float z;
	
	short int enemy_team_number;

	int counter = 0x00;
	
	//cout << "Reading player data..." << endl;

    for (short int i = 0; i < 64; i++){
          
        
		ReadProcessMemory(pHandle_hl2, (void*)(Name_base + counter*0x140), &name, sizeof(int), 0);
		//cout << name << endl;
		ReadProcessMemory(pHandle_hl2, (void*)(Team_base + counter*0x140), &team, sizeof(int), 0);
		ReadProcessMemory(pHandle_hl2, (void*)(Health_base + counter*0x140), &health, sizeof(int), 0);
		ReadProcessMemory(pHandle_hl2, (void*)(X_base + counter*0x140), &x, sizeof(float), 0);
		ReadProcessMemory(pHandle_hl2, (void*)(Y_base + counter*0x140), &y, sizeof(float), 0);
		ReadProcessMemory(pHandle_hl2, (void*)(Z_base + counter*0x140), &z, sizeof(float), 0);
		

		
		Local_player_team = Local_player + 0x90;
		ReadProcessMemory(pHandle_hl2, (void*)Local_player_team, &local_player_team, sizeof(int), 0);
		enemy_team_number = (local_player_team == 3) ? 2 : 3;
		//cout << Player_base << endl;
		
		//cout << "Player's team: "<< local_player_team << endl;
		
        if (team == enemy_team_number && health > 0 && x != 0 && y != 0 && z != 0){
			enemy_team.push_back(Player(name, team, health, x, y, z));
			
		/*
		cout <<"Name: "<< name << endl;
		cout <<"Team: "<< team << endl;
		cout <<"Health: "<< health << endl;
		cout <<"X: "<< x << endl;
		cout <<"Y: "<< y << endl;
		cout <<"Z: "<< z << endl;
		cout << endl;
		*/
        }


		counter = counter + 0x01;
	}

}

bool comparePlayers(Player p1, Player p2) 
{ 
    return (p1.distance_from_player < p2.distance_from_player); 
}

void disable_flash(){
	No_flash = Local_player + 0xFD0;
	ReadProcessMemory(pHandle_hl2, (void*)No_flash, &no_flash, sizeof(int), 0);
	
	//cout << no_flash << endl;
	
	if (no_flash > 0){
		no_flash = 0;
		WriteProcessMemory(pHandle_hl2, (LPVOID)No_flash, &no_flash, sizeof(int), 0);
	}
} 


int main(){
	

    HWND hwnd = FindWindowA(0, ("Counter-Strike Source"));
    
    GetWindowThreadProcessId(hwnd, &pid_hl2);
    pHandle_hl2 = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid_hl2);
    
    
    
    ReadProcessMemory(pHandle_hl2, (void*)Player_base_address, &Player_base, sizeof(int), 0);
    
    Name_base = Player_base + 0x38;
	Team_base = Player_base + 0x58;
	Health_base = Player_base + 0x5C;
	X_base = Player_base + 0x60;
	Y_base = Player_base + 0x64;
	Z_base = Player_base + 0x68;
    
    
    for(;;)
    {
    	


    ReadProcessMemory(pHandle_hl2, (void*)Pitch, &pitch, sizeof(float), 0);
    ReadProcessMemory(pHandle_hl2, (void*)Yaw, &yaw, sizeof(float), 0);

    ReadProcessMemory(pHandle_hl2, (void*)X, &player_x, sizeof(float), 0);
    ReadProcessMemory(pHandle_hl2, (void*)Y, &player_y, sizeof(float), 0);
    ReadProcessMemory(pHandle_hl2, (void*)Z, &player_z, sizeof(float), 0);
    
    
    ReadProcessMemory(pHandle_hl2, (void*)Local_player_address, &Local_player, sizeof(int), 0);
    
    
    disable_flash();
    
    //WriteProcessMemory(pHandle_hl2, (LPVOID)Recoil_x, 0, sizeof(float), 0);
    //WriteProcessMemory(pHandle_hl2, (LPVOID)Recoil_y, 0, sizeof(float), 0);
    
    


    if (GetAsyncKeyState(VK_SPACE))
    {
	  
       get_players();
       
       
      //cout << "Enemy team: " << enemy_team.size() <<endl;
      //cout << "Own team: " << own_team.size() <<endl;
       
       if (enemy_team.size() != 0){
        sort(enemy_team.begin(), enemy_team.end(), comparePlayers);
        
        aim_at(enemy_team[0].x, enemy_team[0].y, enemy_team[0].z);
        
        /*
        cout << "--------------------------------------------" << endl;
        for (short int j = enemy_team.size()-1; j>-1; j--){
        	cout <<"Name: "<< enemy_team[j].name << endl;
			cout <<"Team: "<< enemy_team[j].team << endl;
			cout <<"Health: "<< enemy_team[j].health << endl;
			cout <<"X: "<< enemy_team[j].x << endl;
			cout <<"Y: "<< enemy_team[j].y << endl;
			cout <<"Z: "<< enemy_team[j].z << endl;
			cout << endl;
		}
		*/
         
		 //cout <<"Closest enemy at: "<< 
         //cout << enemy_team[1].name << "; " << enemy_team[1].x << "; " << enemy_team[1].y << "; " << enemy_team[1].z << endl;
       /*
       DWORD XX = 0x0EA154D4;
       DWORD YY = XX + 0x04;
       DWORD ZZ = XX + 0x08;
       
       float xx;
       float yy;
       float zz;
       
        ReadProcessMemory(pHandle_hl2, (void*)XX, &xx, sizeof(float), 0);
    	ReadProcessMemory(pHandle_hl2, (void*)YY, &yy, sizeof(float), 0);
    	ReadProcessMemory(pHandle_hl2, (void*)ZZ, &zz, sizeof(float), 0);
       
         
         */
         //aim_at(xx, yy, zz);
       }
       
    }  
    /*
    if (GetAsyncKeyState(0x54)){
                                   
  		ReadProcessMemory(pHandle_hl2, (void*)Point_x, &point_x, sizeof(float), 0);
		ReadProcessMemory(pHandle_hl2, (void*)Point_y, &point_y, sizeof(float), 0);  
       
          //aim_at(player_x+100, player_y+100, player_z);
          aim_at(point_x, point_y, player_z);
       }
       */
    
   
    
    }
    
    
    
    return 0;
    
}
