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
	float radius=1;
	float tick_speed=1;  //around 250 is max
	//float max_ticks_per_frame=1000;
	float max_ticks_per_frame=tick_speed+10;
	int amount_of_balls_at_spawn=10;
	bool FULLSCREEN_MODE=true;
	float chunk_size=10;
	float chunks_x=192;
	float chunks_y=108;
	int chunk_vector_size=chunks_x*chunks_y;
};
GLOBALS GLOBAL_VARIABLES;

struct ASSETS{
	sf::Font font;

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
	int size=0;
	int indexes[35];
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
	sf::Texture circle_texture;
	std::vector <WALL> walls;

	std::uniform_real_distribution<float> rand_x_cord{900.f, 1150.f};
    std::uniform_real_distribution<float> rand_y_cord{50.f, 1020.f};

	std::uniform_real_distribution<float> rand_x_vel{1.f, 2.f};
    std::uniform_real_distribution<float> rand_y_vel{1.f, 2.f};

	PERFORMACE_COUNTER performance_clocks; 


	
	void RUN();

	void SETUP(){
		LOAD_WALLS();
		performance_clocks.SETUP();
		GLOBAL_ASSETS.LOAD_ALL_ASSETS();
		//window.setVerticalSyncEnabled(true);
		if (GLOBAL_VARIABLES.FULLSCREEN_MODE){window.create(sf::VideoMode({ 1920, 1080 }), "fluid simulation",sf::State::Fullscreen);}
		CREATE_CIRCLE_TEXTURE();
		for (int i=0;i<GLOBAL_VARIABLES.amount_of_balls_at_spawn;i++){
			GENERATE_RANDOM_BALL();
		}
	}	

	void UPDATE_INPUT();

	void UPDATE_PHYSICS(){
		performance_clocks.UPS_UPDATE();
		for (auto& cur_ball:balls){
			cur_ball.coords+=cur_ball.speed;	
		}
		BALL_TO_WALL_COLLISION();
	}

	void BALL_TO_WALL_COLLISION();

	void WALL_AGAINSS_BALL(BALL& cur_ball,WALL& cur_wall);
	
	void DRAW();

	void DRAW_CURSOR();

	void DRAW_WALLS();

	void DRAW_BALLS();

	void LOAD_WALLS();

	void CREATE_CIRCLE_TEXTURE();

	void GENERATE_RANDOM_BALL();
};











void ASSETS::LOAD_ALL_ASSETS(){
		if (!font.openFromFile("assets/fonts/arial.ttf")){std::cout<<"font failed to load";} 
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

	void GAME::BALL_TO_WALL_COLLISION(){
		for (auto& cur_ball:balls){
			for (auto&cur_wall:walls){
				WALL_AGAINSS_BALL(cur_ball,cur_wall);
			}
		}
	}

	void GAME::WALL_AGAINSS_BALL(BALL& cur_ball,WALL& cur_wall){
		sf::FloatRect ball_rect{cur_ball.coords,sf::Vector2f{GLOBAL_VARIABLES.radius*2,GLOBAL_VARIABLES.radius*2}};
		auto the_intersection=ball_rect.findIntersection(cur_wall.rect);
		if (the_intersection.has_value()){
			sf::FloatRect cur_intersection=the_intersection.value();

			float overlapx=cur_intersection.size.x;
			float overlapy=cur_intersection.size.y;

			sf::Vector2f wall_center=cur_wall.rect.position+cur_wall.rect.size/2.f;
			sf::Vector2f ball_center=cur_ball.coords+sf::Vector2f{GLOBAL_VARIABLES.radius,GLOBAL_VARIABLES.radius};

			if (overlapx<overlapy){
				if (ball_center.x<wall_center.x){
					cur_ball.speed.x=-abs(cur_ball.speed.x);
					cur_ball.coords.x=cur_wall.rect.position.x-GLOBAL_VARIABLES.radius*2;
					
				} else{
					cur_ball.speed.x=abs(cur_ball.speed.x);
					cur_ball.coords.x=cur_wall.rect.position.x+cur_wall.rect.size.x;
				}
			} else {
				if (ball_center.y<wall_center.y){
					cur_ball.speed.y=-abs(cur_ball.speed.y);
					cur_ball.coords.y=cur_wall.rect.position.y-GLOBAL_VARIABLES.radius*2;
					
				} else{
					cur_ball.speed.y=abs(cur_ball.speed.y);
					cur_ball.coords.y=cur_wall.rect.position.y+cur_wall.rect.size.y;
				}
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

		float texture_len=circle_texture.getSize().x;
			float texture_wid=circle_texture.getSize().y;
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
		window.draw(vertexes,&circle_texture);
	}

	void GAME::LOAD_WALLS(){
		WALL cur_wall;
		cur_wall.rect=sf::FloatRect({0.f,0.f},{1920.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({1870.f,50.f},{50.f,1020.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({0.f,1030.f},{1920.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({0.f,50.f},{50.f,1030.f});
		walls.push_back(cur_wall);


		cur_wall.rect=sf::FloatRect({800.f,400.f},{400.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({1150.f,450.f},{50.f,200.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({900.f,600.f},{300.f,50.f});
		walls.push_back(cur_wall);
		cur_wall.rect=sf::FloatRect({850.f,500.f},{50.f,150.f});
		walls.push_back(cur_wall);

	}

	void GAME::CREATE_CIRCLE_TEXTURE(){
		float cur_radius=200;
		unsigned int cur_size=cur_radius*2;

		sf::RenderTexture render_circle_texture(sf::Vector2u{cur_size,cur_size});
		render_circle_texture.clear();

		sf::CircleShape circle_shape(cur_radius,64);
		circle_shape.setFillColor(sf::Color::White);
		circle_shape.setPosition({0.f,0.f});


		render_circle_texture.draw(circle_shape);
		render_circle_texture.display();

		sf::Texture cur_texture;
		if (!cur_texture.loadFromImage(render_circle_texture.getTexture().copyToImage())){}
		
	
		cur_texture.setSmooth(true);

		circle_texture=cur_texture;
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
