#include <iostream>
#include <string>
#include <vector>
#include <raylib.h>
#include <random>
#include <cmath>
using namespace std;



/* 
attempt of quad tree !!!!!
+---+---+---+---+---------------+
|   |   |   |   |               |
+---+---+---+---+               |
|   |   |   |   |               |
+---+---+---+---+               |
|       |       |               |
|       |       |               |
|       |       |               |
+---+---+---+---+---------------+
|   |   |       |               |
+---+---+       |               |
|   |   |       |               |
+---+---+---+---+               |
|       |       |               |
|       |       |               |
|       |       |               |
+-------+-------+---------------+
*/ 



float dt = 0.005;
//float theta = 1; //not in use
float grav_constant = 10;





class star {
    public:
        float position_x ;
        float position_y ;
        float velocity_x ;
        float velocity_y ;
        int mass;
        
        //costructor
        star(float pos_x, float pos_y, float vel_x, float vel_y){
            position_x = pos_x;
            position_y = pos_y;
            velocity_x = vel_x;
            velocity_y = vel_y;
            mass = 1;
        }
};



class quad {
    public:
        quad* NWchild;
        quad* NEchild;
        quad* SWchild;
        quad* SEchild;
        int position_x; //from NW corner
        int position_y; //from NW corner
        int length_x;
        int length_y; 
        int order;
        float mass_x;
        float mass_y;
        int mass;
        vector<star> stars; //vector of objects

        //constructor
        quad(int Px, int Py, int Lx, int Ly, vector<star> stars_vector, int order_){
            NWchild = nullptr;
            NEchild = nullptr;
            SWchild = nullptr;
            SEchild = nullptr;
            position_x = Px;
            position_y = Py;
            length_x = Lx;
            length_y = Ly;
            order = order_;
            stars = stars_vector; //vector of objects
            mass = 0;
            mass_x = 0;
            mass_y = 0;
            for (const star s : stars) { //at some point should make mass position and mass total depend on children quad mass and mass pos not every star ...otherwise ...slow.
                mass_x = ( mass_x * mass ) + ( s.mass * s.position_x );
                mass_y = ( mass_y * mass ) + ( s.mass * s.position_y );
                mass += s.mass;
                mass_x /= mass;
                mass_y /= mass;
            }
        }

        void split(vector<star> starsNW, vector<star> starsNE, vector<star> starsSW, vector<star> starsSE, int order_){
            NWchild = new quad(position_x,              position_y,              length_x/2, length_y/2, starsNW, order_ + 1 );
            NEchild = new quad(position_x + length_x/2, position_y,              length_x/2, length_y/2, starsNE, order_ + 1 );
            SWchild = new quad(position_x,              position_y + length_y/2, length_x/2, length_y/2, starsSW, order_ + 1 );
            SEchild = new quad(position_x + length_x/2, position_y + length_y/2, length_x/2, length_y/2, starsSE, order_ + 1 );
        }

};



void test_split(quad* quad_ptr){
    if (quad_ptr->stars.size() > 1 && quad_ptr->order < 7){

        //init lists of stars for each child quad
        vector<star> starsNW;
        vector<star> starsNE;
        vector<star> starsSW;
        vector<star> starsSE;



        //redistribution by position 
        for (const star s : quad_ptr->stars) {
            if (s.position_x < quad_ptr->position_x + quad_ptr->length_x/2){
                if (s.position_y < quad_ptr->position_y + quad_ptr->length_y/2){
                    starsNW.push_back(s);
                }
                else{
                    starsSW.push_back(s);
                }
            }

            else{
                if (s.position_y < quad_ptr->position_y + quad_ptr->length_y/2){
                    starsNE.push_back(s);
                }
                else{
                    starsSE.push_back(s);
                }
            }
        }



        //split and redistribute into child quads
        quad_ptr->split(starsNW, starsNE, starsSW, starsSE, quad_ptr->order);



        //check child nodes to see if any further splits need to be done
        test_split(quad_ptr->NWchild);
        test_split(quad_ptr->NEchild);
        test_split(quad_ptr->SWchild);
        test_split(quad_ptr->SEchild);
        }
}





void test_and_draw_quad(quad* quad_ptr){
    if (quad_ptr->NWchild != nullptr){
        //unsigned char brightness = static_cast<unsigned char>( 255 - (30 * quad_ptr->order));
        //DrawLine(quad_ptr->position_x + quad_ptr->length_x/2, quad_ptr->position_y, quad_ptr->position_x + quad_ptr->length_x/2, quad_ptr->position_y + quad_ptr->length_y, {brightness, brightness, brightness, 155 });  //vertical line
        //DrawLine(quad_ptr->position_x, quad_ptr->position_y + quad_ptr->length_y/2, quad_ptr->position_x + quad_ptr->length_x, quad_ptr->position_y + quad_ptr->length_y/2, {brightness, brightness, brightness, 155 });  //horizontal line
        for (const star s : quad_ptr->stars){
            DrawCircle(s.position_x, s.position_y, 1, RED);
        }
        //test_and_draw_quad(quad_ptr->NWchild);
        //test_and_draw_quad(quad_ptr->NEchild);
        //test_and_draw_quad(quad_ptr->SWchild);
        //test_and_draw_quad(quad_ptr->SEchild);
    }
}











void find_accel(quad* quad_ptr, star star_, float* accel_x_ptr, float* accel_y_ptr ){
    //cout << "start3" << endl;
    float distance = sqrt(((star_.position_x - quad_ptr->mass_x)*(star_.position_x - quad_ptr->mass_x)) + ((star_.position_y - quad_ptr->mass_y)*(star_.position_y - quad_ptr->mass_y)));
    if (distance > quad_ptr->length_x){
        float dx = (star_.position_x - quad_ptr->mass_x)/(distance);
        float dy = (star_.position_y - quad_ptr->mass_y)/(distance);
        float grav = grav_constant * quad_ptr->mass / (distance * distance) ;
        *accel_x_ptr -= grav * dx;
        *accel_y_ptr -= grav * dy;
    }
    else {
        if (quad_ptr->NWchild != nullptr){
            find_accel(quad_ptr->NWchild, star_, accel_x_ptr, accel_y_ptr );
            find_accel(quad_ptr->NEchild, star_, accel_x_ptr, accel_y_ptr );
            find_accel(quad_ptr->SWchild, star_, accel_x_ptr, accel_y_ptr );
            find_accel(quad_ptr->SEchild, star_, accel_x_ptr, accel_y_ptr );
            }
        else{
            for (const star star__ : quad_ptr->stars){
                float distance = sqrt(((star_.position_x - star__.position_x)*(star_.position_x - star__.position_x)) + ((star_.position_y - star__.position_y)*(star_.position_y - star__.position_y)));
                if (distance > 1){
                    float dx = (star_.position_x - star__.position_x)/distance;
                    float dy = (star_.position_y - star__.position_y)/distance;
                    float grav = grav_constant * star__.mass / (distance*distance) ;
                    *accel_x_ptr -= grav * dx;
                    *accel_y_ptr -= grav * dy;
                }
            }
        }
    }
}





void update_velocities_and_move(quad* quad_ptr){
    for (star& s : quad_ptr->stars){
        float accel_x = 0;
        float accel_y = 0;
        find_accel(quad_ptr, s, &accel_x , &accel_y);
        cout << accel_x << endl;
        s.velocity_x += dt * accel_x;
        s.velocity_y += dt * accel_y;
        s.position_x += dt * s.velocity_x;
        s.position_y += dt * s.velocity_y; //perhaps do rk4 but... im tired so we got this for now...
    }
}








int main(){



    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, 512);


    int i;
    vector<star> starlist_;
    for(i=0; i<100; i++){
        starlist_.push_back(star(distrib(gen), distrib(gen), 0, 0 ));
    }

    // build initial main quad... same size as NE triangle
    quad root(0, 0, 512, 512, starlist_, 0);


    InitWindow(512, 512, "QUAD_TREE");
    while (!WindowShouldClose()){
        cout << "start_frame" << endl;
        test_split(&root);
        BeginDrawing();
            ClearBackground(BLACK);
            test_and_draw_quad(&root);
        EndDrawing();
        update_velocities_and_move(&root);
        cout << "end_frame" << endl;

    }
    CloseWindow();
    return 0;
}




