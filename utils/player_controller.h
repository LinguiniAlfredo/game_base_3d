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
    PlayerState state;

    PlayerController(const vec3  position = vec3(0.0f, 0.0f, 0.0f),
                     const vec3  front = vec3(0.0f, 0.0f, -1.0f),
                     const vec3  up = vec3(0.0f, 1.0f, 0.0f),
                     const float yaw = YAW, float pitch = PITCH)
        :Camera(position, front, up, yaw, pitch)
    {
        this->gravity         = vec3(0.f, -9.8f, 0.f);
        this->collision       = new Collision(position, 2.f, 10.f, 2.f);
        this->state           = AIRBORNE;
    }

    PlayerController(const Camera &other) : Camera(other)
    {
        this->gravity         = vec3(0.f, -9.8f, 0.f);
        this->collision       = new Collision(position, 2.f, 10.f, 2.f);
        this->state           = AIRBORNE;
    }

    ~PlayerController()
    {
        delete this->collision;
    }

    void update(const float delta_time) override
    {
        vec3 velocity;
        vector<vec3> collision_normals;

        bool colliding = false;
        update_camera_vectors();

        velocity = this->gravity * delta_time;

        if (AIRBORNE)
            this->collision->position += velocity;
        colliding = check_for_ground(&collision_normals);

        if (colliding) {
            velocity = resolve_collisions(velocity, collision_normals);
            this->collision->position = this->position;
            this->collision->position += velocity;
        }
        this->position = this->collision->position;

        collision_normals.clear();
        colliding = false;
        velocity = this->trajectory * this->movement_speed * delta_time;
        this->collision->position += velocity;

        for (auto &world_block : context.world_blocks) {
            if (this->collision->intersects(world_block->collision)) {
                colliding = true;
                collision_normals.push_back(world_block->collision->normal);
            }
        }
        for (auto &entity : context.entities) {
            if (this->collision->intersects(entity->collision)) {
                colliding = true;
                collision_normals.push_back(entity->collision->normal);
            }
        }
        if (colliding) {
            velocity = resolve_collisions(velocity, collision_normals);
            this->collision->position = this->position;
        vector<vec3> collision_normals;
            this->collision->position += velocity;
        }
        this->position = this->collision->position;

    }

    void resolve_collisions_floor()
    {
        this->collision->position = this->position;
    }

    vec3 resolve_collisions(const vec3 &velocity, const vector<vec3> &collision_normals)
    {
        // instead of cancelling out velocity when colliding, project trajectory along plane we are colliding with
        // s = dot(vp, n)
        // vn = n * s
        // vw = vp - vn
        vec3 v = velocity;
        for (auto &normal : collision_normals) {
            float normal_component = dot(v, normal);
            float angle_rad        = acos(dot(normalize(v), normal));

            
            // check if angle between velocity and normal greater than 90 to not round corners
            if (degrees(angle_rad) > 90) {
                vec3  normal_velocity = normal * normal_component;
                v -= normal_velocity;
                printf("angle: %f\n", degrees(angle_rad));
            }

        }

        return v;
    }

    bool check_for_ground(vector<vec3> *collision_normals)
    {
        // TODO - remove these checked blocks from later collision check
        for (auto &world_block : context.world_blocks) {
            if (world_block->position.y < this->position.y && this->collision->intersects(world_block->collision, true)) {
                this->state = GROUNDED;
                collision_normals->push_back(world_block->collision->normal);
            }
        }
        if (collision_normals->empty())
            this->state = AIRBORNE;

        return !collision_normals->empty();
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


