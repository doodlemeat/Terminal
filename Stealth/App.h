#pragma once

#include <vector>
#include <string>

class State;
namespace sf
{
	class Text;
}

class App
{
public:
	App();
	~App();

	void Init(std::string title, int width, int height);
	void Cleanup();
	void Run();

	void HandleEvents();
	void Logic();
	void Render();

	void AddState(State* state);
	void ChangeState();
	void SetState(std::string state);

private:
	bool mRunning;
	std::vector<State*> states;
	State* current;
	float fps;
};

