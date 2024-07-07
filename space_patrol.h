// space_patrol.h

#ifndef SPACE_PATROL_H
#define SPACE_PATROL_H

#define MAX_WEAPONS 5
#define MAX_SECTORS 9
#define MAX_PIRATE_WEAPONS 3

typedef struct {
    char name[20];
    int damage;
    int cost;
} Weapon;

typedef struct {
    char name[20];
    int health;
    int max_health;
    int fuel;
    int max_fuel;
    int money;
    Weapon weapons[MAX_WEAPONS];
    int current_sector;
    int eta; // Estimated Turns of Arrival
} Player;

typedef struct {
    int number;
    int has_pirates;
    int is_starbase;
} Sector;

typedef struct {
    int health;
    int max_health;
    Weapon weapons[MAX_PIRATE_WEAPONS];
    int strategy; // 0: Attack, 1: Defense
} Pirate;

typedef struct {
    Sector sectors[MAX_SECTORS];
    Player player;
    Pirate pirate;
    char message[256];
} Game;

// Function prototypes
void initialize_game(Game *game);
void display_banner();
void display_grid(Game *game);
void display_player_status(Game *game);
void move_player(Game *game);
void scan_for_pirates(Game *game);
void engage_battle(Game *game);
void visit_starbase(Game *game);
void purchase_weapons(Game *game);
void refuel_ship(Game *game);
void generate_message(Game *game);
void game_loop(Game *game);

#endif // SPACE_PATROL_H
