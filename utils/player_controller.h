#include "camera.h"
#include "collision.h"

struct PlayerController : Camera
{
    //Collision collision;
    float     gravity;

    PlayerController(const vec3 position = vec3(0.0f, 0.0f, 0.0f), const vec3 up = vec3(0.0f, 1.0f, 0.0f), const float yaw = YAW, float pitch = PITCH)
        :Camera(position, up, yaw, pitch)
    {
        this->gravity   = 9.8f;
        //this->collision = new Collision(position);
    }

    ~PlayerController()
    {
        //delete this->collision;
    }

    void update(const float delta_time) override
    {
        this->position += this->trajectory * this->movement_speed * delta_time;
    }

    void process_keyboard(const SDL_Event &e) override
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            const SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_w)
                this->input_vector.z = 1;
            if (key == SDLK_a)
                this->input_vector.x = 1;
            if (key == SDLK_s)
                this->input_vector.z = -1;
            if (key == SDLK_d)
                this->input_vector.x = -1;
            if (key == SDLK_LSHIFT)
                this->movement_speed = SPEED * 4;
        }
        if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            const SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_w)
                this->input_vector.z = 0;
            if (key == SDLK_a)
                this->input_vector.x = 0;
            if (key == SDLK_s)
                this->input_vector.z = 0;
            if (key == SDLK_d)
                this->input_vector.x = 0;
            if (key == SDLK_LSHIFT)
                this->movement_speed = SPEED;
        }
        if (this->input_vector.x != 0 || this->input_vector.y != 0 || this->input_vector.z != 0) {
            this->input_vector = normalize(this->input_vector);
        }
    }

private:
    void update_camera_vectors() override
    {
        vec3 f;
        f.x = cos(radians(this->yaw)) * cos(radians(this->pitch));
        f.y = 0.f;
        f.z = sin(radians(yaw)) * cos(radians(this->pitch));
        this->front = normalize(f);
        this->right = normalize(cross(this->front, this->world_up));
        this->up    = normalize(cross(this->right, this->front));

        this->trajectory = this->input_vector.x * -this->right + this->input_vector.y * this->up + this->input_vector.z * this->front;

        f.x = cos(radians(this->yaw)) * cos(radians(this->pitch));
        f.y = sin(radians(this->pitch));
        f.z = sin(radians(yaw)) * cos(radians(this->pitch));
        this->front = normalize(f);
        this->right = normalize(cross(this->front, this->world_up));
        this->up    = normalize(cross(this->right, this->front));
    }
};


