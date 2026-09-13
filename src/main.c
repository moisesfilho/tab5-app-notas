/**
 * @file main.c
 * @brief Aplicativo de Notas para Tab5 OS
 */

#include "tab5_sdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char s_current_file[256] = {0};
static tab5_ui_obj_t s_new_button = TAB5_UI_INVALID_OBJ;
static tab5_ui_obj_t s_save_button = TAB5_UI_INVALID_OBJ;

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
    tab5_ui_obj_t ta = tab5_ui_get_main_textarea();
    if (ta != TAB5_UI_INVALID_OBJ) {
        tab5_ui_textarea_set_text(ta, "");
    }
    update_app_title();
    tab5_ui_show_toast("Nova nota criada", 1500);
}

static void on_save_note(void *user_data)
{
    (void)user_data;
    char generated_path[256];
    char timestamp[32];
    const char *target = s_current_file;
    if (target[0] == '\0') {
        if (tab5_system_get_timestamp(timestamp, sizeof(timestamp)) != TAB5_OK) {
            tab5_ui_show_toast("Erro ao obter data e hora", 2000);
            return;
        }
        snprintf(generated_path, sizeof(generated_path), "/sdcard/notas/nota-%s.txt", timestamp);
        target = generated_path;
    }
    if (tab5_storage_mkdir("/sdcard/notas") != TAB5_OK) {
        tab5_ui_show_toast("Erro ao preparar armazenamento", 2000);
        return;
    }
    tab5_ui_obj_t ta = tab5_ui_get_main_textarea();
    const char *content = (ta != TAB5_UI_INVALID_OBJ) ? tab5_ui_textarea_get_text(ta) : "";
    content = content != NULL ? content : "";
    if (tab5_storage_write_file(target, content, strlen(content)) == TAB5_OK) {
        strncpy(s_current_file, target, sizeof(s_current_file) - 1);
        s_current_file[sizeof(s_current_file) - 1] = '\0';
        update_app_title();
        char message[256];
        snprintf(message, sizeof(message), "Nota salva: %s", s_current_file);
        tab5_system_log(2, "tab5_notas", message);
        tab5_ui_show_toast(message, 3000);
        tab5_sound_play_beep(1200, 100);
    } else {
        tab5_ui_show_toast("Erro ao gravar arquivo", 2000);
    }
}

static void app_init(void)
{
    tab5_system_log(2, "tab5_notas", "Aplicativo Notas iniciado");
    update_app_title();
    s_new_button = tab5_ui_app_bar_add_action_button("LV_SYMBOL_PLUS", on_new_note, NULL);
    s_save_button = tab5_ui_app_bar_add_action_button("LV_SYMBOL_SAVE", on_save_note, NULL);
    
    tab5_ui_obj_t ta = tab5_ui_get_main_textarea();
    if (ta != TAB5_UI_INVALID_OBJ) {
        tab5_ui_textarea_set_placeholder(ta, "Escreva sua nota...");
    }
    tab5_ui_show_toast("Notas aberto", 1500);
}

TAB5_APP_EXPORT void tab5_app_on_ui_event(tab5_ui_obj_t obj, uint32_t event_type, int32_t event_val)
{
    (void)event_val;
    if (event_type != TAB5_UI_EVENT_CLICKED) {
        return;
    }
    if (obj == s_new_button) {
        on_new_note(NULL);
    } else if (obj == s_save_button) {
        on_save_note(NULL);
    }
}

static void app_open_file(const char *filepath)
{
    if (filepath != NULL && filepath[0] != '\0') {
        strncpy(s_current_file, filepath, sizeof(s_current_file) - 1);
        update_app_title();
        
        char *buf = (char *)malloc(65536);
        size_t read_bytes = 0;
        if (buf != NULL &&
            tab5_storage_read_file(filepath, buf, 65536, &read_bytes) == TAB5_OK) {
            tab5_ui_obj_t ta = tab5_ui_get_main_textarea();
            if (ta != TAB5_UI_INVALID_OBJ) {
                tab5_ui_textarea_set_text(ta, buf);
            }
        }
        free(buf);
        
        char msg[128];
        snprintf(msg, sizeof(msg), "Arquivo aberto: %s", filepath);
        tab5_ui_show_toast(msg, 2000);
    }
}

TAB5_APP_EXPORT void tab5_app_on_open_file(const char *path)
{
    app_open_file(path);
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
    app_init();
    return 0;
}
