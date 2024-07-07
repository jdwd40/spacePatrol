#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "colors.h"
#include "space_patrol.h"

int main() {
    Game game;
    display_banner();
    initialize_game(&game);
    game_loop(&game);
    return 0;
}

// Display a banner
void display_banner() {
    printf("%s", COLOR_BLUE);
    printf("******************************************************\n");
    printf("*                                                    *\n");
    printf("*                WELCOME TO SPACE PATROL             *\n");
    printf("*                                                    *\n");
    printf("******************************************************\n");
    printf("%s", COLOR_RESET);
}

// Initialize the game state
void initialize_game(Game *game) {
    // Initialize player
    strcpy(game->player.name, "Player");
    game->player.health = 100;
    game->player.max_health = 100;
    game->player.fuel = 100;
    game->player.max_fuel = 100;
    game->player.money = 500;
    game->player.current_sector = 1;
    game->player.eta = 0;

    // Initialize sectors
    for (int i = 0; i < MAX_SECTORS; i++) {
        game->sectors[i].number = i + 1;
        game->sectors[i].has_pirates = (i != 0) ? rand() % 2 : 0;
        game->sectors[i].is_starbase = (i == 0);
    }

    // Initialize player weapons (empty at start)
    for (int i = 0; i < MAX_WEAPONS; i++) {
        strcpy(game->player.weapons[i].name, "Empty");
        game->player.weapons[i].damage = 0;
        game->player.weapons[i].cost = 0;
    }

    // Initialize a default pirate
    game->pirate.health = 50;
    game->pirate.max_health = 50;
    for (int i = 0; i < MAX_PIRATE_WEAPONS; i++) {
        strcpy(game->pirate.weapons[i].name, "Laser");
        game->pirate.weapons[i].damage = 10;
        game->pirate.weapons[i].cost = 0;
    }
    game->pirate.strategy = 0;

    // Initialize message
    strcpy(game->message, "Welcome to Space Patrol! Prepare for your mission.");
}

// Display the grid in a 3x3 format with abbreviations and always include sector numbers
void display_grid(Game *game) {
    printf("%sSpace Grid:%s\n", COLOR_CYAN, COLOR_RESET);
    for (int i = 0; i < MAX_SECTORS; i++) {
        // Always start with the sector number
        printf("[%d:", i + 1);

        // Check the sector's status and display the corresponding abbreviation
        if (i == game->player.current_sector - 1) {
            printf("%spy%s", COLOR_GREEN, COLOR_RESET);
        } else if (game->sectors[i].is_starbase) {
            printf("%ssb%s", COLOR_BLUE, COLOR_RESET);
        } else if (game->sectors[i].has_pirates) {
            printf("%spr%s", COLOR_RED, COLOR_RESET);
        } else {
            printf(" "); // Just a space for empty sectors
        }
        
        printf("]"); // Close the sector display

        // Placeholder for additional information, if needed
        // printf(" Info: TBD "); // Uncomment or modify this line to add additional info
        
        // Format the grid as 3x3 by adding a newline after every third sector
        if ((i + 1) % 3 == 0) {
            printf("\n");
        } else {
            printf(" "); // Add space between sectors in the same row
        }
    }
}

// Display the player's status in a fancy box
void display_player_status(Game *game) {
    Player *player = &game->player;
    printf("%s", COLOR_BLUE);
    printf("******************************************************\n");
    printf("*                     PLAYER STATUS                  *\n");
    printf("******************************************************\n");
    printf("* Health: %-5d    Max Health: %-5d                  *\n", player->health, player->max_health);
    printf("* Fuel:   %-5d    Max Fuel:   %-5d                  *\n", player->fuel, player->max_fuel);
    printf("* Money:  %-5d                                        *\n", player->money);
    printf("* Current Sector: %-2d                                *\n", player->current_sector);
    printf("******************************************************\n");
    printf("%s", COLOR_RESET);
}

// Move the player to a new sector
void move_player(Game *game) {
    int sector;
    printf("Enter the sector number to move to (1-9): ");
    scanf("%d", &sector);
    if (sector < 1 || sector > 9) {
        printf("Invalid sector!\n");
        return;
    }
    if (sector == game->player.current_sector) {
        printf("You are already in this sector!\n");
        return;
    }

    int turns = abs(sector - game->player.current_sector) * 2; // Each sector takes 2 turns to travel through
    if (game->player.fuel < turns) {
        printf("Not enough fuel to move!\n");
        return;
    }

    game->player.fuel -= turns;
    game->player.eta = turns; // Set ETA
    game->player.current_sector = sector;
    printf("Moved to sector %d. It will take %d turns to arrive.\n", sector, turns);
}

// Scan for pirates in the current sector
void scan_for_pirates(Game *game) {
    Sector *current_sector = &game->sectors[game->player.current_sector - 1];
    if (current_sector->has_pirates) {
        printf("Pirates detected in sector %d!\n", current_sector->number);
        engage_battle(game);
    } else {
        printf("No pirates in this sector.\n");
    }
}

// Engage in a battle with pirates
void engage_battle(Game *game) {
    char *strategies[] = {"Attack", "Defense"};
    while (game->player.health > 0 && game->pirate.health > 0) {
        int player_choice, pirate_choice;
        printf("Choose your strategy (%s0: Attack%s, %s1: Defense%s): ", COLOR_YELLOW, COLOR_RESET, COLOR_YELLOW, COLOR_RESET);
        scanf("%d", &player_choice);
        pirate_choice = rand() % 2;

        printf("You chose %s%s%s, Pirate chose %s%s%s.\n", COLOR_GREEN, strategies[player_choice], COLOR_RESET, COLOR_RED, strategies[pirate_choice], COLOR_RESET);

        // Rock-paper-scissors logic
        if (player_choice == pirate_choice) {
            printf("%sIt's a draw!%s\n", COLOR_MAGENTA, COLOR_RESET);
        } else if ((player_choice == 0 && pirate_choice == 1) || (player_choice == 1 && pirate_choice == 0)) {
            printf("%sYou win this round!%s\n", COLOR_GREEN, COLOR_RESET);
            game->pirate.health -= 10; // Simplified damage calculation
        } else {
            printf("%sPirate wins this round!%s\n", COLOR_RED, COLOR_RESET);
            game->player.health -= 10; // Simplified damage calculation
        }

        printf("Player health: %s%d%s, Pirate health: %s%d%s\n", COLOR_GREEN, game->player.health, COLOR_RESET, COLOR_RED, game->pirate.health, COLOR_RESET);
    }

    if (game->player.health > 0) {
        printf("%sYou defeated the pirates!%s\n", COLOR_GREEN, COLOR_RESET);
        game->player.money += 100; // Earn money for defeating pirates
    } else {
        printf("%sYou were defeated by the pirates!%s\n", COLOR_RED, COLOR_RESET);
    }
}

// Visit Earth Starbase
void visit_starbase(Game *game) {
    int choice;
    printf("%sWelcome to Earth Starbase!%s\n", COLOR_BLUE, COLOR_RESET);
    printf("1. %sPurchase Weapons%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("2. %sRefuel Ship%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("3. %sLeave Starbase%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            purchase_weapons(game);
            break;
        case 2:
            refuel_ship(game);
            break;
        case 3:
            printf("%sLeaving Earth Starbase.%s\n", COLOR_CYAN, COLOR_RESET);
            break;
        default:
            printf("%sInvalid choice!%s\n", COLOR_RED, COLOR_RESET);
            break;
    }
}

// Purchase weapons at the starbase
void purchase_weapons(Game *game) {
    Weapon available_weapons[] = {
        {"Laser", 10, 100},
        {"Plasma Cannon", 20, 200},
        {"Photon Torpedo", 30, 300}
    };

    int choice;
    printf("Available Weapons:\n");
    for (int i = 0; i < 3; i++) {
        printf("%d. %s (Damage: %d, Cost: %d)\n", i + 1, available_weapons[i].name, available_weapons[i].damage, available_weapons[i].cost);
    }
    printf("Enter the number of the weapon to purchase: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > 3) {
        printf("Invalid choice!\n");
        return;
    }

    Weapon selected_weapon = available_weapons[choice - 1];
    if (game->player.money < selected_weapon.cost) {
        printf("Not enough money to purchase this weapon!\n");
        return;
    }

    for (int i = 0; i < MAX_WEAPONS; i++) {
        if (strcmp(game->player.weapons[i].name, "Empty") == 0) {
            game->player.weapons[i] = selected_weapon;
            game->player.money -= selected_weapon.cost;
            printf("Purchased %s for %d money.\n", selected_weapon.name, selected_weapon.cost);
            return;
        }
    }

    printf("No empty slots available to purchase this weapon!\n");
}

// Refuel the ship at the starbase
void refuel_ship(Game *game) {
    int fuel_needed = game->player.max_fuel - game->player.fuel;
    int cost = fuel_needed * 2; // Assume refuel cost is 2 money per unit of fuel

    if (game->player.money < cost) {
        printf("Not enough money to refuel the ship!\n");
        return;
    }

    game->player.fuel = game->player.max_fuel;
    game->player.money -= cost;
    printf("Refueled the ship for %d money.\n", cost);
}

// Generate a random message for the player
void generate_message(Game *game) {
    int rand_event = rand() % 3; // Generate a random event (0 to 2)

    switch (rand_event) {
        case 0:
            strcpy(game->message, "You have received an SOS signal from a nearby ship.");
            break;
        case 1:
            strcpy(game->message, "Pirates are ambushing you! Prepare for battle.");
            scan_for_pirates(game); // Automatically engage in battle
            break;
        case 2:
            strcpy(game->message, "All systems are normal. Continue your journey.");
            break;
        default:
            strcpy(game->message, "Unknown event.");
            break;
    }
}

// Main game loop
void game_loop(Game *game) {
    int choice;

    while (1) {
        display_grid(game);
        display_player_status(game);
        printf("ETA to destination: %d turns\n", game->player.eta);
        printf("Message: %s\n", game->message);
        
        printf("1. Move to a sector\n");
        printf("2. Scan for pirates\n");
        printf("3. Visit Earth Starbase\n");
        printf("4. Quit game\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                move_player(game);
                break;
            case 2:
                scan_for_pirates(game);
                break;
            case 3:
                visit_starbase(game);
                break;
            case 4:
                printf("Quitting game. Final status: Health = %d, Fuel = %d, Money = %d\n", game->player.health, game->player.fuel, game->player.money);
                return;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }

        // Generate a new message and update ETA each turn
        generate_message(game);
        if (game->player.eta > 0) {
            game->player.eta--;
        }

        if (game->player.health <= 0) {
            printf("Game over! Your ship was destroyed.\n");
            return;
        }
    }
}
