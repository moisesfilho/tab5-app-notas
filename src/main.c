/**
 * @file main.c
 * @brief Aplicativo de Notas para Tab5 OS
 */

#include "tab5_sdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char s_current_file[256] = {0};

static void update_app_title(void)
{
    if (s_current_file[0] == '\0') {
        tab5_ui_app_bar_set_title("Notas - Sem titulo");
    } else {
        const char *last_slash = strrchr(s_current_file, '/');
        const char *filename = (last_slash != NULL) ? last_slash + 1 : s_current_file;
        char title[128];
        snprintf(title, sizeof(title), "Notas - %s", filename);
        tab5_ui_app_bar_set_title(title);
    }
}

static void on_new_note(void *user_data)
{
    (void)user_data;
    s_current_file[0] = '\0';
    update_app_title();
    tab5_ui_show_toast("Nova nota criada", 1500);
}

static void on_save_note(void *user_data)
{
    (void)user_data;
    char path[256];
    const char *target = (s_current_file[0] != '\0') ? s_current_file : "nota.txt";
    tab5_err_t err = tab5_storage_path_resolve(target, path, sizeof(path), true);
    if (err == TAB5_OK) {
        tab5_system_log(2, "tab5_notas", "Salvando nota em sandbox...");
        tab5_ui_show_toast("Nota salva com sucesso!", 2000);
        tab5_sound_play_beep(1200, 100);
    } else {
        tab5_ui_show_toast("Erro de permissao ao salvar", 2500);
    }
}

static void app_init(void)
{
    tab5_system_log(2, "tab5_notas", "Aplicativo Notas iniciado");
    update_app_title();
    tab5_ui_app_bar_add_action_button("LV_SYMBOL_PLUS", on_new_note, NULL);
    tab5_ui_app_bar_add_action_button("LV_SYMBOL_SAVE", on_save_note, NULL);
    tab5_ui_show_toast("Notas aberto", 1500);
}

static void app_open_file(const char *filepath)
{
    if (filepath != NULL && filepath[0] != '\0') {
        strncpy(s_current_file, filepath, sizeof(s_current_file) - 1);
        update_app_title();
        char msg[128];
        snprintf(msg, sizeof(msg), "Arquivo aberto: %s", filepath);
        tab5_ui_show_toast(msg, 2000);
    }
}

static void app_resume(void)
{
    tab5_system_log(2, "tab5_notas", "Notas retomado ao primeiro plano");
}

static void app_pause(void)
{
    tab5_system_log(2, "tab5_notas", "Notas pausado");
}

static void app_destroy(void)
{
    tab5_system_log(2, "tab5_notas", "Notas finalizado");
}

TAB5_APP_EXPORT int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tab5_lifecycle_callbacks_t cbs = {
        .on_init = app_init,
        .on_resume = app_resume,
        .on_pause = app_pause,
        .on_destroy = app_destroy,
        .on_open_file = app_open_file,
    };

    tab5_lifecycle_register(&cbs);
    return 0;
}
