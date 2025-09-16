#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint concert_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > concert_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("concert.pnct"));
	concert_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > concert_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("concert.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = concert_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = concert_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > Cdrone_sample(LoadTagEarly, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Cdrone.wav"));
});

Load< Sound::Sample > Sphere0_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Sphere0.wav"));
});
Load< Sound::Sample > Sphere1_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Sphere1.wav"));
});
Load< Sound::Sample > Sphere2_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Sphere2.wav"));
});
Load< Sound::Sample > Square0_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Square0.wav"));
});
Load< Sound::Sample > Square1_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Square1.wav"));
});
Load< Sound::Sample > Tetra0_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Tetra0.wav"));
});
Load< Sound::Sample > Tetra1_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Tetra1.wav"));
	});
Load< Sound::Sample > Tetra2_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Tetra2.wav"));
	});
Load< Sound::Sample > Tetra3_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Tetra3.wav"));
	});

Load< Sound::Sample > Drum_sample(LoadTagEarly, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("Drums.wav"));
	});



PlayMode::PlayMode() : scene(*concert_scene) {
	//get pointers to leg for convenience:
	shapes.reserve(3);
	for (auto &transform : scene.transforms) {
		if (transform.name == "Tetrahedron") shapes[0]  = &transform;
		else if (transform.name == "Cube") shapes[1] = &transform;
		else if (transform.name == "Sphere") shapes[2] = &transform;
	}
	if (shapes[0] == nullptr) std::cout << "wdawd" << std::endl;


	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	camera->transform->rotation = glm::quat(0.521f,0.478f,0.478f,0.521f);
	baseRot = glm::quat(0.521f, 0.478f, 0.478f, 0.521f);

	

	//start music loop playing:
	// (note: position will be over-ridden in update())
	//leg_tip_loop = Sound::loop_3D(*dusty_floor_sample, 1.0f, get_leg_tip_position(), 10.0f);

	Cdrone = Sound::play(*Cdrone_sample);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_EVENT_KEY_DOWN && startWait > 6.0f) {
		if (evt.key.key == SDLK_ESCAPE) {
			SDL_SetWindowRelativeMouseMode(Mode::window, false);
			return true;
		} else if (evt.key.key == SDLK_A) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_D) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_W) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_S) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_SPACE) {
			spacePressed = true;
			if (Cdrone) Cdrone->stop();
			Cdrone = Sound::play(*Cdrone_sample);
		}
	} else if (evt.type == SDL_EVENT_KEY_UP) {
		if (evt.key.key == SDLK_A) {
			left.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_D) {
			right.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_W) {
			up.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_S) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		//if (SDL_GetWindowRelativeMouseMode(Mode::window) == false) {
		//	SDL_SetWindowRelativeMouseMode(Mode::window, true);
		//	return true;
		//}
	} else if (evt.type == SDL_EVENT_MOUSE_MOTION) {
	/*	if (SDL_GetWindowRelativeMouseMode(Mode::window) == true) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;*/
	}


	return false;
}




void PlayMode::update(float elapsed) {
	startWait += elapsed;

	if (!musicStarted && (left.pressed ^ right.pressed) && startWait>5.0f) {
		musicStarted = true;

		Sound::stop_all_samples();


		Spheres.emplace_back(Sound::loop_3D(*Sphere0_sample, 0.0f, shapes[2]->position, 10.0f));
		Spheres.emplace_back(Sound::loop_3D(*Sphere1_sample, 0.0f, shapes[2]->position, 10.0f));
		Spheres.emplace_back(Sound::loop_3D(*Sphere2_sample, 0.0f, shapes[2]->position, 10.0f));



		Tetrahedrons.emplace_back(Sound::loop_3D(*Tetra0_sample, 0.0f, shapes[0]->position, 10.0f));
		Tetrahedrons.emplace_back(Sound::loop_3D(*Tetra1_sample, 0.0f, shapes[0]->position, 10.0f));
		Tetrahedrons.emplace_back(Sound::loop_3D(*Tetra2_sample, 0.0f, shapes[0]->position, 10.0f));
		Tetrahedrons.emplace_back(Sound::loop_3D(*Tetra3_sample, 0.0f, shapes[0]->position, 10.0f));



		Squares.emplace_back(Sound::loop_3D(*Square0_sample, 0.0f, shapes[1]->position, 10.0f));
		Squares.emplace_back(Sound::loop_3D(*Square1_sample, 0.0f, shapes[1]->position, 10.0f));
		

		Drums = Sound::loop(*Drum_sample,0.0f,0.0f);
	}

	//turn camera:
	{
		if(!turning){		
			if (left.pressed && !right.pressed&& startWait > 5.0f) {
				turning = true;
				curShape = mod((curShape - 1) ,3);
			}
			if (!left.pressed && right.pressed&& startWait > 5.0f) {
				turning = true;
				curShape = (curShape + 1) % 3;
			}

			destAngle = (curShape - 1) * -unitDeltaAngle;
			deltaAngle = destAngle - curAngle;
			startAngle = curAngle;
			turningTime = 0.0f;
		}
		if (turning) {
			turningTime += elapsed;
			
			curAngle = startAngle + (deltaAngle * turningTime) / turningDuration;
			//std::cout <<curShape<<" " << destAngle << " " << curShape << std::endl;
			camera->transform->rotation = glm::normalize(
				baseRot
				* glm::angleAxis(glm::radians(curAngle), glm::vec3(0.0f, 1.0f, 0.0f)));
			if (deltaAngle > 0 && curAngle > destAngle) {
				turningTime = 0;
				turning = false;
				curAngle = destAngle;
				
				if(!win )tryCorrect();
				changing = true;
				changeTime = 0.0f;
			}
			if (deltaAngle < 0 && curAngle < destAngle) {
				turningTime = 0;
				turning = false;
				curAngle = destAngle;

				if(!win)tryCorrect();
				changing = true;
				changeTime = 0.0f;
			}		
		}
	}

	//enact change of scale played by the current musician
	{
		if (changing) {
			changeTime += elapsed;
			shapes[curShape]->position.z = std::sin(changeTime*30.0f);
			if (std::sin(changeTime*30.0f) < 0) {
				shapes[curShape]->position.z = 0;
				changing = false;
				changeTime = 0;
			}
		}
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_parent_from_local();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		if (!spacePressed) {
			text = spaceTut;
		}
		else {
			text = adTut;
		}

		if (curSphere == 0 && curSquare == 0 && curTetrahedron == 0) {
			win = true;
			text = WIN;
			Drums->set_volume(1.0f);
			Sound::set_volume(0.7f);
		}
		


		constexpr float H = 0.09f;
		lines.draw_text(spaceTut,
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text(spaceTut,
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xf0, 0x3f, 0x2f, 0x00));
	}
	GL_ERRORS();
}


