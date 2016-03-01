/* LSViewer.cpp

   B. Bird - 02/08/2016
*/
#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "LSystem.h"
#include "matrix.h"
//#include "colourRGB.h"
#include "transformed_renderer.h"

using namespace std;

static const int DEFAULT_SIZE_X = 800;
static const int DEFAULT_SIZE_Y = 600;


class A3Canvas{
public:
    float *leaf_vx,*leaf_vy;
    static const unsigned int leaf_verts = 8;
    int WINDOW_SIZE_X, WINDOW_SIZE_Y;
    
	A3Canvas(LSystem* L){
        WINDOW_SIZE_X = DEFAULT_SIZE_X;
        WINDOW_SIZE_Y = DEFAULT_SIZE_Y;
		float vx[] = {0,1.0 ,1.25,   1,  0,  -1,-1.25,-1};
		float vy[] = {0,0.75,1.75,2.75,4.0,2.75, 1.75,0.75};
		LS_iterations = 0;
		this->L_system = L;
        leaf_vx = new float[8];
        leaf_vy = new float[8];
        for(int i=0;i<8;i++){
            leaf_vx[i] = vx[i];
            leaf_vy[i] = vy[i];
        }
        num_trees = 1;
	}

	
	void frame_loop(SDL_Renderer* r, SDL_Window* w){
		unsigned int last_frame = SDL_GetTicks();
		//unsigned int frame_number = 0;
		draw(r,0);
		while(1){
			//cout << "Frame " << frame_number << endl;
			unsigned int current_frame = SDL_GetTicks();
			unsigned int delta_ms = current_frame - last_frame;
			
			SDL_Event e;
			//Handle all queued events
			while(SDL_PollEvent(&e)){
				switch(e.type){
					case SDL_QUIT:
						//Exit immediately
						return;
					case SDL_KEYDOWN:
						//e.key stores the key pressed
						handle_key_down(e.key.keysym.sym);
						draw(r,delta_ms);
						break;
                    case SDL_WINDOWEVENT:
                        if(resized(e.window)){
                            SDL_RenderPresent(r);
                            draw(r,delta_ms);
                        }
                        break;
					default:
						break;
				}
			}
			
			
			
		}
		
	}
private:
	int LS_iterations, num_trees;
	LSystem* L_system;
	void handle_key_down(SDL_Keycode key){
		if (key == SDLK_UP){
			LS_iterations++;
		}else if (key == SDLK_DOWN){	
			LS_iterations--;
			if (LS_iterations < 0)
				LS_iterations = 0;
		}
        else if(key == SDLK_RIGHT){
            num_trees++;
        }
        else if(key == SDLK_LEFT){
            num_trees--;
            if(num_trees < 1)   num_trees = 1;
        }
	}
    bool resized(SDL_WindowEvent e){
        if(e.event == SDL_WINDOWEVENT_SIZE_CHANGED){
            WINDOW_SIZE_X = e.data1;
            WINDOW_SIZE_Y = e.data2;
            return true;
        }
        else return false;
    }
	
	
	inline Matrix3 Rotation(float radians){
		Matrix3 M;
		M.identity();
		M(0,0) = M(1,1) = cos(radians);
		M(1,0) = -(M(0,1) = sin(radians));
		return M;
	}
	inline Matrix3 Translation(float tx, float ty){
		Matrix3 M;
		M.identity();
		M(0,2) = tx;
		M(1,2) = ty;
		return M;
	}
	inline Matrix3 Scale(float sx, float sy){
		Matrix3 M;
		M.identity();
		M(0,0) = sx;
		M(1,1) = sy;
		return M;
	}


	void draw_leaf(TransformedRenderer& tr){
		tr.fillPolygon(leaf_vx,leaf_vy,leaf_verts, 64,224,0, 255);
		tr.drawPolygon(leaf_vx,leaf_vy,leaf_verts, 64,128,0, 255);
	}
    void draw_stem(TransformedRenderer& tr){
        tr.fillRectangle(-0.5,0,0.5,6,178,106,45,255);
    }



	void draw(SDL_Renderer *renderer, float frame_delta_ms){
	    stack<Matrix3> t_stack;

		//float frame_delta_seconds = frame_delta_ms/1000.0;

		string ls_string = L_system->GenerateSystemString(LS_iterations);
		//cerr << "Drawing with " << LS_iterations << " iterations." << endl;
		//cerr << "System string: " << ls_string << endl;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
        double window_scale_x = WINDOW_SIZE_X / (double)DEFAULT_SIZE_X;
        double window_scale_y = WINDOW_SIZE_Y / (double)DEFAULT_SIZE_Y;
        double init_scale_x = 6*window_scale_x / double(num_trees/2 + 1);
        double init_scale_y = 6*window_scale_y / double(num_trees/2 + 1);
        if(init_scale_x < 1) init_scale_x = 1;
        if(init_scale_y < 1) init_scale_y = 1;
		TransformedRenderer tr(renderer);
		Matrix3 transform, init_transform;
		transform.identity();
        init_transform.identity();
        init_transform *= Translation(WINDOW_SIZE_X/(num_trees + 1), WINDOW_SIZE_Y);
        init_transform *= Scale(init_scale_x, -init_scale_y);
		
        for(unsigned int i=0; i<num_trees; i++){  
            while(!t_stack.empty()) t_stack.pop();
            transform = init_transform * Translation(i*WINDOW_SIZE_X/(init_scale_x*(num_trees+1)),0);
            tr.set_transform(transform);
            for(unsigned int j=0; j<ls_string.size(); j++){
                switch(ls_string[j]){
                        case 'L':
                            draw_leaf(tr);
                            break;
                        case 'T':
                            draw_stem(tr);
                            transform *= Translation(0,6);
                            break;
                        case '+':
                            transform *= Rotation(M_PI/6); 
                            break;
                        case '-':
                            transform *= Rotation(-M_PI/6); 
                            break;
                        case 's':
                            transform *= Scale(0.9,0.9);
                            break;
                        case 'S':
                            transform *= Scale(1/0.9,1/0.9);
                            break;
                        case 'h':
                            transform *= Scale(0.9,1);
                            break;
                        case 'H':
                            transform *= Scale(1/0.9,1);
                            break;
                        case 'v':
                            transform *= Scale(1,0.9);
                            break;
                        case 'V':
                            transform *= Scale(1,1/0.9);
                            break;
                        case '[':
                            t_stack.push(transform);
                            break;
                        case ']':
                            transform = t_stack.top();
                            t_stack.pop();
                            break;
                        default:
                            break;
                }
                tr.set_transform(transform);
            }
        }
		
	
		SDL_RenderPresent(renderer);
	}
};

int main(int argc, char** argv){

	if (argc < 2){
		cerr << "Usage: " << argv[0] << " <input file>" << endl;
		return 0;
	}
	char* input_filename = argv[1];
	
	LSystem* L = LSystem::ParseFile(input_filename);
	if (!L){
		cerr << "Parsing failed." << endl;
		return 0;
	}

	SDL_Window* window = SDL_CreateWindow("CSC 205 A3",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              DEFAULT_SIZE_X, DEFAULT_SIZE_Y, 
							  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
							  
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0/*SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED*/);

	//Initialize the canvas to solid green
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	A3Canvas canvas(L);

	canvas.frame_loop(renderer, window);
	
	delete L;
	
	return 0;
}
