#include "SettingsScreen.h"

#include <RPC_internal.h>
#include "ScreenHeading.h"
#include "OnboardingScreen.h"

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *settingsScreen;

static lv_indev_t *encoderIndev;
static lv_group_t *mainGroup;

// ---------------------------------------------------------
// heading
static lv_obj_t *btnBack;
static lv_obj_t *btnBackLabel;

static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

// ---------------------------------------------------------
// main elements
static lv_obj_t *gridLayout;
static lv_style_t gridLayoutStyle;

static lv_obj_t *labelFrequency;
static lv_obj_t *dropdownFrequency;

static lv_obj_t *labelSampleLength;
static lv_obj_t *spinboxSampleLength;

// ---------------------------------------------------------
// callbacks
static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnBack) {
            onboarding_screen_load();
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (obj == dropdownFrequency) {
            int frequencySelect = lv_dropdown_get_selected(dropdownFrequency);
            RPC1.println("SET settings/frequency " + String(frequencySelect));
        } else if (obj == spinboxSampleLength) {
            int sampleLength = lv_spinbox_get_value(spinboxSampleLength);
            RPC1.println("SET settings/sample-length " + String(sampleLength));
        }
    }
}

// ---------------------------------------------------------
// header function implementations
void settings_screen_create(lv_indev_t *_encoderIndev) {
    // init screen
    settingsScreen = lv_obj_create(NULL);

    mainGroup = lv_group_create();
    encoderIndev = _encoderIndev;

    // init back button
    static lv_style_t btnBackStyle = screen_heading_get();
    
    btnBack = lv_btn_create(settingsScreen);
    lv_obj_add_style(btnBack, &btnBackStyle, 0);
    lv_obj_align(btnBack, LV_ALIGN_TOP_LEFT, SCREEN_HEADING_BUTTON_X, SCREEN_HEADING_BUTTON_Y);
    lv_obj_set_size(btnBack, SCREEN_HEADING_BUTTON_W, SCREEN_HEADING_BUTTON_H);
    lv_obj_add_event_cb(btnBack, handleButtonClick, LV_EVENT_CLICKED, NULL);

    btnBackLabel = lv_label_create(btnBack);
    lv_label_set_recolor(btnBackLabel, true);
    lv_label_set_text(btnBackLabel, "#121212 " LV_SYMBOL_LEFT);
    lv_obj_center(btnBackLabel);

    // init heading
    lv_style_init(&labelHeadingStyle);
    lv_style_set_text_font(&labelHeadingStyle, &lv_font_montserrat_26);

    labelHeading = lv_label_create(settingsScreen);
    lv_label_set_text(labelHeading, "Settings");
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, SCREEN_HEADING_LABEL_X, SCREEN_HEADING_LABEL_Y);
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);

    // grid layout
    lv_style_init(&gridLayoutStyle);
    lv_style_set_bg_opa(&gridLayoutStyle, LV_OPA_TRANSP);
    lv_style_set_border_opa(&gridLayoutStyle, LV_OPA_TRANSP);
    lv_style_set_pad_all(&gridLayoutStyle, 10);

    static lv_coord_t col_dsc[] = { 100, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static lv_coord_t row_dsc[] = { 40, 40, LV_GRID_TEMPLATE_LAST };

    gridLayout = lv_obj_create(settingsScreen);
    lv_obj_set_grid_dsc_array(gridLayout, col_dsc, row_dsc);
    lv_obj_set_size(gridLayout, 320, 145);
    lv_obj_add_style(gridLayout, &gridLayoutStyle, 0);
    lv_obj_align(gridLayout, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // frequency part
    labelFrequency = lv_label_create(gridLayout);
    lv_label_set_text(labelFrequency, "Frequency:");
    lv_obj_set_grid_cell(labelFrequency, LV_GRID_ALIGN_START, 0, 1, // col
                                         LV_GRID_ALIGN_CENTER, 0, 1); // row

    dropdownFrequency = lv_dropdown_create(gridLayout);
    lv_dropdown_set_options(dropdownFrequency, "1 Hz\n"
                                               "2 Hz\n"
                                               "4 Hz\n"
                                               "5 Hz\n"
                                               "10 Hz\n"
                                               "20 Hz\n"
                                               "40 Hz\n"
                                               "50 Hz\n"
                                               "100 Hz\n"
                                               "200 Hz\n"
                                               "400 Hz\n"
                                               "500 Hz\n");
    lv_obj_set_grid_cell(dropdownFrequency, LV_GRID_ALIGN_END, 1, 1, // col
                                            LV_GRID_ALIGN_CENTER, 0, 1); // row
    lv_dropdown_set_selected(dropdownFrequency, 8);
    lv_obj_add_event_cb(dropdownFrequency, handleButtonClick, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(mainGroup, dropdownFrequency);

    // sample-length part
    labelSampleLength = lv_label_create(gridLayout);
    lv_label_set_text(labelSampleLength, "Sample length (ms):");
    lv_obj_set_grid_cell(labelSampleLength, LV_GRID_ALIGN_START, 0, 1, // col
                                            LV_GRID_ALIGN_CENTER, 1, 1); // row

    spinboxSampleLength = lv_spinbox_create(gridLayout);
    lv_spinbox_set_digit_format(spinboxSampleLength, 5, 0);
    lv_spinbox_set_range(spinboxSampleLength, 0, 99999);
    lv_spinbox_set_value(spinboxSampleLength, 1000);
    lv_obj_set_grid_cell(spinboxSampleLength, LV_GRID_ALIGN_END, 1, 1, // col
                                              LV_GRID_ALIGN_CENTER, 1, 1); // row
    lv_obj_add_event_cb(spinboxSampleLength, handleButtonClick, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(mainGroup, spinboxSampleLength);
    
    // lastly, add the back button to the group
    lv_group_add_obj(mainGroup, btnBack);
}

void settings_screen_load() {
    lv_scr_load(settingsScreen);
    lv_indev_set_group(encoderIndev, mainGroup);

    RPC1.println("GET settings/frequency");
    RPC1.println("GET settings/sample-length");
}

void settings_screen_receive_message(String command, String subject, String payload) {
    if (command == "POST") {
        if (subject == "settings/frequency") {
            lv_dropdown_set_selected(dropdownFrequency, payload.toInt());
        } else if (subject == "settings/sample-length") {
            lv_spinbox_set_value(spinboxSampleLength, payload.toInt());
        }
    }
}