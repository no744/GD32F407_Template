SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_night_clould_png.c
    images/ui_img_tempersture_png.c
    images/ui_img_humidity_png.c
    fonts/ui_font_Font1.c)

add_library(ui ${SOURCES})
