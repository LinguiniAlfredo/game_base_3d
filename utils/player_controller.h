#include "camera.h"
#include "collision.h"

enum PlayerState {
    GROUNDED,
    AIRBORNE
};

struct PlayerController : Camera
{
    Collision   *collision;
    vec3        gravity;
    vec3        target_position;
    PlayerState state;

    PlayerController(const vec3 position = vec3(0.0f, 0.0f, 0.0f),
                     const vec3 front = vec3(0.0f, 0.0f, 0.0f),
                     const vec3 up = vec3(0.0f, 1.0f, 0.0f),
                     const float yaw = YAW, float pitch = PITCH)
        :Camera(position, front, up, yaw, pitch)
    {
        this->gravity = vec3(0.f, -9.8f, 0.f);
        this->collision = new Collision(position, 2.f, 10.f, 2.f);
        this->state     = AIRBORNE;
    }

    PlayerController(const Camera &other) : Camera(other)
    {
        this->gravity = vec3(0.f, -9.8f, 0.f);
        this->collision = new Collision(position, 2.f, 10.f, 2.f);
        this->state     = AIRBORNE;
    }

    ~PlayerController()
    {
        delete this->collision;
    }

    void update(const float delta_time) override
    {
        this->target_position += this->trajectory * this->movement_speed * delta_time;
        if (this->state == AIRBORNE)
            this->target_position += this->gravity * delta_time;

        this->collision->position = this->target_position;

        bool colliding = check_for_ground();

        for (auto &world_block : context.world_blocks) {
            if (world_block->position.y >= this->position.y && this->collision->intersects(world_block->collision)) {
                colliding = true;
            }
        }
        for (auto &entity : context.entities) {
            if (this->collision->intersects(entity->collision)) {
                colliding = true;
                break;
            }
        }

        if (colliding) {
            resolve_collisions();
        } else {
            this->position = this->target_position;
        }
    }

    void resolve_collisions()
    {
        this->collision->position = this->position;
        this->target_position = this->position;
        // instead of cancelling out velocity when colliding, project velocity along plane we are colliding with
        // s = dot(vp, n)
        // vn = n * s
        // vw = vp - vn
    }

    bool check_for_ground()
    {
        // TODO - remove these checked blocks from later collision check
        for (auto &world_block : context.world_blocks) {
            if (world_block->position.y < this->position.y && this->collision->intersects(world_block->collision)) {
                this->state = GROUNDED;
                return true;
            }
        }
        this->state = AIRBORNE;
        return false;
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
                this->movement_speed = SPEED * 2;
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
        // we update vectors once without pitch so movement trajectory is bound to the plane
        // then update vectors again with pitch for looking around
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


