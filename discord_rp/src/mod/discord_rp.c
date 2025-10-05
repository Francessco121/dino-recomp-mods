#include "modding.h"
#include "recomputils.h"
// #include "discord_game_sdk.h"

// static void DISCORD_REQUIRE(enum EDiscordResult x) {
//     if (x != DiscordResult_Ok) {
//         recomp_exit_with_error(recomp_sprintf_helper("Discord Error: %X", x));
//     }
// }

// typedef struct {
//     struct IDiscordCore* core;
//     struct IDiscordUsers* users;
// } Application;

// static Application app;
// static IDiscordCoreEvents events;

// static void DISCORD_CALLBACK UpdateActivityCallback(void* data, enum EDiscordResult result)
// {
//     DISCORD_REQUIRE(result);
// }
RECOMP_IMPORT(".", int native_lib_test(const char* str));

RECOMP_HOOK("game_init") void discord_rp_game_init() {
    recomp_printf("discord_rp init\n");

    int retVal = native_lib_test("String from mod code!");

    recomp_printf("'native_lib_test' returned %i\n", retVal);

    // struct DiscordCreateParams params;
    // DiscordCreateParamsSetDefault(&params);
    // params.client_id = 1424118706157518870;
    // params.flags = DiscordCreateFlags_Default;
    // params.events = &events;
    // params.event_data = &app;

    // DISCORD_REQUIRE(DiscordCreate(DISCORD_VERSION, &params, &app.core));

    // struct IDiscordActivityManager *activity_manager = app.core->get_activity_manager(app.core);

    // struct DiscordActivity activity;
    // bzero(&activity, sizeof(activity));
    // recomp_sprintf(activity.details, "hello?");

    // activity_manager->update_activity(activity_manager, &activity, &app, UpdateActivityCallback);
}

RECOMP_CALLBACK("*", recomp_on_game_tick) void discord_rp_game_tick() {
    //DISCORD_REQUIRE(app.core->run_callbacks(app.core));
}
