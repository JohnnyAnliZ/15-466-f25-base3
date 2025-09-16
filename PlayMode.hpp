#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//strings for messaging
	std::string spaceTut = "Press Space to Hear Reference Note"; 
	std::string adTut = "Press A,D to Conduct";
	std::string WIN = "You've Done It, Enjoy the Music, Maestro!";
	std::string& text = spaceTut;

	float wobble = 0.0f;
	float e = 0.00001f;

	//bools
	bool turning = false;
	float turningDuration = 0.5f;
	float turningTime = 0.0f;
	glm::quat baseRot;
	float curAngle = 0.0f;
	float destAngle = 0.0f;
	float startAngle = 0.0f;
	float deltaAngle = 0.0f;
	float unitDeltaAngle = 13.0f;

	bool changing = false;
	float changeTime = 0.0f;

	bool musicStarted = false;
	bool spacePressed = false;

	bool win = false;

	float startWait = 0;
	//music
 
	std::shared_ptr< Sound::PlayingSample > Drums;

	std::vector<std::shared_ptr< Sound::PlayingSample >>  Spheres;//curShape 2
	std::vector<std::shared_ptr< Sound::PlayingSample >>  Squares;//curShape 1
	std::vector<std::shared_ptr< Sound::PlayingSample >>  Tetrahedrons; //curShape 0 
	//

	std::vector<Scene::Transform*> shapes;

	int32_t curSphere = 0;//%3
	int32_t curSquare = 1;//%2
	int32_t curTetrahedron = 0;//%4
	int32_t curShape = 1;//%3

	int32_t mod(int32_t a, int32_t b) {
		return (a % b + b) % b;
	}

	//change track by setting volume
	void tryCorrect() {
		if (curShape == 2) {
			Spheres[curSphere]->set_volume(0.0f);
			curSphere = mod(curSphere + 1, 3);
			Spheres[curSphere]->set_volume(1.0f);
		}
		else if (curShape == 1) {
			Squares[curSquare]->set_volume(0.0f);
			curSquare = mod(curSquare + 1, 2);
			Squares[curSquare]->set_volume(1.0f);
		}
		else {
			Tetrahedrons[curTetrahedron]->set_volume(0.0f);
			curTetrahedron = mod(curTetrahedron + 1, 4);
			Tetrahedrons[curTetrahedron]->set_volume(1.0f);
		}
	}

	
	//c refrence tone 
	std::shared_ptr< Sound::PlayingSample > Cdrone;
	
	//camera:
	Scene::Camera *camera = nullptr;



	

};
