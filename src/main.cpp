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
	sf::RenderWindow window{sf::VideoMode({ 1920, 1080 }), "fluid simulation"};
	INPUT input;
	std::vector <CHUNK> chunks{size_t(GLOBAL_VARIABLES.chunk_vector_size)};
	std::vector <BALL> balls;
	sf::Texture circle_texture;

	std::uniform_real_distribution<float> rand_x_cord{0.f, 1920.f};
    std::uniform_real_distribution<float> rand_y_cord{0.f, 1080.f};

	std::uniform_real_distribution<float> rand_x_vel{-50.f, 50.f};
    std::uniform_real_distribution<float> rand_y_vel{-50.f, 50.f};

	PERFORMACE_COUNTER performance_clocks; 


	
	void RUN(){
		SETUP();
		float time_accumulator=0;
		float tick_speed=1;
		float delta_time=1.f/60.f;
		sf::Clock delta_clock;
		while (window.isOpen()){
			float elapsed=delta_clock.restart().asSeconds()*tick_speed;
			time_accumulator+=elapsed;
			for (;time_accumulator>=delta_time;time_accumulator-=delta_time){
				UPDATE_INPUT();
				UPDATE_PHYSICS();
			}	
			performance_clocks.FPS_UPDATE();
			DRAW();
		}
	}

	void SETUP(){
		performance_clocks.SETUP();
		GLOBAL_ASSETS.LOAD_ALL_ASSETS();
		//window.setVerticalSyncEnabled(true);
		CREATE_CIRCLE_TEXTURE();
		for (int i=0;i<400000;i++){
			GENERATE_RANDOM_BALL();
		}
	}	

	void UPDATE_INPUT(){
		input.read(window);
	}

	void UPDATE_PHYSICS(){
		performance_clocks.UPS_UPDATE();
	}
	
	void DRAW(){
		window.clear();
		DRAW_BALLS();
		DRAW_CURSOR();
		performance_clocks.DRAW(window);
		window.display();
	}

	void DRAW_CURSOR(){
		sf::CircleShape cursor(100.f);
		cursor.setFillColor(sf::Color::Cyan);
		cursor.setPosition(input.mouse_true_coords);
		cursor.setOrigin({100.f,100.f});
		window.draw(cursor);
	}

	void DRAW_BALLS();

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
