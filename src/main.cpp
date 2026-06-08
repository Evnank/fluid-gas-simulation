#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <algorithm>
#include <optional>
#include <map>
#include <unordered_map>

std::mt19937 random_engine{std::random_device{}()};


struct GLOBALS{
	sf::View default_view=sf::View(sf::FloatRect({0,0},{1920,1080}));
	float radius=10;
	float collision_distance_sq=radius*radius*4;
	float tick_speed=0.1;  
	//float max_ticks_per_frame=1000;
	float max_ticks_per_frame=tick_speed+10;
	int amount_of_balls_at_spawn=30;
	bool FULLSCREEN_MODE=true;
	float chunk_size=10;
	float chunks_x=192;
	float chunks_y=108;
	int chunk_vector_size=chunks_x*chunks_y;

	float wall_hit_efficiency=1;
	float gravity_power=0;
};
GLOBALS GLOBAL_VARIABLES;

struct ASSETS{
	sf::Font font;
	sf::Texture ball_texture;

	void LOAD_ALL_ASSETS();
};
ASSETS GLOBAL_ASSETS;


struct INPUT{
	sf::Vector2f mouse_true_coords;
	bool Mouse1,Mouse2;

	bool SPACE;

	bool F,R,M;

	bool ESCAPE,LSHIFT,TAB,ENTER,PageUp;
	
	bool F1,F2,F9;

	void read(sf::RenderWindow& window);
};

struct WALL{
	sf::FloatRect rect;
};




struct CHUNK{
	std::vector<int> indexes;
};


struct BALL{
	sf::Vector2f coords{0.f,0.f};
	sf::Vector2f speed{0.f,0.f};
};

struct PERFORMACE_COUNTER{
	int FRAMES_COUNTER=0;
	sf::Clock FPS_CLOCK;
	sf::Time FPS_UPDATE_TIME=sf::milliseconds(500);
	std::string FPS_STRING;

	int UPDATES_COUNTER=0;
	sf::Clock UPS_CLOCK;
	sf::Time UPS_UPDATE_TIME=sf::milliseconds(500);
	std::string UPS_STRING;
	sf::Text FPS_UPS_RENDER_TEXT{GLOBAL_ASSETS.font};
	
	void SETUP();

	void FPS_UPDATE();

	void UPS_UPDATE();

	void DRAW(sf::RenderWindow& window);

};



struct GAME{
	sf::RenderWindow window{sf::VideoMode({ 1920, 1000 }), "fluid simulation",sf::State::Windowed};
	INPUT input;
	std::vector <CHUNK> chunks{size_t(GLOBAL_VARIABLES.chunk_vector_size)};
	std::vector <BALL> balls;
	std::vector <WALL> walls;

	std::uniform_real_distribution<float> rand_x_cord{910.f, 1060.f};
    std::uniform_real_distribution<float> rand_y_cord{460.f, 570.f};

	std::uniform_real_distribution<float> rand_x_vel{-4.f, 4.f};
    std::uniform_real_distribution<float> rand_y_vel{-4.f, 4.f};

	PERFORMACE_COUNTER performance_clocks; 


	
	void RUN();

	void SETUP(){
		LOAD_WALLS();
		performance_clocks.SETUP();
		GLOBAL_ASSETS.LOAD_ALL_ASSETS();
		//window.setVerticalSyncEnabled(true);
		if (GLOBAL_VARIABLES.FULLSCREEN_MODE){window.create(sf::VideoMode({ 1920, 1080 }), "fluid simulation",sf::State::Fullscreen);}
		for (int i=0;i<GLOBAL_VARIABLES.amount_of_balls_at_spawn;i++){
			GENERATE_RANDOM_BALL();
		}
	}	

	void UPDATE_INPUT();

	void UPDATE_PHYSICS();

	void APPLY_GRAVITY();

	void CHUNK_FILLING();

	void BALL_TO_BALL_LOOP();

	void CHECK_CHUNK_TO_CHUNK_COLLISION(CHUNK& chunk1,CHUNK& chunk2){
		float radius=GLOBAL_VARIABLES.radius;
		for (int i=0;i<chunk1.indexes.size();i++){
			BALL& ball1=balls[chunk1.indexes[i]];
			for (int g=0;g<chunk2.indexes.size();g++){


				BALL& ball2=balls[chunk2.indexes[g]];

				if (ball1.coords==ball2.coords){continue;}
				sf::Vector2f between_vector=ball2.coords-ball1.coords;
				float distance_sq=between_vector.lengthSquared();

				if (distance_sq<0.01f){continue;}
				if (distance_sq>GLOBAL_VARIABLES.collision_distance_sq){continue;}
				float distance=(sqrt(distance_sq));
				sf::Vector2f normal=between_vector/distance;
				float vel1new=ball1.speed.dot(normal);
				float vel2new=ball2.speed.dot(normal);
				float vel_add=vel2new-vel1new;
				float overlap=radius*2-distance;
				sf::Vector2f coords_add=normal*overlap/2.f;

				ball1.coords.x-=coords_add.x;
				BALL_TO_WALL_COLLISION_X(ball1);
				ball1.coords.y-=coords_add.y;
				BALL_TO_WALL_COLLISION_Y(ball1);

				ball2.coords.x+=coords_add.x;
				BALL_TO_WALL_COLLISION_X(ball2);
				ball2.coords.y+=coords_add.y;
				BALL_TO_WALL_COLLISION_Y(ball2);
				ball2.coords+=normal*distance/2.f;

				//ball1.speed+=normal*vel_add;
				//ball2.speed-=normal*vel_add;
			}
		}
	}

	void CHECK_OUT_OF_BOUNDS(BALL& cur_ball);

	void BALL_TO_WALL_COLLISION_X(BALL& cur_ball);
	void BALL_TO_WALL_COLLISION_Y(BALL& cur_ball);

	void WALL_AGAINSS_BALL_X(BALL& cur_ball,WALL& cur_wall);
	void WALL_AGAINSS_BALL_Y(BALL& cur_ball,WALL& cur_wall);
	
	void DRAW();

	void DRAW_CURSOR();

	void DRAW_WALLS();

	void DRAW_BALLS();

	void LOAD_WALLS();

	void GENERATE_RANDOM_BALL();
};


















void ASSETS::LOAD_ALL_ASSETS(){
		if (!font.openFromFile("assets/fonts/arial.ttf")){std::cout<<"font failed to load";} 
		if (!ball_texture.loadFromFile("assets/textures/Circle.png")){}
	}









void INPUT::read(sf::RenderWindow& window){
		Mouse1=false;Mouse2=false;
		SPACE=false;
		ESCAPE=false;LSHIFT=false;TAB=false;ENTER=false;PageUp=false;
		F=false;R=false;M=false;
		F1=false;F2=false;F9=false;
		mouse_true_coords=window.mapPixelToCoords(sf::Mouse::getPosition(window));
		while (const std::optional event=window.pollEvent()) {
			if (event->is<sf::Event::Closed>()){window.close();}
				
			if (const auto* key=event->getIf<sf::Event::KeyPressed>()){
				if (key->code == sf::Keyboard::Key::Space){SPACE=true;}

				if (key->code == sf::Keyboard::Key::Escape){ESCAPE=true;}
				if (key->code == sf::Keyboard::Key::LShift){LSHIFT=true;}
				if (key->code == sf::Keyboard::Key::Tab){TAB=true;}
				if (key->code == sf::Keyboard::Key::Enter){ENTER=true;}
				if (key->code == sf::Keyboard::Key::PageUp){PageUp=true;}

				if (key->code == sf::Keyboard::Key::M){M=true;}
				if (key->code == sf::Keyboard::Key::F){F=true;}
				if (key->code == sf::Keyboard::Key::R){R=true;}

				if (key->code == sf::Keyboard::Key::F1){F1=true;}
				if (key->code == sf::Keyboard::Key::F2){F2=true;}
				if (key->code == sf::Keyboard::Key::F9){F9=true;}
			}
			if (const auto* mouse=event->getIf<sf::Event::MouseButtonPressed>()){
				if (mouse->button == sf::Mouse::Button::Left){Mouse1=true;}
				if (mouse->button == sf::Mouse::Button::Right){Mouse2=true;}
			}
		}
	}









	void PERFORMACE_COUNTER::SETUP(){
		FPS_CLOCK.restart();
		UPS_CLOCK.restart();
		FPS_UPS_RENDER_TEXT.setCharacterSize(50);
		FPS_UPS_RENDER_TEXT.setPosition({1690.f,0.f});
		FPS_UPS_RENDER_TEXT.setFillColor(sf::Color::Green);
	} 

	void PERFORMACE_COUNTER::FPS_UPDATE(){
		FRAMES_COUNTER++;
		if (FPS_CLOCK.getElapsedTime()>FPS_UPDATE_TIME){
			FPS_STRING=std::to_string(int(FRAMES_COUNTER*(sf::milliseconds(1000)/FPS_UPDATE_TIME)));
			FRAMES_COUNTER=0;
			FPS_CLOCK.restart();
		}
	}

	void PERFORMACE_COUNTER::UPS_UPDATE(){
		UPDATES_COUNTER++;
		if (UPS_CLOCK.getElapsedTime()>UPS_UPDATE_TIME){
			UPS_STRING=std::to_string(int(UPDATES_COUNTER*(sf::milliseconds(1000)/UPS_UPDATE_TIME)));
			UPDATES_COUNTER=0;
			UPS_CLOCK.restart();
		}
	}

	void PERFORMACE_COUNTER::DRAW(sf::RenderWindow& window){
		window.setView(GLOBAL_VARIABLES.default_view);
		FPS_UPS_RENDER_TEXT.setString("FPS/UPS\n"+FPS_STRING+"/"+UPS_STRING);
		window.draw(FPS_UPS_RENDER_TEXT);
	}








	void GAME::RUN(){
		SETUP();
		float time_accumulator=0;
		float delta_time=1.f/60.f;
		sf::Clock delta_clock;
		float max_time_accumulator=delta_time*GLOBAL_VARIABLES.max_ticks_per_frame;
		while (window.isOpen()){
			float elapsed=delta_clock.restart().asSeconds()*GLOBAL_VARIABLES.tick_speed;
			time_accumulator+=elapsed;
			if (time_accumulator>max_time_accumulator){time_accumulator=max_time_accumulator;}
			UPDATE_INPUT();
			for (;time_accumulator>=delta_time;time_accumulator-=delta_time){
				UPDATE_PHYSICS();
				if (delta_clock.getElapsedTime().asSeconds()>delta_time*0.7){time_accumulator=0;break;}
			}	
			performance_clocks.FPS_UPDATE();
			DRAW();
		}
	}

	void GAME::UPDATE_INPUT(){
		input.read(window);
	}

	void GAME::UPDATE_PHYSICS(){
		performance_clocks.UPS_UPDATE();
		APPLY_GRAVITY();
		for (auto& cur_ball:balls){
			cur_ball.coords.x+=cur_ball.speed.x;	
			BALL_TO_WALL_COLLISION_X(cur_ball);
			cur_ball.coords.y+=cur_ball.speed.y;	
			BALL_TO_WALL_COLLISION_Y(cur_ball);
			CHECK_OUT_OF_BOUNDS(cur_ball);
		}	
		CHUNK_FILLING();
		BALL_TO_BALL_LOOP();

	}

	void GAME::APPLY_GRAVITY(){
		for (auto& cur_ball:balls){
			cur_ball.speed.y+=GLOBAL_VARIABLES.gravity_power;
		}
	}

	void GAME::CHUNK_FILLING(){
		for (auto& cur_chunk:chunks){
			cur_chunk.indexes.clear();
		}
		for (int i=0;i<balls.size();i++){
			BALL& cur_ball=balls[i];
			int cur_x=int(cur_ball.coords.x)/GLOBAL_VARIABLES.chunk_size;
			int cur_y=int(cur_ball.coords.y)/GLOBAL_VARIABLES.chunk_size;
			chunks[cur_x+cur_y*GLOBAL_VARIABLES.chunks_x].indexes.push_back(i);
		}
	}

	void GAME::BALL_TO_BALL_LOOP(){
		int ch_x=GLOBAL_VARIABLES.chunks_x;
		int ch_y=GLOBAL_VARIABLES.chunks_y;
		for (int chunk1_x=0;chunk1_x<ch_x;chunk1_x++){
			for (int chunk1_y=0;chunk1_y<ch_y;chunk1_y++){
						CHUNK& chunk1=chunks[chunk1_x+chunk1_y*ch_x];
						CHECK_CHUNK_TO_CHUNK_COLLISION(chunk1,chunk1);

						if (chunk1_x<ch_x-1){
							CHUNK& chunk2=chunks[chunk1_x+1+chunk1_y*ch_x];
							CHECK_CHUNK_TO_CHUNK_COLLISION(chunk1,chunk2);
						}
						if (chunk1_y<ch_y-1){
							CHUNK& chunk2=chunks[chunk1_x+(chunk1_y+1)*ch_x];
							CHECK_CHUNK_TO_CHUNK_COLLISION(chunk1,chunk2);
						}
						if (chunk1_y<ch_y-1 && chunk1_x<ch_x-1){
							CHUNK& chunk2=chunks[chunk1_x+1+(chunk1_y+1)*ch_x];
							CHECK_CHUNK_TO_CHUNK_COLLISION(chunk1,chunk2);
						}	
						if (chunk1_y<ch_y-1 && chunk1_x>0){
							CHUNK& chunk2=chunks[chunk1_x-1+(chunk1_y+1)*ch_x];
							CHECK_CHUNK_TO_CHUNK_COLLISION(chunk1,chunk2);
						}	
			}
		}
	}

	void GAME::CHECK_OUT_OF_BOUNDS(BALL& cur_ball){
			if (cur_ball.coords.x<0){cur_ball.coords.x=0;}
			if (cur_ball.coords.x>GLOBAL_VARIABLES.chunk_size*GLOBAL_VARIABLES.chunks_x){cur_ball.coords.x=GLOBAL_VARIABLES.chunk_size*GLOBAL_VARIABLES.chunks_x-1;}
			if (cur_ball.coords.y<0){cur_ball.coords.y=0;}
			if (cur_ball.coords.y>GLOBAL_VARIABLES.chunk_size*GLOBAL_VARIABLES.chunks_y){cur_ball.coords.y=GLOBAL_VARIABLES.chunk_size*GLOBAL_VARIABLES.chunks_y-1;}
	}

	void GAME::BALL_TO_WALL_COLLISION_X(BALL& cur_ball){
			for (auto&cur_wall:walls){
				WALL_AGAINSS_BALL_X(cur_ball,cur_wall);
			}
	}

	void GAME::BALL_TO_WALL_COLLISION_Y(BALL& cur_ball){
			for (auto&cur_wall:walls){
				WALL_AGAINSS_BALL_Y(cur_ball,cur_wall);
			}
	}

	void GAME::WALL_AGAINSS_BALL_X(BALL& cur_ball,WALL& cur_wall){
			float wallefc=GLOBAL_VARIABLES.wall_hit_efficiency;
  			sf::FloatRect ball_rect{cur_ball.coords,sf::Vector2f{GLOBAL_VARIABLES.radius*2,GLOBAL_VARIABLES.radius*2}};
  		 	auto the_intersection=ball_rect.findIntersection(cur_wall.rect);
 	 	if (the_intersection.has_value()){
			sf::FloatRect cur_intersection=the_intersection.value();
			
    		float ball_center=cur_ball.coords.x+GLOBAL_VARIABLES.radius;
			float wall_center=cur_wall.rect.position.x+cur_wall.rect.size.x/2.f;
			if (ball_center>wall_center){
				cur_ball.speed.x=abs(cur_ball.speed.x)*wallefc;
				cur_ball.coords.x=cur_wall.rect.position.x+cur_wall.rect.size.x;
			} else {
				cur_ball.speed.x=-abs(cur_ball.speed.x)*wallefc;
				cur_ball.coords.x=cur_wall.rect.position.x-GLOBAL_VARIABLES.radius*2;
			}
  	  	}
	}

	void GAME::WALL_AGAINSS_BALL_Y(BALL& cur_ball,WALL& cur_wall){
			float wallefc=GLOBAL_VARIABLES.wall_hit_efficiency;
  			sf::FloatRect ball_rect{cur_ball.coords,sf::Vector2f{GLOBAL_VARIABLES.radius*2,GLOBAL_VARIABLES.radius*2}};
  		 	auto the_intersection=ball_rect.findIntersection(cur_wall.rect);
 	 	if (the_intersection.has_value()){
			sf::FloatRect cur_intersection=the_intersection.value();
			
    		float ball_center=cur_ball.coords.y+GLOBAL_VARIABLES.radius;
			float wall_center=cur_wall.rect.position.y+cur_wall.rect.size.y/2.f;
			if (ball_center>wall_center){
				cur_ball.speed.y=abs(cur_ball.speed.y)*wallefc;
				cur_ball.coords.y=cur_wall.rect.position.y+cur_wall.rect.size.y;
			} else {
				cur_ball.speed.y=-abs(cur_ball.speed.y)*wallefc;
				cur_ball.coords.y=cur_wall.rect.position.y-GLOBAL_VARIABLES.radius*2;
			}
  	  	}
	}

	void GAME::DRAW(){
		window.clear();
		DRAW_WALLS();
		DRAW_BALLS();
		DRAW_CURSOR();
		performance_clocks.DRAW(window);
		window.display();
	}


	void GAME::DRAW_CURSOR(){
		sf::CircleShape cursor(50.f);
		cursor.setFillColor(sf::Color::Cyan);
		cursor.setPosition(input.mouse_true_coords);
		cursor.setOrigin({50.f,50.f});
		window.draw(cursor);
	}

	void GAME::DRAW_WALLS(){
		sf::Texture wall_texture;
		sf::VertexArray vertexes(sf::PrimitiveType::Triangles);
		sf::Color color=sf::Color(40,40,40,255);
		sf::CircleShape shape(GLOBAL_VARIABLES.radius);

		float texture_len=wall_texture.getSize().x;
		float texture_wid=wall_texture.getSize().y;

		for (auto& cur_wall:walls){
			float left=cur_wall.rect.position.x;
			float top=cur_wall.rect.position.y;
			float right=left+cur_wall.rect.size.x;
			float bottom=top+cur_wall.rect.size.y;

			vertexes.append(sf::Vertex({left,top},color,{0.f,0.f}));
			vertexes.append(sf::Vertex({right,top},color,{texture_len,0.f}));
			vertexes.append(sf::Vertex({left,bottom},color,{0.f,texture_wid}));

			vertexes.append(sf::Vertex({left,bottom},color,{0.f,texture_wid}));
			vertexes.append(sf::Vertex({right,top},color,{texture_len,0.f}));
			vertexes.append(sf::Vertex({right,bottom},color,{texture_len,texture_wid}));
		}
		window.draw(vertexes,&wall_texture);
	}

	void GAME::DRAW_BALLS(){
		sf::VertexArray vertexes(sf::PrimitiveType::Triangles);
		sf::Color color=sf::Color::Blue;
		sf::CircleShape shape(GLOBAL_VARIABLES.radius);

		float texture_len=GLOBAL_ASSETS.ball_texture.getSize().x;
			float texture_wid=GLOBAL_ASSETS.ball_texture.getSize().y;
		for (auto& cur_ball:balls){
			float left=cur_ball.coords.x;
			float top=cur_ball.coords.y;
			float right=left+GLOBAL_VARIABLES.radius*2;
			float bottom=top+GLOBAL_VARIABLES.radius*2;

			vertexes.append(sf::Vertex({left,top},color,{0.f,0.f}));
			vertexes.append(sf::Vertex({right,top},color,{texture_len,0.f}));
			vertexes.append(sf::Vertex({left,bottom},color,{0.f,texture_wid}));

			vertexes.append(sf::Vertex({left,bottom},color,{0.f,texture_wid}));
			vertexes.append(sf::Vertex({right,top},color,{texture_len,0.f}));
			vertexes.append(sf::Vertex({right,bottom},color,{texture_len,texture_wid}));
		}
		window.draw(vertexes,&GLOBAL_ASSETS.ball_texture);
	}

	void GAME::LOAD_WALLS(){
		WALL cur_wall;
		cur_wall.rect=sf::FloatRect({0.f,0.f},{1920.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({1870.f,0.f},{50.f,1080.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({0.f,1030.f},{1920.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({0.f,0.f},{50.f,1080.f});
		walls.push_back(cur_wall);

		
		cur_wall.rect=sf::FloatRect({750.f,400.f},{50.f,400.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({800.f,750.f},{200.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({1000.f,700.f},{50.f,100.f});
		walls.push_back(cur_wall);

		cur_wall.rect=sf::FloatRect({750.f,400.f},{450.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({1150.f,400.f},{50.f,250.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({900.f,600.f},{300.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({850.f,500.f},{50.f,150.f});
		walls.push_back(cur_wall);

	}

	void GAME::GENERATE_RANDOM_BALL(){
		BALL cur_ball;
		cur_ball.coords.x=rand_x_cord(random_engine);
		cur_ball.coords.y=rand_y_cord(random_engine);

		cur_ball.speed.x=rand_x_vel(random_engine);
		cur_ball.speed.y=rand_y_vel(random_engine);
		
		balls.push_back(cur_ball);
	}






int main(){
	GAME game;
	game.RUN();
}
