#include "game_state.h"

static void initialize_player_names(GameState *game) {
    uint8_t player_id;
    char name[3];

    name[0] = 'P';
    name[2] = '\0';
    for (player_id = 0u; player_id < game->player_count; player_id++) {
        name[1] = (char)('1' + player_id);
        player_init(
            &game->players[player_id],
            player_id,
            name,
            game->starting_life
        );
    }
}

static void clear_commander_damage(GameState *game) {
    uint8_t target;
    uint8_t source;
    uint8_t commander_slot;

    for (target = 0u; target < MAX_PLAYERS; target++) {
        for (source = 0u; source < MAX_PLAYERS; source++) {
            for (
                commander_slot = 0u;
                commander_slot < MAX_COMMANDERS_PER_PLAYER;
                commander_slot++
            ) {
                game->commander_damage[target][source][commander_slot] = 0u;
            }
        }
    }
}

void game_state_reset(GameState *game) {
    uint8_t player_id;

    for (player_id = 0u; player_id < game->player_count; player_id++) {
        player_reset(&game->players[player_id], game->starting_life);
    }
    clear_commander_damage(game);
    game->turn_number = 1u;
    game->active_player = 0;
    game->monarch_player = NO_PLAYER;
    game->initiative_player = NO_PLAYER;
    game->storm_count = 0u;
    game->day_night_state = 0u;
}

void game_state_init(
    GameState *game,
    uint8_t player_count,
    int16_t starting_life
) {
    if (player_count < MIN_PLAYERS) {
        player_count = MIN_PLAYERS;
    } else if (player_count > MAX_PLAYERS) {
        player_count = MAX_PLAYERS;
    }

    game->player_count = player_count;
    game->starting_life = starting_life;
    game->poison_threshold = DEFAULT_POISON_THRESHOLD;
    game->commander_damage_threshold = DEFAULT_COMMANDER_DAMAGE_THRESHOLD;
    game->partner_mode_enabled = 0u;
    game->confirm_eliminations = 1u;

    initialize_player_names(game);
    game_state_reset(game);
}
