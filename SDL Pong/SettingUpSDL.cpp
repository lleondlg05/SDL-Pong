#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <chrono>

const int SCREEN_WIDTH = 1020;
const int SCREEN_HEIGHT = 480;
const int BALL_WIDTH = 15;
const int BALL_HEIGHT = 15;
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 100;

enum Buttons
{
	PaddleOneUp = 0,
	PaddleOneDown,
	PaddleTwoUp,
	PaddleTwoDown,
};

const float PADDLE_SPEED = 1.0f;

class Vec2
{
public:
	Vec2()
		: x(0.0f), y(0.0f)
	{}

	Vec2(float x, float y)
		: x(x), y(y)
	{}

	Vec2 operator+(Vec2 const& rhs)
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2& operator += (Vec2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	Vec2 operator*(float rhs)
	{
		return Vec2(x * rhs, y * rhs);
	}

	float x, y;
};

class Ball
{
	public:
		Ball(Vec2 position)
			: position(position)
		{
			rect.x = static_cast<int>(position.x);
			rect.y = static_cast<int>(position.y);
			rect.w = BALL_WIDTH;
			rect.h = BALL_HEIGHT;
		}

		void Draw(SDL_Renderer* renderer)
		{
			rect.x = static_cast<int>(position.x);
			rect.y = static_cast<int>(position.y);

			SDL_RenderFillRect(renderer, &rect);
		}

		Vec2 position;
		SDL_Rect rect;
};

class Paddle
{
	public:
		Paddle(Vec2 position, Vec2 Velocity)
			: position(position), velocity(velocity)
		{
			rect.x = static_cast<int>(position.x);
			rect.y = static_cast<int>(position.y);
			rect.w = PADDLE_WIDTH;
			rect.h = PADDLE_HEIGHT;
		}

		void Update(float dt)
		{
			position += velocity * dt;

			if (position.y <= 0)
			{
				position.y = 0;
			}
			else if(position.y > (SCREEN_HEIGHT - PADDLE_HEIGHT))
			{
				position.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
			}
		}

		void Draw(SDL_Renderer* renderer)
		{
			rect.y = static_cast<int>(position.y);

			SDL_RenderFillRect(renderer, &rect);
		}

		Vec2 velocity;
		Vec2 position;
		SDL_Rect rect;
};

class PlayerScore
{
	public:
		PlayerScore(Vec2 position, SDL_Renderer* renderer, TTF_Font* font)
			: renderer(renderer), font(font)
		{
			surface = TTF_RenderText_Solid(font, "0", { 0xFF, 0xFF, 0xFF, 0xFF });
			texture = SDL_CreateTextureFromSurface(renderer, surface);

			int width, height;
			SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

			rect.x = static_cast<int>(position.x);
			rect.y = static_cast<int>(position.y);
			rect.w = width;
			rect.h = height;
		}

		~PlayerScore()
		{
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}

		void Draw()
		{
			SDL_RenderCopy(renderer, texture, nullptr, &rect);
		}

		SDL_Renderer* renderer;
		TTF_Font* font;
		SDL_Surface* surface{};
		SDL_Texture* texture{};
		SDL_Rect rect{};
};

int main(int args, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow("SDL Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	
	//Initialize font
	TTF_Font* scoreFont = TTF_OpenFont("DejaVuSansMono.ttf", 40);
	
	Ball ball(Vec2 ( (SCREEN_WIDTH / 2) - (BALL_WIDTH / 2) , (SCREEN_HEIGHT / 2) - (BALL_HEIGHT / 2) ) );

	Paddle paddleOne(Vec2 (50, (SCREEN_HEIGHT / 2) - (PADDLE_HEIGHT / 2) ), Vec2(0.0f, 0.0f) );
	Paddle paddleTwo(Vec2 (SCREEN_WIDTH - 50, (SCREEN_HEIGHT / 2) - (PADDLE_HEIGHT / 2) ), Vec2(0.0f, 0.0f) );

	PlayerScore playerOneScoreText(Vec2(SCREEN_WIDTH / 4, 20), renderer, scoreFont);

	PlayerScore playerTwoScoreText(Vec2(3 * SCREEN_WIDTH / 4, 20), renderer, scoreFont);

	{
		bool running = true;
		bool buttons[4] = {};

		float dt = 0.0f;

		while (running)
		{
			auto startTime = std::chrono::high_resolution_clock::now();

			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					running = false;
				}
				else if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						running = false;
					}

					else if (event.key.keysym.sym == SDLK_w)
					{
						buttons[Buttons::PaddleOneUp] = true;
					}

					else if (event.key.keysym.sym == SDLK_s)
					{
						buttons[Buttons::PaddleOneDown] = true;
					}

					else if (event.key.keysym.sym == SDLK_UP)
					{
						buttons[Buttons::PaddleTwoUp] = true;
					}

					else if (event.key.keysym.sym == SDLK_DOWN)
					{
						buttons[Buttons::PaddleTwoDown] = true;
					}
				}
				else if (event.type == SDL_KEYUP)
				{
					if (event.key.keysym.sym == SDLK_w)
					{
						buttons[Buttons::PaddleOneUp] = false;
					}

					else if (event.key.keysym.sym == SDLK_s)
					{
						buttons[Buttons::PaddleOneDown] = false;
					}

					else if (event.key.keysym.sym == SDLK_UP)
					{
						buttons[Buttons::PaddleTwoUp] = false;
					}

					else if (event.key.keysym.sym == SDLK_DOWN)
					{
						buttons[Buttons::PaddleTwoDown] = false;
					}
				}

				if (buttons[Buttons::PaddleOneUp])
				{
					paddleOne.velocity.y = -PADDLE_SPEED;
				}
				else if (buttons[Buttons::PaddleOneDown])
				{
					paddleOne.velocity.y = PADDLE_SPEED;
				}
				else
				{
					paddleOne.velocity.y = 0;
				}

				if (buttons[Buttons::PaddleTwoUp])
				{
					paddleTwo.velocity.y = -PADDLE_SPEED;
				}
				else if (buttons[Buttons::PaddleTwoDown])
				{
					paddleTwo.velocity.y = PADDLE_SPEED;
				}
				else
				{
					paddleTwo.velocity.y = 0;
				}

				//Clear the window to black
				SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
				SDL_RenderClear(renderer);

				//Set the draw color to be white
				SDL_SetRenderDrawColor(renderer, 0x0FF, 0x0FF, 0x0FF, 0xFF);

				//Draw the net
				for (int y = 0; y < SCREEN_HEIGHT; ++y)
				{
					if (y % 5)
					{
						SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, y);
					}
				}

				//Update Paddle Movement
				paddleOne.Update(dt);
				paddleTwo.Update(dt);

				//Draw the ball
				ball.Draw(renderer);

				//Draw the paddle 1 & 2
				paddleOne.Draw(renderer);
				paddleTwo.Draw(renderer);

				//Draw the score texts
				playerOneScoreText.Draw();
				playerTwoScoreText.Draw();

				//Present the backbuffer
				SDL_RenderPresent(renderer);

				auto stopTime = std::chrono::high_resolution_clock::now();
				dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
			}
		}

		//Clean Up
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_CloseFont(scoreFont);
		TTF_Quit();
		SDL_Quit();

		return 0;
	}
}