#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>

std::mt19937 random_engine{std::random_device{}()};


struct GLOBALS{
	float radius=1;
	float chunk_size=10;
	float chunks_x=192;
	float chunks_y=108;
	int chunk_vector_size=chunks_x*chunks_y;
};
GLOBALS GLOBAL_VARIABLES;

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


	void SETUP(){
		CREATE_CIRCLE_TEXTURE();
		for (int i=0;i<100000;i++){
			GENERATE_RANDOM_BALL();
		}
	}	

	void RUN(){
		SETUP();
		while (window.isOpen()){
			UPDATE_INPUT();
			UPDATE_PHYSICS();
			DRAW();
		}
	}

	void UPDATE_INPUT(){
		input.read(window);
	}

	void UPDATE_PHYSICS(){
		
	}
	
	void DRAW(){
		window.clear();
		DRAW_BALLS();
		sf::CircleShape cursor(100.f);
		cursor.setFillColor(sf::Color::Green);
		cursor.setPosition(input.mouse_true_coords);
		cursor.setOrigin({100.f,100.f});
		window.draw(cursor);
		window.display();
	}

	void DRAW_BALLS();

	void CREATE_CIRCLE_TEXTURE();

	void GENERATE_RANDOM_BALL();
};




















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
