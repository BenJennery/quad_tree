#include <iostream>
#include <string>
#include <vector>
#include <raylib.h>
#include <random>
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
+---+---+       +               |
|   |   |       |               |
+---+---+---+---+               |
|       |       |               |
|       |       |               |
+-------+-------+---------------+
*/ 



class star {
    public:
        float position_x ;
        float position_y ;
        float velocity_x ;
        float velocity_y ;
        
        //costructor
        star(float pos_x, float pos_y, float vel_x, float vel_y){
            position_x = pos_x;
            position_y = pos_y;
            velocity_x = vel_x;
            velocity_y = vel_y;
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
        vector<star> stars; //vector of objects

        //constructor
        quad(int Px, int Py, int Lx, int Ly, vector<star> stars_vector){
            NWchild = nullptr;
            NEchild = nullptr;
            SWchild = nullptr;
            SEchild = nullptr;
            position_x = Px;
            position_y = Py;
            length_x = Lx;
            length_y = Ly;
            stars = stars_vector; //vector of objects
        }

        void split(vector<star> starsNW, vector<star> starsNE, vector<star> starsSW, vector<star> starsSE){
            NWchild = new quad(position_x,              position_y,              length_x/2, length_y/2, starsNW );
            NEchild = new quad(position_x + length_x/2, position_y,              length_x/2, length_y/2, starsNE );
            SWchild = new quad(position_x,              position_y + length_y/2, length_x/2, length_y/2, starsSW );
            SEchild = new quad(position_x + length_x/2, position_y + length_y/2, length_x/2, length_y/2, starsSE );
        }

        void merge(){
            NWchild = nullptr;
            NEchild = nullptr;
            SWchild = nullptr;
            SEchild = nullptr;
        }
};



void test_split(quad* quad_ptr){
    if (quad_ptr->stars.size() > 1){

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
        quad_ptr->split(starsNW, starsNE, starsSW, starsSE);



        //check child nodes to see if any further splits need to be done
        test_split(quad_ptr->NWchild);
        test_split(quad_ptr->NEchild);
        test_split(quad_ptr->SWchild);
        test_split(quad_ptr->SEchild);
        }
}





void test_and_draw_quad(quad* quad_ptr){
    if (quad_ptr->NWchild != nullptr){
        DrawLine(quad_ptr->position_x + quad_ptr->length_x/2, quad_ptr->position_y, quad_ptr->position_x + quad_ptr->length_x/2, quad_ptr->position_y + quad_ptr->length_y, WHITE);  //vertical line
        DrawLine(quad_ptr->position_x, quad_ptr->position_y + quad_ptr->length_y/2, quad_ptr->position_x + quad_ptr->length_x, quad_ptr->position_y + quad_ptr->length_y/2, WHITE);  //horizontal line
        for (const star s : quad_ptr->stars){
            DrawCircle(s.position_x, s.position_y, 1, RED);
        }
        test_and_draw_quad(quad_ptr->NWchild);
        test_and_draw_quad(quad_ptr->NEchild);
        test_and_draw_quad(quad_ptr->SWchild);
        test_and_draw_quad(quad_ptr->SEchild);
    }
}



void init_and_draw(quad* quad_ptr){
    InitWindow(512, 512, "QUAD_TREE");
    while (!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(BLACK);
            test_and_draw_quad(quad_ptr);
        EndDrawing();
    }
}






int main(){



    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, 512);



    //make triangle of stars in north east corner...no vel
    int i;
    vector<star> starlist_;
    for(i=0; i<256; i++){
        starlist_.push_back(star(distrib(gen), distrib(gen), 0, 0 ));
    }

    // build initial main quad... same size as NE triangle
    quad root(0, 0, 512, 512, starlist_);

    // start the split
    test_split(&root);


    init_and_draw(&root);
    CloseWindow();
    return 0;
}








//TODO list, combine split and test_split functions, current solution works for now while testing.
//re-combine stars list in merge function
//mass stuff perhaps